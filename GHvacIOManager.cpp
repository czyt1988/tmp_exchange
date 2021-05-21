#include "GHvacIOManager.h"
#include <QThread>
GHvacIOManager::GHvacIOManager(QObject *p) : QObject(p)
{
}


GHvacIOManager *GHvacIOManager::getInstance()
{
    static GHvacIOManager s_io;

    return (&s_io);
}

bool GHvacIOManager::isHaveData() const
{
    return (m_datainfo.tables.size() > 0);
}


/**
 * @brief 打开文件以zip为后缀的回放文件
 */
void GHvacIOManager::openReviewData(const QString& filepath)
{
    GHvacDataFileIO *io = new GHvacDataFileIO();
    //设置相关信息

    QThread *readThread = new QThread();

    io->moveToThread(readThread);
    connect(readThread, &QThread::finished, io, &GHvacDataFileIO::deleteLater);
    connect(readThread, &QThread::finished, io, &QThread::deleteLater);
    //业务逻辑
    connect(this, &GHvacIOManager::startOpenFile, io, &GHvacDataFileIO::open);
    connect(io, &GHvacDataFileIO::readed, this, &GHvacIOManager::onFileReaded);
    connect(io, &GHvacDataFileIO::message, this, &GHvacIOManager::message);
    connect(io, &GHvacDataFileIO::error, this, &GHvacIOManager::onIoError);
    //唤起线程自杀
    connect(io, &GHvacDataFileIO::readed, this, [readThread](GHvacDataInfo) {
        readThread->quit();
    });
    connect(io, &GHvacDataFileIO::error, this, [readThread](QString) {
        readThread->quit();
    });
    readThread->start();
    emit message(QStringLiteral("正打开:%1").arg(filepath));
    emit startOpenFile(filepath);
}


void GHvacIOManager::onFileReaded(GHvacDataInfo info)
{
    m_datainfo = info;
    emit fileReaded(info);
}


void GHvacIOManager::onIoError(const QString& msg)
{
    emit openFailed();
    emit message(msg);
}
