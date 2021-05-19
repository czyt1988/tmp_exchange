#include "GTemplateManager.h"
#include "GTemplate.h"
#include <QDebug>
#include <QDir>
GTemplateManager::GTemplateManager(QObject *p):QObject(p)
  ,mCurrentIndex(-1)
{
}

GTemplateManager *GTemplateManager::getInstance()
{
    static GTemplateManager s_templateMgr;
    return &s_templateMgr;
}

int GTemplateManager::loadTemplates(const QString &path)
{
    deleteTemplates();
    QDir dir(path);
    QStringList xmlfilelist = dir.entryList(QDir::NoDotAndDotDot|QDir::Files);

    for (const QString& p : xmlfilelist)
    {
        qDebug() << dir.path() + QDir::separator() + p;
        QScopedPointer<GTemplate> tp(new GTemplate(this));
        if (tp->load(dir.path() + QDir::separator() + p)) {
            mTemplate.append(tp.take());
        }
    }
    return mTemplate.size();
}

QList<GTemplate *> GTemplateManager::getAllTemplates() const
{
    return mTemplate;
}

GTemplate *GTemplateManager::getCurrentTemplate() const
{
    if(mCurrentIndex >= 0 && mCurrentIndex < mTemplate.size()){
        return mTemplate[mCurrentIndex];
    }
    return nullptr;
}

void GTemplateManager::setCurrentTemplate(GTemplate *t)
{
    int index = mTemplate.indexOf(t);
    if(index >= 0){
        mCurrentIndex = index;
        emit templateChanged(t);
    }
}

void GTemplateManager::setCurrentTemplate(int index)
{
    if(index >= 0 && index < mTemplate.size()){
        mCurrentIndex = index;
        emit templateChanged(getCurrentTemplate());
    }
}

int GTemplateManager::getTemplateCount() const
{
    return mTemplate.size();
}

/**
 * @brief 删除所有模板
 */
void GTemplateManager::deleteTemplates()
{
    for (auto i : mTemplate)
    {
        i->deleteLater();
    }
    mCurrentIndex = -1;
    mTemplate.clear();
}
