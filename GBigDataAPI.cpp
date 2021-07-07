#include "GBigDataAPI.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonDocument>
#include <QElapsedTimer>
#include <QThread>
#define START_DEBUG    1

#define BD_REQUEST(url, signalFunName)										      \
    do{													      \
        QNetworkRequest request(url);									      \
        QNetworkReply *rep = m_networkmgr->get(request);						      \
        connect(rep, &QNetworkReply::finished								      \
            , this, [this, rep]() {									      \
            QByteArray bin = rep->readAll();							      \
            QJsonParseError err;									      \
            QJsonDocument doc = QJsonDocument::fromJson(bin, &err);					      \
            if (doc.isNull()) {									      \
                qDebug() << QStringLiteral("发生错误:") << err.errorString() << " offset:" << err.offset; \
                qDebug() << "data->" << QString(bin);						      \
                rep->deleteLater();								      \
                return;										      \
            }											      \
            emit signalFunName(doc.object());							      \
            rep->deleteLater();									      \
        });												      \
        connect(rep, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error) \
            , this, [rep](QNetworkReply::NetworkError err) {						      \
            qDebug() << err;									      \
            rep->deleteLater();									      \
        });												      \
    }while(0)

#define BD_REQUEST_ARRAY(url, signalFunName)									      \
    do{													      \
        QNetworkRequest request(url);									      \
        QNetworkReply *rep = m_networkmgr->get(request);						      \
        connect(rep, &QNetworkReply::finished								      \
            , this, [this, rep]() {									      \
            QByteArray bin = rep->readAll();							      \
            QJsonParseError err;									      \
            QJsonDocument doc = QJsonDocument::fromJson(bin, &err);					      \
            if (doc.isNull()) {									      \
                qDebug() << QStringLiteral("发生错误:") << err.errorString() << " offset:" << err.offset; \
                qDebug() << "data->" << QString(bin);						      \
                rep->deleteLater();								      \
                return;										      \
            }											      \
            emit signalFunName(doc.array());							      \
            rep->deleteLater();									      \
        });												      \
        connect(rep, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error) \
            , this, [rep](QNetworkReply::NetworkError err) {						      \
            qDebug() << err;									      \
            rep->deleteLater();									      \
        });												      \
    }while(0)

GBigDataAPI::GBigDataAPI(QObject *p) : QObject(p)
{
    m_networkmgr = new QNetworkAccessManager(this);
}


/**
 * @brief 请求ping
 */
void GBigDataAPI::requestPing()
{
    QString url = QStringLiteral("http://10.2.5.235:8000/serve/ping/");

    BD_REQUEST(url, ping);
}


/**
 * @brief 请求工程信息
 * @param projectid
 */
void GBigDataAPI::requestProjectInfo(int projectid)
{
    QString url = QStringLiteral("http://10.2.5.235:8000/api/v1.0/hvac/project-live-detail/?proid=%1&showtype=json-cn").arg(projectid);

    BD_REQUEST(url, projectInfos);
}


/**
 * @brief 请求工程故障上报数据
 * @param projectid
 */
void GBigDataAPI::requestProjectFaults(int projectid)
{
    QString url = QStringLiteral("http://10.2.5.235:8000/api/v1.0/hvac/project-fault/?proid=%1").arg(projectid);

    BD_REQUEST(url, projectFaults);
}


/**
 * @brief 工程换版记录
 * @param projectid
 */
void GBigDataAPI::requesMacChangeHistory(int projectid)
{
    QString url = QStringLiteral("http://10.2.5.235:8000/api/v1.0/hvac/project-mac-change-history/?proid=%1").arg(projectid);

    BD_REQUEST(url, projectMacChangeHistory);
}


void GBigDataAPI::requesProduceInfo(int projectid)
{
    QString url = QStringLiteral("http://10.2.5.235:8000/api/v1.0/hvac/produce-info/?proid=%1").arg(projectid);

    BD_REQUEST(url, produceInfo);
}


void GBigDataAPI::requesInstallInfo(int projectid)
{
    QString url = QStringLiteral("http://10.2.5.235:8000/api/v1.0/hvac/install-info/?proid=%1").arg(projectid);

    BD_REQUEST(url, installInfo);
}


/**
 * @brief 获取所有监控工程
 */
void GBigDataAPI::requesListAllMonitorInfos()
{
    QString url = QStringLiteral("http://10.2.5.235:8000/api/v1.0/hvac/all-monitor-infos/?showdetails=1");

    BD_REQUEST(url, listAllMonitorInfos);
}


