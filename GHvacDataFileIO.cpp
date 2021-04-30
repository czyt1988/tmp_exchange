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
#include <QJsonDocument>
#include <QJsonObject>
#include <cmath>
GHvacDataFileIO::GHvacDataFileIO(QObject *p) : QObject(p)
    , mIsOpen(false)
{
    qRegisterMetaType<GHvacDataInfo>();
    mSetting.codec = QStringLiteral("UTF-8");
    mSetting.canipfield = QStringLiteral("can1 ip");
    mSetting.datetimefield = QStringLiteral("记录时间");
    mSetting.datetimeformat = "yyyy/M/d H:m:s";
    mSetting.table_system = QStringLiteral("system.csv");
    mSetting.table_module = QStringLiteral("module.csv");
    mSetting.table_idu = QStringLiteral("idu.csv");
    mSetting.tablename_system = toTableName(mSetting.table_system);
    mSetting.tablename_module = toTableName(mSetting.table_module);
    mSetting.tablename_idu = toTableName(mSetting.table_idu);
    setDatetimeField(mSetting.datetimefield);
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
    if (!loadSetting()) {
        return;
    }

    QList<TablePtr> originTables;

    for (const QString& f : mHvacInfo.filesList)
    {
        if (!f.contains("csv")) {
            continue;
        }
        try{
            QElapsedTimer tic;
            tic.start();
            emit message(QStringLiteral("开始读取%1").arg(f));
            TablePtr table = std::make_shared<TableType>();
            table->setName(toTableName(f));
            mZip->setCurrentFile(f);
            int linecount = getLineCount();
            QuaZipFile archived(mZip.get());
            if (!archived.open(QIODevice::ReadOnly, nullptr)) {
                continue;
            }
            QTextStream s(&archived);
            s.setCodec(mSetting.codec.toLocal8Bit().data());
            SACsvStream csv(&s);
            QStringList header = csv.readCsvLine();
            int dt1 = header.indexOf(mSetting.datetimefield);
            int dt2 = header.indexOf(QStringLiteral("接收时间"));
            table->setRowNames(header);
            qDebug() << QStringLiteral("数据文件:") << f << QStringLiteral(" 数据头:") << header;
            QVector<double> rd;
            //预先分配好内存，避免溢出
            QString msg = QStringLiteral("%1表预估内存:%2 MB").arg(toTableName(f)).arg(double(linecount*header.size()*sizeof(NumType))/1024.0/1024.0);
            emit message(msg);
            qDebug() << msg;
            table->reserve(linecount);
            qDebug() <<QStringLiteral("预分配内存完成");
            while (!csv.atEnd())
            {
                QStringList line = csv.readCsvLine();
                converStringListToDoubleList(line, rd);
                if (dt1 >= 0) {
                    rd[dt1] = formatDatetime(line[dt1]).toSecsSinceEpoch();
                }
                if (dt2 >= 0) {
                    rd[dt2] = formatDatetime(line[dt2]).toSecsSinceEpoch();
                }
                table->appendColumn(rd.begin(), rd.end());
            }

            originTables.append(table);
            emit message(QStringLiteral("读取%1完成，数据大小(%2,%3)耗时%4").arg(table->getName()).arg(table->rowCount()).arg(table->columnCount()).arg(tic.elapsed()));
        }
        catch (std::bad_alloc e) {
            qDebug() << QStringLiteral("内存溢出");
            emit error(QStringLiteral("内存溢出，数据文件过大，请裁剪"));
            return;
        }
    }
    //开始对can ip进行聚合
    groupByCanIP(originTables);
    unionDateTime();
    orderByDatetime();
    qDebug() << filepath << QStringLiteral("完成解析");
    emit readed(mHvacInfo);
}


QString GHvacDataFileIO::toTableName(const QString& csvname)
{
    QStringList flist = csvname.split('.');

    if (flist.size() > 1) {
        //把csv去掉
        flist.pop_back();
    }
    return (flist.join(' '));
}


QDateTime GHvacDataFileIO::formatDatetime(const QString& str)
{
    QDateTime dt = QDateTime::fromString(str, mSetting.datetimeformat);

    if (dt.isValid()) {
        return (dt);
    }
    QString dfmt = "yyyy/M/d H:m";

    dt = QDateTime::fromString(str, dfmt);
    if (dt.isValid()) {
        return (dt);
    }

    dfmt = "yyyy-MM-dd HH:mm:ss";
    dt = QDateTime::fromString(str, dfmt);
    if (dt.isValid()) {
        return (dt);
    }
    dfmt = "yyyy-MM-dd HH:mm";
    dt = QDateTime::fromString(str, dfmt);
    if (dt.isValid()) {
        return (dt);
    }
    dfmt = "yyyy-M-d H:m:s";
    dt = QDateTime::fromString(str, dfmt);
    if (dt.isValid()) {
        return (dt);
    }
    dfmt = "yyyy-M-d H:m";
    dt = QDateTime::fromString(str, dfmt);

    return (dt);
}


