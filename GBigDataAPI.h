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
public slots:
    //ping请求
    void requestPing();

    //请求工程信息
    void requestProjectInfo(int projectid);

    //请求工程故障上报数据
    void requestProjectFaults(int projectid);

    //请求工程换版信息
    void requesMacChangeHistory(int projectid);

    //请求生产信息 - hive查询，速度较慢
    void requesProduceInfo(int projectid);

    //请求安装信息 - hive查询，速度较慢
    void requesInstallInfo(int projectid);

    //获取所有监控工程
    void requesListAllMonitorInfos();

    //请求监控数据
    void requestMonitorData(const QString& mac);

signals:

    /**
     * @brief 收到ping回复
     * @param res 返回json，形如：
     *
     * @code
     * {
     *  "statue": true,
     *  "data": {
     *   "servetime": "2021-06-29T09:41:25.302"
     *  }
     * }
     * @endcode
     */
    void ping(QJsonObject res);

    /**
     * @brief 对应getProjectInfo
     * @param res
     */
    void projectInfos(QJsonObject res);

    /**
     * @brief 返回工程错误信息
     * @param res
     */
    void projectFaults(QJsonObject res);

    /**
     * @brief 工程换版记录
     * @param res
     */
    void projectMacChangeHistory(QJsonObject res);

    /**
     * @brief 获取生产信息
     * @param res
     */
    void produceInfo(QJsonObject res);

    /**
     * @brief 获取安装信息
     * @param res
     */
    void installInfo(QJsonObject res);

    /**
     * @brief 例举所有监控工程
     * @param res
     */
    void listAllMonitorInfos(QJsonObject res);

    /**
     * @brief 监控数据
     * @param res
     */
    void monitorData(QJsonObject res);

private:
    QNetworkAccessManager *m_networkmgr;
};



/**
 * @brief api管理单例
 */
class GAPIManager : public QObject {
    Q_OBJECT
    GAPIManager();
public:
    static GAPIManager& getInstance();
    static GBigDataAPI *bigdataAPI();
    void dstory();//为了正常关闭socket

public slots:
    //ping请求
    void requestPing();

    //请求工程信息
    void requestProjectInfo(int projectid);

    //请求工程故障上报数据
    void requestProjectFaults(int projectid);

    //请求工程换版信息
    void requesMacChangeHistory(int projectid);

    //请求生产信息 - hive查询，速度较慢
    void requesProduceInfo(int projectid);

    //请求安装信息 - hive查询，速度较慢
    void requesInstallInfo(int projectid);

    //获取所有监控工程
    void requesListAllMonitorInfos();

    //请求监控数据
    void requestMonitorData(const QString& mac);

signals:
    //一下信号为多线程请求时发出,无须关注
    //ping请求
    void apiRequestPing();

    //请求工程信息
    void apiRequestProjectInfo(int projectid);

    //请求工程故障上报数据
    void apiRequestProjectFaults(int projectid);

    //请求工程换版信息
    void apiRequesMacChangeHistory(int projectid);

    //请求生产信息 - hive查询，速度较慢
    void apiRequesProduceInfo(int projectid);

    //请求安装信息 - hive查询，速度较慢
    void apiRequesInstallInfo(int projectid);

    //获取所有监控工程
    void apiRequesListAllMonitorInfos();

    //请求监控数据
    void apiRequestMonitorData(const QString& mac);

signals:

    // 收到ping回复
    void ping(QJsonObject res);

    //对应getProjectInfo
    void projectInfos(QJsonObject res);

    //返回工程错误信息
    void projectFaults(QJsonObject res);

    //工程换版记录
    void projectMacChangeHistory(QJsonObject res);

    //获取生产信息
    void produceInfo(QJsonObject res);

    //获取安装信息
    void installInfo(QJsonObject res);

    //例举所有监控工程
    void listAllMonitorInfos(QJsonObject res);

    //监控数据
    void monitorData(QJsonObject res);

protected:
    void createApi();

private:
    GBigDataAPI *m_api;
    QThread *m_bigdataApiThread;
};

#ifndef GLOBAL_API_MGR
#define GLOBAL_API_MGR		GAPIManager::getInstance()
#endif
#ifndef GLOBAL_API_MGR_PTR
#define GLOBAL_API_MGR_PTR	&(GAPIManager::getInstance())
#endif

#endif // GBIGDATAAPI_H