/**
 * @brief 请求监控数据
 * @param mac
 */
void GBigDataAPI::requestMonitorData(const QString& mac)
{
    QString url = QStringLiteral("http://10.2.5.235:8000/api/v1.0/hvac/gmv-monitor/?mac=%1").arg(mac);

    BD_REQUEST(url, monitorData);
}


GAPIManager::GAPIManager()
{
    createApi();
}


GAPIManager& GAPIManager::getInstance()
{
    static GAPIManager s_apimgr;

    return (s_apimgr);
}


GBigDataAPI *GAPIManager::bigdataAPI()
{
    return (getInstance().m_api);
}


void GAPIManager::dstory()
{
    m_bigdataApiThread->quit();
    m_bigdataApiThread->wait();
}


void GAPIManager::requestPing()
{
    emit apiRequestPing();
}


void GAPIManager::requestProjectInfo(int projectid)
{
    emit apiRequestProjectInfo(projectid);
}


void GAPIManager::requestProjectFaults(int projectid)
{
    emit apiRequestProjectFaults(projectid);
}


void GAPIManager::requesMacChangeHistory(int projectid)
{
    emit apiRequesMacChangeHistory(projectid);
}


void GAPIManager::requesProduceInfo(int projectid)
{
    emit apiRequesProduceInfo(projectid);
}


void GAPIManager::requesInstallInfo(int projectid)
{
    emit apiRequesInstallInfo(projectid);
}


void GAPIManager::requesListAllMonitorInfos()
{
    emit apiRequesListAllMonitorInfos();
}


void GAPIManager::requestMonitorData(const QString& mac)
{
    emit apiRequestMonitorData(mac);
}


void GAPIManager::createApi()
{
    m_api = new GBigDataAPI();
    m_bigdataApiThread = new QThread();
    m_api->moveToThread(m_bigdataApiThread);
    connect(m_bigdataApiThread, &QThread::finished, m_api, &GBigDataAPI::deleteLater);
    connect(m_bigdataApiThread, &QThread::finished, m_api, &QThread::deleteLater);
    //请求
    //ping
    connect(this, &GAPIManager::apiRequestPing, m_api, &GBigDataAPI::requestPing);
    //请求工程信息
    connect(this, &GAPIManager::apiRequestProjectInfo, m_api, &GBigDataAPI::requestProjectInfo);
    //请求工程故障上报数据
    connect(this, &GAPIManager::apiRequestProjectFaults, m_api, &GBigDataAPI::requestProjectFaults);
    //请求工程换版信息
    connect(this, &GAPIManager::apiRequesMacChangeHistory, m_api, &GBigDataAPI::requesMacChangeHistory);
    //请求生产信息 - hive查询，速度较慢
    connect(this, &GAPIManager::apiRequesProduceInfo, m_api, &GBigDataAPI::requesProduceInfo);
    //请求安装信息 - hive查询，速度较慢
    connect(this, &GAPIManager::apiRequesInstallInfo, m_api, &GBigDataAPI::requesInstallInfo);
    //获取所有监控工程
    connect(this, &GAPIManager::apiRequesListAllMonitorInfos, m_api, &GBigDataAPI::requesListAllMonitorInfos);
    //请求监控数据
    connect(this, &GAPIManager::apiRequestMonitorData, m_api, &GBigDataAPI::requestMonitorData);

    //触发返回
    //ping
    connect(m_api, &GBigDataAPI::ping, this, &GAPIManager::ping);
    //对应getProjectInfo
    connect(m_api, &GBigDataAPI::projectInfos, this, &GAPIManager::projectInfos);
    //返回工程错误信息
    connect(m_api, &GBigDataAPI::projectFaults, this, &GAPIManager::projectFaults);
    //工程换版记录
    connect(m_api, &GBigDataAPI::projectMacChangeHistory, this, &GAPIManager::projectMacChangeHistory);
    //获取生产信息
    connect(m_api, &GBigDataAPI::produceInfo, this, &GAPIManager::produceInfo);
    //获取安装信息
    connect(m_api, &GBigDataAPI::installInfo, this, &GAPIManager::installInfo);
    //例举所有监控工程
    connect(m_api, &GBigDataAPI::listAllMonitorInfos, this, &GAPIManager::listAllMonitorInfos);
    //监控数据
    connect(m_api, &GBigDataAPI::monitorData, this, &GAPIManager::monitorData);
    m_bigdataApiThread->start();
    //先进行ping操作，初始化
    requestPing();
}
