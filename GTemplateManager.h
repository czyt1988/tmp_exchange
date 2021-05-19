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
    int loadTemplates(const QString& path);
    QList<GTemplate *> getAllTemplates() const;
    GTemplate *getCurrentTemplate() const;
    void setCurrentTemplate(GTemplate *t);
    void setCurrentTemplate(int index);
    int getTemplateCount() const;

signals:

    /**
     * @brief 模板发生改变信号
     * @param temp
     */
    void templateChanged(GTemplate *temp);

protected:
    void deleteTemplates();

private:
    QList<GTemplate *> mTemplate;
    int mCurrentIndex;
};
#ifndef TemplateManager
#define TemplateManager    GTemplateManager::getInstance()
#endif
#endif // GTEMPLATELOADER_H
