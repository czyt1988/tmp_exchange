#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include <QtWidgets/QWidget>
#include "GTemplate.h"
#include "GModuleValueView.h"
#include "GHvacDataFileIO.h"
#include <QThread>
#include <QTimer>
#include "GHvacDataInfo.h"
namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void setTemplate(GTemplate *t);
    void loadTemplate();

    enum Speed {
        Speed1x		= 1
        , Speed2x	= 2
        , Speed3x	= 3
    };
    enum Mode {
        None
        , Stopped
        , Runing
    };
protected:
    void changeEvent(QEvent *e);

private slots:
    void on_pushButtonBrower_clicked();

    void on_pushButtonRun_clicked();

    void on_comboBoxTemplate_currentIndexChanged(int index);

    void onFileReaded(GHvacDataInfo info);

    void onIOErrRec(const QString& msg);

    void onTimeout();

    void updateValue(int toSecsSinceEpoch);
    void on_pushButtonSpeed_clicked();


    void on_horizontalSlider_valueChanged(int value);

    void valueRender(const QJsonObject& obj);

private:
    void deleteTemplates();
    void setRunMode();
    void setStopMode();
    void setSpeed(Speed s);

signals:

    /**
     * @brief 打开文件
     * @param filepath
     */
    void openFile(const QString& filepath);

    /**
     * @brief message
     * @param msg
     */
    void message(const QString& msg);

private:
    Ui::MainWidget *ui;
    QList<GTemplate *> mTemplate;
    GHvacDataInfo mHvacInfo;
    GTemplate *mCurrentTemplate;
    GModuleValueView *mModuleWidget;
    QTimer mTimer;
    Speed mCurrentSpeed;
    Mode mMode;
};

#endif // MAINWIDGET_H
