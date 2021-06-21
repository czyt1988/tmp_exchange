#ifndef GPROJECTARCHIVESWIDGET_H
#define GPROJECTARCHIVESWIDGET_H
#include "GBigDataAPI.h"
#include <QStandardItemModel>
#include <QtWidgets/QWidget>
#include "GArchivesTableModel.h"

namespace Ui {
class GProjectArchivesWidget;
}

class GProjectArchivesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GProjectArchivesWidget(QWidget *parent = 0);
    ~GProjectArchivesWidget();
public slots:
    void setProjectID(int proid);

protected:
    void changeEvent(QEvent *e);

private slots:
    void onRecProjectInfo(QJsonObject res);
    void onRecProjectFaults(QJsonArray res);
    void onRecProjectMacChangeHistory(QJsonObject res);
    void on_pushButtonSearch_clicked();

    void on_pushButtonExport_clicked();

private:
    void resetModel();
    QList<QStandardItem *> createRow(const QString& key, const QString& value);
    void stepProgress();

signals:
    void message(const QString& info);

private:
    Ui::GProjectArchivesWidget *ui;
    GBigDataAPI *m_api;
    QStandardItemModel *m_treeModel;
    GArchivesTableModel *m_tableModel;
    QString m_searchArg; //有可能是工程id，有可能是条码
};

#endif // GPROJECTARCHIVESWIDGET_H
