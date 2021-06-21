#ifndef GHVACIOMANAGER_H
#define GHVACIOMANAGER_H
#include <QObject>
#include "GHvacDataFileIO.h"
#include "GHvacDataInfo.h"

/**
 * @brief 负责所有io事务
 */
class GHvacIOManager : public QObject
{
    Q_OBJECT
private:
    GHvacIOManager(QObject *p = nullptr);
public:
    static GHvacIOManager *getInstance();
    //判断当前是否有读取数据
    bool isHaveData() const;
public slots:
    // 打开文件以zip为后缀的回放文件
    void openReviewData(const QString& filepath);

signals:

    /**
     * @brief 打开文件
     * @param filepath
     */
    void startOpenFile(const QString& filepath);

    /**
     * @brief message
     * @param msg
     */
    void message(const QString& msg);

    /**
     * @brief 文件读取完成发射的信号
     * @param info
     */
    void fileReaded(GHvacDataInfo info);

    /**
     * @brief 打开失败触发信号
     */
    void openFailed();

    /**
     * @brief 获取到工程id
     */
    void hasGetProjectID(int projectid);
private slots:

    void onFileReaded(GHvacDataInfo info);
    void onIoError(const QString& msg);

private:
    GHvacDataInfo m_datainfo;
};

#ifndef IOManager
#define IOManager    GHvacIOManager::getInstance()
#endif

#endif // GHVACIOMANAGER_H
