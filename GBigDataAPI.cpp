#include "GBigDataAPI.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonDocument>
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
            , this, [this, rep](QNetworkReply::NetworkError err) {					      \
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
            , this, [this, rep](QNetworkReply::NetworkError err) {					      \
            qDebug() << err;									      \
            rep->deleteLater();									      \
        });												      \
    }while(0)

GBigDataAPI::GBigDataAPI(QObject *p) : QObject(p)
{
    m_networkmgr = new QNetworkAccessManager(this);
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

    BD_REQUEST_ARRAY(url, projectFaults);
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
