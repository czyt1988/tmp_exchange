#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "GHvacDataInfo.h"
#include "GTemplate.h"
#include "GPlotWidget.h"
class MainWidget;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void onMessage(const QString& msg);
    void onFileReaded(GHvacDataInfo data);
    void onTemplateChanged(GTemplate *temp);
    void onFaultWidgetIndexReques(int index);

    void on_actionPlotWindow_triggered();

private:
    Ui::MainWindow *ui;
    MainWidget *mMainWidget;
    GPlotWidget *mPlotWindow;
};

#endif // MAINWINDOW_H
