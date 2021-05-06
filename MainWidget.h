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
        NoneMode
        , StoppedMode
        , RuningMode
    };

    GTemplate *getCurrentTemplate() const;

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
    void onIoError(const QString& msg);

private:
    void deleteTemplates();
    void setRunMode();
    void setStopMode();
    void setNoneMode();
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

    /**
     * @brief 文件读取完成发射的信号
     * @param info
     */
    void fileReaded(GHvacDataInfo info);

    /**
     * @brief 模板发生改变信号
     * @param temp
     */
    void templateChanged(GTemplate *temp);

private:
    Ui::MainWidget *ui;
    QList<GTemplate *> mTemplate;
    GHvacDataInfo mHvacInfo;
    GTemplate *mCurrentTemplate;
    GModuleValueView *mModuleWidget;
    QTimer mTimer;
    Speed mCurrentSpeed;
    Mode mMode;
    QIcon mSpeed1;
    QIcon mSpeed2;
    QIcon mSpeed3;
};

#endif // MAINWIDGET_H