bool GHvacDataFileIO::loadSetting()
{
    int setindex = mZip->getFileNameList().indexOf("hvac-setting.json");

    if (setindex < 0) {
        return (true);
    }
    mZip->setCurrentFile(mHvacInfo.filesList[setindex]);
    QuaZipFile archived(mZip.get());

    if (!archived.open(QIODevice::ReadOnly, nullptr)) {
        emit message(QStringLiteral("无法打开hvac-setting.json，以默认模式打开"));
        return (true);
    }
    QTextStream s(&archived);
    QJsonParseError err;
    QJsonDocument json = QJsonDocument::fromJson(archived.readAll(), &err);

    if (json.isNull()) {
        emit message(QStringLiteral("文件异常:")+err.errorString());
        emit error(err.errorString());
        return (false);
    }
    QJsonObject obj = json.object();

    if (obj.empty()) {
        emit message(QStringLiteral("文件异常:配置信息读取错误"));
        emit error(QStringLiteral("文件异常:配置信息读取错误"));
        return (false);
    }
    mSetting.codec = obj["codec"].toString();
    mSetting.canipfield = obj["canipfield"].toString();
    mSetting.datetimefield = obj["datetimefield"].toString();
    mSetting.datetimeformat = obj["datetimeformat"].toString();
    QJsonObject tobj = obj["tables"].toObject();

    if (obj.empty()) {
        emit message(QStringLiteral("文件异常:配置信息读取错误[2]"));
        emit error(QStringLiteral("文件异常:配置信息读取错误[2]"));
        return (false);
    }
    mSetting.table_system = tobj["system"].toString();
    mSetting.table_module = tobj["module"].toString();
    mSetting.table_idu = tobj["idu"].toString();
    //转换tablename
    mSetting.tablename_system = toTableName(mSetting.table_system);
    mSetting.tablename_module = toTableName(mSetting.table_module);
    mSetting.tablename_idu = toTableName(mSetting.table_idu);
    qDebug()	<< "mSetting.codec:"<<mSetting.codec
            << "\n mSetting.canipfield:"<<mSetting.canipfield
            << "\n mSetting.canipfield:"<<mSetting.canipfield
            << "\n mSetting.datetimefield:"<<mSetting.datetimefield
            << "\n mSetting.datetimeformat:"<<mSetting.datetimeformat
            << "\n mSetting.table_system:"<<mSetting.table_system
            << "\n mSetting.table_module:"<<mSetting.table_module
            << "\n mSetting.table_idu:"<<mSetting.table_idu
    ;
    return (true);
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
    QString canipfield = mSetting.canipfield;

    for (TablePtr t : tables)
    {
        if (t->getName() == mSetting.tablename_system) {
            mHvacInfo.tables.append(t);
        }else if (t->getName() == mSetting.tablename_module) {
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
        }else if (t->getName() == mSetting.tablename_idu) {
            QElapsedTimer tic;
            tic.start();
            QPair<QList<typename TableType::TablePtr>, QList<typename TableType::Type> > res
                = t->groupBy(canipfield);
            QString info(QStringLiteral("idu 表包含can ip:"));
            QString info2(QStringLiteral(" 各数据量为:"));
            for (int i = 0; i < res.first.size(); ++i)
            {
                mHvacInfo.iduCanIPs.append(res.second[i]);
                info += (QString::number(res.second[i]) + ",");
                info2 += QString::number(res.first[i]->columnCount())+ ",";
                res.first[i]->setName(QString("idu_%1").arg(int(res.second[i])));
                mHvacInfo.tables.append(res.first[i]);
            }
            emit message(info+info2+QStringLiteral(" 耗时:%1").arg(tic.elapsed()));
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
    QString datetimefield = mSetting.datetimefield;
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
    qDebug() << "orderByDatetime";
    QString datetimefield = mSetting.datetimefield;
    QElapsedTimer tic;

    tic.start();
    for (TablePtr t : mHvacInfo.tables)
    {
        qDebug() << "orderBy:" << datetimefield << t->getName() << t->rowCount();
        int index = t->nameToIndex(datetimefield);
        if (index < 0) {
            message(QStringLiteral("%1表未找到时间字段%1").arg(t->getName()).arg(datetimefield));
            qDebug() << t->getName() << t->rowNames();
            continue;
        }
        t->orderBy(index);
    }
    message(QStringLiteral("完成所有表时间排序，耗时%1").arg(tic.elapsed()));
    for (TablePtr t : mHvacInfo.tables)
    {
        qDebug() << t->getName();
        qDebug() << *t;
    }
}


int GHvacDataFileIO::getLineCount()
{
    QuaZipFile archived(mZip.get());

    if (!archived.open(QIODevice::ReadOnly, nullptr)) {
        return (8);
    }
    QTextStream s(&archived);
    int l(0);

    while (!s.atEnd())
    {
        s.readLine();
        ++l;
    }
    qDebug() << QStringLiteral("行数：") << l;
    return (l);
}
