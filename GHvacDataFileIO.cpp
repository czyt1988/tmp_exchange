#include "GHvacDataFileIO.h"
#include <QDebug>
#include <QDir>
#include "quazipfile.h"
#include <QTextStream>
#include "SACsvStream.h"
#include "SATable.h"
#include <QDateTime>
#include <QElapsedTimer>
GHvacDataFileIO::GHvacDataFileIO(QObject *p) : QObject(p)
    , mIsOpen(false)
{
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
    mTables.clear();
    if (nullptr == mZip) {
        mZip = std::make_shared<QuaZip>();
    }
    mZip->setZipName(mFileName);
    mIsOpen = mZip->open(QuaZip::mdUnzip);
    if (!mIsOpen) {
        emit error(QStringLiteral("无法打开文件%1").arg(mFileName));
    }
    mFilesList = mZip->getFileNameList();
    QList<TablePtr> originTables;

    for (const QString& f : mFilesList)
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
        table->setRowNames(header);
        QVector<double> rd;
        while (!csv.atEnd())
        {
            QStringList line = csv.readCsvLine();
            converStringListToDoubleList(line, rd);
            if (dt1 >= 0) {
                rd[dt1] = QDateTime::fromString(line[dt1], "yyyy-MM-dd HH:mm:ss").toSecsSinceEpoch();
            }
            if (dt2 >= 0) {
                rd[dt2] = QDateTime::fromString(line[dt2], "yyyy-MM-dd HH:mm:ss").toSecsSinceEpoch();
            }
            table->appendColumn(rd.begin(), rd.end());
        }
        originTables.append(table);
        emit message(QStringLiteral("读取%1完成，耗时%2").arg(table->getName()).arg(tic.elapsed()));
    }
    //开始对can ip进行聚合
    groupByCanIP(originTables);
    unionDateTime();
    emit readed(mTables);
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
            data = std::nan("");
        }
        d[i] = data;
    }
}


void GHvacDataFileIO::groupByCanIP(QList<TablePtr> tables)
{
    QString canipfield = "can1 ip";

    for (TablePtr t : tables)
    {
        if (t->getName() == "system") {
            mTables.append(t);
        }else if (t->getName() == "module") {
            QElapsedTimer tic;
            tic.start();
            QPair<QList<typename TableType::TablePtr>, QList<typename TableType::Type> > res = t->groupBy(canipfield);
            QString info(QStringLiteral("module 表包含can ip:"));
            for (int i = 0; i < res.first.size(); ++i)
            {
                info += (QString::number(res.second[i]) + ",");
                res.first[i]->setName(QString("module_%1").arg(int(res.second[i])));
                mTables.append(res.first[i]);
            }
            emit message(info+QStringLiteral(" 耗时:%1").arg(tic.elapsed()));
        }else if (t->getName() == "idu") {
            QElapsedTimer tic;
            tic.start();
            QPair<QList<typename TableType::TablePtr>, QList<typename TableType::Type> > res = t->groupBy(canipfield);
            QString info(QStringLiteral("idu 表包含can ip:"));
            for (int i = 0; i < res.first.size(); ++i)
            {
                info += (QString::number(res.second[i]) + ",");
                res.first[i]->setName(QString("idu_%1").arg(int(res.second[i])));
                mTables.append(res.first[i]);
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
        for (TablePtr t : mTables)
        {
            TableType::SeriesPtr r = t->row(datetimefield);
            if (r) {
                datetime += (r->toList().toSet());
            }
        }
        unionDatetime = datetime.toList();
        std::sort(unionDatetime.begin(), unionDatetime.end());
    }
    message(QStringLiteral("完成时间提取，共%1个数据表，时间并集计算得到时间个数为%2个").arg(mTables.size()).arg(unionDatetime.size()));
}
