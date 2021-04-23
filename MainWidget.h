#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include <QtWidgets/QWidget>
#include "GTemplate.h"
#include "GModuleValueView.h"
#include "GHvacDataFileIO.h"
#include <QThread>

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

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_pushButtonBrower_clicked();

    void on_pushButtonRun_clicked();

    void on_comboBoxTemplate_currentIndexChanged(int index);

    void onFileReaded(QList<GHvacDataFileIO::TablePtr> tables);

    void onIOErrRec(const QString& msg);

private:
    void deleteTemplates();

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
    GTemplate *mCurrentTemplate;
    GModuleValueView *mModuleWidget;
};

#endif // MAINWIDGET_H
