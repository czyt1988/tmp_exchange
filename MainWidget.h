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

    enum Speed {
        Speed1x		= 1
        , Speed2x	= 2
        , Speed3x	= 3
        , SpeedMax	= 4
    };
    enum Mode {
        NoneMode
        , StoppedMode
        , RuningMode
    };

    GTemplate *getCurrentTemplate() const;
    //获取当前模式
    Mode getMode() const;
    void setRunMode();
    void setStopMode();
    void setSpeed(Speed s);
public slots:
    // 打开文件
    void open();

    /**
     * @brief 跳转到对应的索引(时间)
     * @param i
     */
    void toIndex(int i);
protected:
    void changeEvent(QEvent *e);

private slots:

    void onFileReaded(GHvacDataInfo info);

    void onIOErrRec(const QString& msg);

    void onTimeout();

    void updateValue(int msecsSinceEpoch);
    void on_pushButtonSpeed_clicked();


    void on_horizontalSlider_valueChanged(int value);

    void valueRender(const QJsonObject& obj);
    void onIoError(const QString& msg);

    void on_comboBoxCanIP_currentIndexChanged(const QString& arg1);

private:

    void setNoneMode();

    void updateValueBySliderValue(int value);

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

    /**
     * @brief 文件读取完成发射的信号
     * @param info
     */
    void fileReaded(GHvacDataInfo info);

    /**
     * @brief 打开失败触发信号
     */
    void openFailed();

private:
    Ui::MainWidget *ui;
    GHvacDataInfo mHvacInfo;
    GModuleValueView *mModuleWidget;
    QTimer mTimer;
    Speed mCurrentSpeed;
    Mode mMode;
    QIcon mSpeed1;
    QIcon mSpeed2;
    QIcon mSpeed3;
    GTemplate* mCurrentTemplate;
};

#endif // MAINWIDGET_H
