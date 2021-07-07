#ifndef GTEMPLATEMANAGER_H
#define GTEMPLATEMANAGER_H
#include <QObject>
class GTemplate;
class GTemplateManager : public QObject
{
    Q_OBJECT
private:
    GTemplateManager(QObject *p = nullptr);
public:
    static GTemplateManager *getInstance();

    //回放模板
    int loadReviewTemplates(const QString& path);
    QList<GTemplate *> getAllReviewTemplates() const;
    GTemplate *getCurrentReviewTemplate() const;
    void setCurrentReviewTemplate(GTemplate *t);
    void setCurrentReviewTemplate(int index);
    int getReviewTemplateCount() const;

    //监控模板
    int loadMonitorTemplates(const QString& path);
    QList<GTemplate *> getAllMonitorTemplates() const;
    GTemplate *getCurrentMonitorTemplate() const;
    void setCurrentMonitorTemplate(GTemplate *t);
    void setCurrentMonitorTemplate(int index);
    int getMonitorTemplateCount() const;

signals:

    /**
     * @brief 模板发生改变信号
     * @param temp
     */
    void reviewTemplateChanged(GTemplate *temp);
    void monitorTemplateChanged(GTemplate *temp);

protected:
    void deleteReviewTemplates();
    void deleteMonitorTemplates();

private:
    QList<GTemplate *> mReviewTemplate;
    QList<GTemplate *> mMonitorTemplate;
    int mCurrentReviewIndex;
    int mCurrentMonitorIndex;
};
#ifndef TemplateManager
#define TemplateManager    GTemplateManager::getInstance()
#endif
#endif // GTEMPLATELOADER_H
