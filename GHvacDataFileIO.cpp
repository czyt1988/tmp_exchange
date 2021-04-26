#include "GHvacDataFileIO.h"
#include <QDebug>
#include <QDir>
#include "quazipfile.h"
#include <QTextStream>
#include "SACsvStream.h"
#include "SATable.h"
#include <QDateTime>
#include <QElapsedTimer>
#include <numeric>
#include <cmath>
GHvacDataFileIO::GHvacDataFileIO(QObject *p) : QObject(p)
    , mIsOpen(false)
    , mCanipfield("can1 ip")
{
    qRegisterMetaType<GHvacDataInfo>();
    setDatetimeField(QStringLiteral("记录时间"));
}


GHvacDataFileIO::~GHvacDataFileIO()
{
    qDebug() << "delete GHvacDataFileIO";
}


void GHvacDataFileIO::setFileName(const QString& filepath)
{
    mFileName = filepath;
}


void GHvacDataFileIO::open(const QString& filepath)
{
    setFileName(filepath);
    mHvacInfo.clear();
    if (nullptr == mZip) {
        mZip = std::make_shared<QuaZip>();
    }
    mZip->setZipName(mFileName);
    mIsOpen = mZip->open(QuaZip::mdUnzip);
    if (!mIsOpen) {
        emit error(QStringLiteral("无法打开文件%1").arg(mFileName));
    }
    mHvacInfo.filesList = mZip->getFileNameList();
    QList<TablePtr> originTables;

    for (const QString& f : mHvacInfo.filesList)
    {
        QElapsedTimer tic;
        tic.start();
        emit message(QStringLiteral("开始读取%1").arg(f));
        QStringList flist = f.split('.');
        if (flist.size() > 1) {
            //把csv去掉
            flist.pop_back();
        }
        TablePtr table = std::make_shared<TableType>();
        table->setName(flist.join(' '));
        mZip->setCurrentFile(f);
        QuaZipFile archived(mZip.get());
        if (!archived.open(QIODevice::ReadOnly, nullptr)) {
            continue;
        }
        QTextStream s(&archived);
        s.setCodec("UTF-8");
        SACsvStream csv(&s);
        QStringList header = csv.readCsvLine();
        int dt1 = header.indexOf(QStringLiteral("记录时间"));
        int dt2 = header.indexOf(QStringLiteral("接收时间"));
        //确定时间格式
        bool havesuretimeformat = false;
        QString dfmt = "yyyy/M/d H:m:s";
        table->setRowNames(header);
        QVector<double> rd;
        while (!csv.atEnd())
        {
            QStringList line = csv.readCsvLine();
            converStringListToDoubleList(line, rd);
            if (dt1 >= 0) {
                if (!havesuretimeformat) {
                    havesuretimeformat = true;
                    if (line[dt1].count(':') == 1) {
                        dfmt = "yyyy/M/d H:m";
                    }
                }
                rd[dt1] = QDateTime::fromString(line[dt1], dfmt).toSecsSinceEpoch();
            }
            if (dt2 >= 0) {
                rd[dt2] = QDateTime::fromString(line[dt2], dfmt).toSecsSinceEpoch();
            }
            table->appendColumn(rd.begin(), rd.end());
        }

        originTables.append(table);
        emit message(QStringLiteral("读取%1完成，耗时%2").arg(table->getName()).arg(tic.elapsed()));
    }
    //开始对can ip进行聚合
    groupByCanIP(originTables);
    unionDateTime();
    orderByDatetime();
    qDebug() << filepath << QStringLiteral("，完成解析");
    emit readed(mHvacInfo);
}


GHvacDataFileIO::Error GHvacDataFileIO::getError() const
{
    if (nullptr == mZip) {
        return (UnknowError);
    }
    int err = mZip->getZipError();

    switch (err)
    {
    case ZIP_PARAMERROR:
        return (ParamError);

    case ZIP_BADZIPFILE:
        return (BadDataFileError);

    case ZIP_INTERNALERROR:
        return (InternalError);

    default:
        break;
    }
    return (UnknowError);
}


void GHvacDataFileIO::setDatetimeField(const QString& f)
{
    mHvacInfo.fieldOfDatetime = f;
}


void GHvacDataFileIO::setCanipField(const QString& canipfield)
{
    mCanipfield = canipfield;
}


