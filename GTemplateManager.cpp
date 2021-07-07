#include "GTemplateManager.h"
#include "GTemplate.h"
#include <QDebug>
#include <QDir>
GTemplateManager::GTemplateManager(QObject *p) : QObject(p)
    , mCurrentReviewIndex(-1)
    , mCurrentMonitorIndex(-1)
{
}


GTemplateManager *GTemplateManager::getInstance()
{
    static GTemplateManager s_templateMgr;

    return (&s_templateMgr);
}


int GTemplateManager::loadReviewTemplates(const QString& path)
{
    deleteReviewTemplates();
    QDir dir(path);
    QStringList xmlfilelist = dir.entryList(QDir::NoDotAndDotDot|QDir::Files);

    for (const QString& p : xmlfilelist)
    {
        qDebug() << dir.path() + QDir::separator() + p;
        QScopedPointer<GTemplate> tp(new GTemplate(this));
        if (tp->load(dir.path() + QDir::separator() + p)) {
            mReviewTemplate.append(tp.take());
        }
    }
    return (mReviewTemplate.size());
}


QList<GTemplate *> GTemplateManager::getAllReviewTemplates() const
{
    return (mReviewTemplate);
}


GTemplate *GTemplateManager::getCurrentReviewTemplate() const
{
    if ((mCurrentReviewIndex >= 0) && (mCurrentReviewIndex < mReviewTemplate.size())) {
        return (mReviewTemplate[mCurrentReviewIndex]);
    }
    return (nullptr);
}


void GTemplateManager::setCurrentReviewTemplate(GTemplate *t)
{
    int index = mReviewTemplate.indexOf(t);

    if (index >= 0) {
        mCurrentReviewIndex = index;
        emit reviewTemplateChanged(t);
    }
}


void GTemplateManager::setCurrentReviewTemplate(int index)
{
    if ((index >= 0) && (index < mReviewTemplate.size())) {
        mCurrentReviewIndex = index;
        emit reviewTemplateChanged(getCurrentReviewTemplate());
    }
}


int GTemplateManager::getReviewTemplateCount() const
{
    return (mReviewTemplate.size());
}


int GTemplateManager::loadMonitorTemplates(const QString& path)
{
    deleteMonitorTemplates();
    QDir dir(path);
    QStringList xmlfilelist = dir.entryList(QDir::NoDotAndDotDot|QDir::Files);

    for (const QString& p : xmlfilelist)
    {
        qDebug() << dir.path() + QDir::separator() + p;
        QScopedPointer<GTemplate> tp(new GTemplate(this));
        if (tp->load(dir.path() + QDir::separator() + p)) {
            mMonitorTemplate.append(tp.take());
        }
    }
    return (mMonitorTemplate.size());
}


QList<GTemplate *> GTemplateManager::getAllMonitorTemplates() const
{
    return (mMonitorTemplate);
}


GTemplate *GTemplateManager::getCurrentMonitorTemplate() const
{
    if ((mCurrentMonitorIndex >= 0) && (mCurrentMonitorIndex < mMonitorTemplate.size())) {
        return (mMonitorTemplate[mCurrentMonitorIndex]);
    }
    return (nullptr);
}


void GTemplateManager::setCurrentMonitorTemplate(GTemplate *t)
{
    int index = mMonitorTemplate.indexOf(t);

    if (index >= 0) {
        mCurrentMonitorIndex = index;
        emit monitorTemplateChanged(t);
    }
}


void GTemplateManager::setCurrentMonitorTemplate(int index)
{
    if ((index >= 0) && (index < mMonitorTemplate.size())) {
        mCurrentMonitorIndex = index;
        emit monitorTemplateChanged(getCurrentMonitorTemplate());
    }
}


int GTemplateManager::getMonitorTemplateCount() const
{
    return (mMonitorTemplate.size());
}


/**
 * @brief 删除所有模板
 */
void GTemplateManager::deleteReviewTemplates()
{
    for (auto i : mReviewTemplate)
    {
        i->deleteLater();
    }
    mCurrentReviewIndex = -1;
    mReviewTemplate.clear();
}


void GTemplateManager::deleteMonitorTemplates()
{
    for (auto i : mMonitorTemplate)
    {
        i->deleteLater();
    }
    mCurrentMonitorIndex = -1;
    mMonitorTemplate.clear();
}
