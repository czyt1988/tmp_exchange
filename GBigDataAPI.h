#ifndef GBIGDATAAPI_H
#define GBIGDATAAPI_H
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
class QNetworkAccessManager;

/**
 * @brief 封装了大数据接口相关的操作
 */
class GBigDataAPI : public QObject
{
    Q_OBJECT
public:
    GBigDataAPI(QObject *p = nullptr);
    enum ApiType {
        API_ProjectInfo
    };

public slots:
    //请求工程信息
    void requestProjectInfo(int projectid);

    //请求工程故障上报数据
    void requestProjectFaults(int projectid);

    //请求工程换版信息
    void requesMacChangeHistory(int projectid);

signals:

    /**
     * @brief 对应getProjectInfo
     * @param res
     */
    void projectInfos(QJsonObject res);

    /**
     * @brief 返回工程错误信息
     * @param res
     */
    void projectFaults(QJsonArray res);

    /**
     * @brief 工程换版记录
     * @param res
     */
    void projectMacChangeHistory(QJsonObject res);

private:
    QNetworkAccessManager *m_networkmgr;
};

#endif // GBIGDATAAPI_H
