#ifndef GMONITORWIDGET_H
#define GMONITORWIDGET_H

#include <QtWidgets/QWidget>
#include <QTimer>
#include "GTemplate.h"
#include "GModuleValueView.h"
#include <QJsonObject>
#include <QDateTime>
class GAPIManager;
namespace Ui {
class GMonitorWidget;
}

class GMonitorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GMonitorWidget(QWidget *parent = nullptr);
    ~GMonitorWidget();
    void startMonitor(const QString& mac);
    bool startMonitor();
    void stopMonitor();
    bool isStart() const;

    //设置mac
    void setMac(const QString& mac);

    //获取刷新频率
    int interval() const;

    //设置刷新频率
    void setInterval(int ms);

    //设置监控的模板
    void setTemplate(GTemplate *t);

    //设置api
    void setupAPI(GAPIManager *api);

    //获取设置的mac
    QString mac() const;

    //获取监控持续时间
    int lastMinute() const;

protected:
    void changeEvent(QEvent *e);

private slots:
    void acceptMonitorData(const QJsonObject& obj);

private:
    void onTimeout();
    void valueRender(const QJsonObject& obj);

signals:

    /**
     * @brief message
     * @param msg
     */
    void message(const QString& msg);

private:
    Ui::GMonitorWidget *ui;
    QTimer mTimer;
    GTemplate *mCurrentTemplate;
    GModuleValueView *mModuleWidget;
    GAPIManager *mApi;
    QString m_mac;
    int m_interval;
    QJsonObject m_obj;
    QDateTime m_startDatetime;
};

#endif // GMONITORWIDGET_H
