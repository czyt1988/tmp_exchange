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

const QStringList c_canipfield_stringlist = { QStringLiteral("can_ip"),
                          QStringLiteral("can ip"),
                          QStringLiteral("Canip"),
                          QStringLiteral("can1ip"),
                          QStringLiteral("can1 ip"),
                          QStringLiteral("can1_ip") };
const QStringList c_datetimefild_stringlist = { QStringLiteral("记录时间"),
                        QStringLiteral("RecordTime") };
const QStringList c_projectfild_stringlist = { QStringLiteral("工程ip"),
                           QStringLiteral("工程id"),
                           QStringLiteral("Proj_ID"),
                           QStringLiteral("projectid"),
                           QStringLiteral("project id"),
                           QStringLiteral("project_id"),
                           QStringLiteral("Projid"),
                           QStringLiteral("Proj_id")
};
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
    mSetting.projectidField = QStringLiteral("工程IP");
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
            table->setCaseSensitivity(TableType::CaseInsensitive);
            table->setMode(TableType::FixedMode);
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
            qDebug() << QStringLiteral("数据文件:") << f << QStringLiteral(" \n数据头:") << header;
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
                    rd[dt1] = formatDatetime(line[dt1]).toMSecsSinceEpoch();
                }
                if (dt2 >= 0) {
                    rd[dt2] = formatDatetime(line[dt2]).toMSecsSinceEpoch();
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

    //先提取工程id
    getProjectID(originTables);

    //开始对can ip进行聚合
    try{
        if (!groupByCanIP(originTables)) {
            emit error(QStringLiteral("聚合canip过程发生异常"));
            return;
        }
    }
    catch (std::bad_alloc e) {
        qDebug() << QStringLiteral("groupby过程内存溢出");
        emit error(QStringLiteral("groupby过程内存溢出，数据文件过大，请裁剪"));
        return;
    }


    try{
        unionDateTime();
    }
    catch (std::bad_alloc e) {
        qDebug() << QStringLiteral("时标对齐过程内存溢出");
        emit error(QStringLiteral("时标对齐过程内存溢出，数据文件过大，请裁剪"));
        return;
    }

    try{
        orderByDatetime();
    }
    catch (std::bad_alloc e) {
        qDebug() << QStringLiteral("按时间排序过程内存溢出");
        emit error(QStringLiteral("按时间排序过程内存溢出，数据文件过大，请裁剪"));
        return;
    }
//    for (TablePtr t : mHvacInfo.tables)
//    {
//        qDebug() << *t;
//    }
//    for (TablePtr t : mHvacInfo.tables)
//    {
//        QFile f(QStringLiteral("%1.csv").arg(t->getName()));
//        if (f.open(QIODevice::ReadWrite)) {
//            toCsv(t, &f);
//        }
//    }
    qDebug() << filepath << QStringLiteral("完成解析") << mHvacInfo;

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
    if (dt.isValid()) {
        return (dt);
    }
    dfmt = "M/d/yyyy HH:mm:ss";
    dt = QDateTime::fromString(str, dfmt);
    if (dt.isValid()) {
        return (dt);
    }
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
    mSetting.projectidField = obj["projectidField"].toString();
    mSetting.datetimefield = obj["datetimefield"].toString();
    setDatetimeField(mSetting.datetimefield);
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


/**
 * @brief 把数据转换为csv
 * @param table
 * @param io
 * @return
 */
bool GHvacDataFileIO::toCsv(GHvacDataFileIO::TablePtr table, QIODevice *io)
{
    if (!io->isOpen()) {
        return (false);
    }
    QTextStream txt(io);
    SACsvStream csv(&txt);

    csv << table->rowNames();
    csv.newLine();
    const int cols = table->columnCount();
    const int rows = table->rowCount();

    for (int i = 0; i < cols; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            csv<<(table->at(j, i));
        }
        csv.newLine();
    }
    return (true);
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


bool GHvacDataFileIO::groupByCanIP(QList<TablePtr> tables)
{
    QString canipfield = mSetting.canipfield;

    for (TablePtr t : tables)
    {
        if (t->getName() == mSetting.tablename_system) {
            mHvacInfo.tables.append(t);
        }else if (t->getName() == mSetting.tablename_module) {
            if (!t->haveFieldid(canipfield)) {
                //进行测试，对可能的canip字段进行测试,如果找到就继续，没有找到就退出
                bool isfix = false;
                for (const QString& cf : c_canipfield_stringlist)
                {
                    if (t->haveFieldid(cf)) {
                        isfix = true;
                        canipfield = cf;
                        mSetting.canipfield = cf;
                        break;
                    }
                }
                if (!isfix) {
                    emit message(QStringLiteral("文件异常:配置信息中%1字段未能在数据表%2中查询到").arg(canipfield).arg(t->getName()));
                    return (false);
                }
            }
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
            if (!t->haveFieldid(canipfield)) {
                bool isfix = false;
                for (const QString& cf : c_canipfield_stringlist)
                {
                    if (t->haveFieldid(cf)) {
                        isfix = true;
                        canipfield = cf;
                        break;
                    }
                }
                if (!isfix) {
                    emit message(QStringLiteral("文件异常:配置信息中%1字段未能在数据表%2中查询到").arg(canipfield).arg(t->getName()));
                    return (false);
                }
            }
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
    return (true);
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

    if (datetimefield.isEmpty()) {
        datetimefield = QStringLiteral("记录时间");
    }
    QList<TableType::Type> unionDatetime;

    do
    {
        QSet<TableType::Type> datetime;
        for (TablePtr t : mHvacInfo.tables)
        {
            if (!t->haveFieldid(datetimefield)) {
                //如果没有时间字段，就进行猜测
                for (const QString& df : c_datetimefild_stringlist)
                {
                    if (t->haveFieldid(df)) {
                        datetimefield = df;
                        setDatetimeField(df);//猜测对了，把时间字段赋值
                        mSetting.datetimefield = df;
                        break;
                    }
                }
            }
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
            mHvacInfo.allDateTimeScale.append(QDateTime::fromMSecsSinceEpoch(t));
        }
    }while(0);
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
        if (!t->haveFieldid(datetimefield)) {
            //如果没有时间字段，就进行猜测
            for (const QString& df : c_datetimefild_stringlist)
            {
                if (t->haveFieldid(df)) {
                    datetimefield = df;
                    setDatetimeField(df);//猜测对了，把时间字段赋值
                    mSetting.datetimefield = df;
                    break;
                }
            }
        }
        qDebug() << "orderBy:" << datetimefield << t->getName() << t->rowCount();
        int index = t->nameToIndex(datetimefield);
        if (index < 0) {
            message(QStringLiteral("%1表未找到时间字段%1").arg(t->getName()).arg(datetimefield));
            qDebug() << t->getName() << t->rowNames();
            continue;
        }
        t->orderBy(index);
    }
    emit message(QStringLiteral("完成所有表时间排序，耗时%1").arg(tic.elapsed()));

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


void GHvacDataFileIO::getProjectID(QList<TablePtr> tables)
{
    QString projectfield = mSetting.projectidField;

    if (projectfield.isEmpty()) {
        projectfield = QStringLiteral("Proj_ID");
    }
    for (TablePtr t : tables)
    {
        if (!t->haveFieldid(projectfield)) {
            //如果没有时间字段，就进行猜测
            for (const QString& pf : c_projectfild_stringlist)
            {
                if (t->haveFieldid(pf)) {
                    projectfield = pf;
                    mSetting.projectidField = pf;
                    break;
                }
            }
        }
        if (!t->haveFieldid(projectfield)) {
            emit message(QStringLiteral("未能找到工程id相关字段"));
        }
        //取工程id
        TableType::SeriesPtr r = t->row(projectfield);
        if (r && (r->size() > 0)) {
            mHvacInfo.projectid = r->at(0);
            emit hasGetProjectID(mHvacInfo.projectid);
            emit message(QStringLiteral("工程id：%1").arg(mHvacInfo.projectid));
            return;
        }
    }
}
