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
signals:

    /**
     * @brief 模板发生改变信号
     * @param temp
     */
    void reviewTemplateChanged(GTemplate *temp);

protected:
    void deleteTemplates();

private:
    QList<GTemplate *> mReviewTemplate;
    int mCurrentReviewIndex;
};
#ifndef TemplateManager
#define TemplateManager    GTemplateManager::getInstance()
#endif
#endif // GTEMPLATELOADER_H
