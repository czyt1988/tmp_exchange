#ifndef GDATAREVIEWWIDGET_H
#define GDATAREVIEWWIDGET_H
#include <QtWidgets/QWidget>
#include "GTemplate.h"
#include "GModuleValueView.h"
#include "GHvacDataFileIO.h"
#include <QThread>
#include <QTimer>
#include "GHvacDataInfo.h"

namespace Ui {
class GDataReviewWidget;
}

/**
 * @brief 空调数据回放窗口/监控窗口
 */
class GDataReviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GDataReviewWidget(QWidget *parent = 0);
    ~GDataReviewWidget();
    void setTemplate(GTemplate *t);

    /**
     * @brief 回放速度
     */
    enum Speed {
        Speed1x		= 1
        , Speed2x	= 2
        , Speed3x	= 3
        , SpeedMax	= 4
    };

    /**
     * @brief 当前的模式
     */
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
    // 开始打开文件，用于停止当前的timer 在review模式下使用
    void startOpenReviewData();

    //设置数据 在review模式下使用
    void setData(GHvacDataInfo datainfo);

    /**
     * @brief 跳转到对应的索引(时间) 在review模式下使用
     * @param i
     */
    void toIndex(int i);

protected:
    void changeEvent(QEvent *e);

private slots:
    void onTimeout();
    void updateValue(qint64 msecsSinceEpoch);
    void valueRender(const QJsonObject& obj);
    void on_horizontalSlider_valueChanged(int value);
    void on_comboBoxCanIP_currentIndexChanged(const QString& arg1);

private:

    void setNoneMode();

    void updateValueBySliderValue(int value);

signals:

    /**
     * @brief message
     * @param msg
     */
    void message(const QString& msg);

private:
    Ui::GDataReviewWidget *ui;
    GHvacDataInfo mHvacInfo;
    GModuleValueView *mModuleWidget;
    QTimer mTimer;
    Speed mCurrentSpeed;
    Mode mMode;
    GTemplate *mCurrentTemplate;
};

#endif // MAINWIDGET_H
