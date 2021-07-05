#ifndef GLISTALLMONITORDIALOG_H
#define GLISTALLMONITORDIALOG_H


#include <QtWidgets/QDialog>
#include <QStandardItemModel>
#include "GBigDataAPI.h"
#include <QPointer>
namespace Ui {
class GListAllMonitorDialog;
}

/**
 * @brief 列举所有监控信息
 */
class GListAllMonitorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GListAllMonitorDialog(QWidget *parent = nullptr);
    ~GListAllMonitorDialog();
    void setupAPI(GBigDataAPI *api);
    void updateInfo();

    //获取选择的mac
    QString getSelectMac() const;

    //获取选择的行
    int getSelectRow() const;

    //获取选择的工程id
    int getSelectProjectID() const;

protected:
    void changeEvent(QEvent *e);
    void clearTable();

private slots:
    void on_toolButtonUpdate_clicked();
    void onRecAllMonitorInfos(QJsonObject res);

private:
    Ui::GListAllMonitorDialog *ui;
    QPointer<GBigDataAPI> m_api;
    QStandardItemModel *m_tableModel;
};

#endif // GLISTALLMONITORDIALOG_H