void GHvacDataFileIO::converStringListToDoubleList(const QStringList& str, QVector<double>& d)
{
    if (d.size() != str.size()) {
        d.resize(str.size());
    }
    int s = str.size();
    double data = 0;
    bool isOK = false;

    for (int i = 0; i < s; ++i)
    {
        data = str[i].toDouble(&isOK);
        if (!isOK) {
            data = NAN;
        }
        d[i] = data;
    }
}


void GHvacDataFileIO::groupByCanIP(QList<TablePtr> tables)
{
    QString canipfield = mCanipfield;

    for (TablePtr t : tables)
    {
        if (t->getName() == "system") {
            mHvacInfo.tables.append(t);
        }else if (t->getName() == "module") {
            QElapsedTimer tic;
            tic.start();
            QPair<QList<typename TableType::TablePtr>, QList<typename TableType::Type> > res = t->groupBy(canipfield);
            QString info(QStringLiteral("module 表包含can ip:"));
            for (int i = 0; i < res.first.size(); ++i)
            {
                mHvacInfo.moduleCanIPs.append(res.second[i]);
                info += (QString::number(res.second[i]) + ",");
                res.first[i]->setName(QString("module_%1").arg(int(res.second[i])));
                mHvacInfo.tables.append(res.first[i]);
            }
            emit message(info+QStringLiteral(" 耗时:%1").arg(tic.elapsed()));
        }else if (t->getName() == "idu") {
            QElapsedTimer tic;
            tic.start();
            QPair<QList<typename TableType::TablePtr>, QList<typename TableType::Type> > res = t->groupBy(canipfield);
            QString info(QStringLiteral("idu 表包含can ip:"));
            for (int i = 0; i < res.first.size(); ++i)
            {
                mHvacInfo.iduCanIPs.append(res.second[i]);
                info += (QString::number(res.second[i]) + ",");
                res.first[i]->setName(QString("idu_%1").arg(int(res.second[i])));
                mHvacInfo.tables.append(res.first[i]);
            }
            emit message(info+QStringLiteral(" 耗时:%1").arg(tic.elapsed()));
        }
    }
}


/**
 * @brief 时标对齐
 */
void GHvacDataFileIO::unionDateTime()
{
    //开始对mTables的时间轴进行对齐设置
    message(QStringLiteral("开始合并时间"));
    QElapsedTimer tic;

    tic.start();
    QString datetimefield = QStringLiteral("记录时间");
    QList<TableType::Type> unionDatetime;

    if (true) {
        QSet<TableType::Type> datetime;
        for (TablePtr t : mHvacInfo.tables)
        {
            qDebug() << "name:" << t->getName() << " r:" << t->rowCount() << " c:" << t->columnCount();
            TableType::SeriesPtr r = t->row(datetimefield);
            if (r) {
                qDebug() << "r.size:" << r->size();
                datetime += (r->toList().toSet());
                qDebug() << "datetime size:"<<datetime.size();
            }
        }
        unionDatetime = datetime.toList();
        std::sort(unionDatetime.begin(), unionDatetime.end());
        for (TableType::Type t : unionDatetime)
        {
            mHvacInfo.allDateTimeScale.append(QDateTime::fromSecsSinceEpoch(t));
        }
    }
    message(QStringLiteral("完成时间提取，共%1个数据表，时间并集计算得到时间个数为%2个，耗时%3").arg(mHvacInfo.tables.size()).arg(unionDatetime.size()).arg(tic.elapsed()));
    if (unionDatetime.size() > 5) {
        qDebug()	<< unionDatetime[0] << unionDatetime[1] << unionDatetime[2]
                << "..." << *(unionDatetime.end()-3)<< *(unionDatetime.end()-2)<< *(unionDatetime.end()-1);
        message(QStringLiteral("时间范围%1~%2")
            .arg(mHvacInfo.allDateTimeScale.first().toString(Qt::ISODate))
            .arg(mHvacInfo.allDateTimeScale.back().toString(Qt::ISODate)));
    }
}


void GHvacDataFileIO::orderByDatetime()
{
    QString datetimefield = QStringLiteral("记录时间");
    QElapsedTimer tic;

    tic.start();
    for (TablePtr t : mHvacInfo.tables)
    {
        t->orderBy(datetimefield);
    }
    message(QStringLiteral("完成所有表时间排序，耗时%1").arg(tic.elapsed()));
    for (TablePtr t : mHvacInfo.tables)
    {
        qDebug() << t->getName();
        qDebug() << *t;
    }
}
