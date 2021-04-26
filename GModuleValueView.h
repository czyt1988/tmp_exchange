#ifndef GMODULEVALUEVIEW_H
#define GMODULEVALUEVIEW_H


#include <QtWidgets/QWidget>
#include "GNodeInfo.h"
#include "GItemDisplayWidget.h"
class QGridLayout;

namespace Ui {
class GModuleValueView;
}

class GModuleValueView : public QWidget
{
    Q_OBJECT

public:
    explicit GModuleValueView(QWidget *parent = 0);
    ~GModuleValueView();
    void append(const GNodeInfo& info);
    void setNodeInfos(const QList<GNodeInfo>& infos);
    void clear();
    void updateValue(const GNodeInfo& n);

protected:
    void changeEvent(QEvent *e);

private:
    QList<GNodeInfo> mNodeinfos;
    QHash<GNodeInfo, GItemDisplayWidget *> mNodeToWidget;
    Ui::GModuleValueView *ui;
    QGridLayout *mLayout;
};

#endif // GMODULEVALUEVIEW_H
