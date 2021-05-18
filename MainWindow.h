#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "GHvacDataInfo.h"
#include "GTemplate.h"
#include "GPlotWidget.h"
#include "SARibbonMainWindow.h"
#include "MainWidget.h"
#include <GFaultWidget.h>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QScopedPointer>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPannel.h"

class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void onMessage(const QString& msg);
    void onFileReaded(GHvacDataInfo data);
    void onTemplateChanged(GTemplate *temp);
    void onFaultWidgetIndexReques(int index);

    void onActionOpenTriggered();
    void onActionDataViewWindowTriggered();
    void onActionFigureWindowTriggered();
    void onOpenFile(const QString& filename);
    void onOpenFailed();

private:
    void init();

private:
    class UI {
public:
        QAction *actionOpen;
        QAction *actionDataViewWindow;
        QAction *actionFigureWindow;
        QWidget *centralwidget;
        QVBoxLayout *verticalLayout_3;
        QTabWidget *tabWidget;
        QStatusBar *statusbar;
        QDockWidget *dockWidgetMessage;
        QWidget *dockWidgetContents;
        QVBoxLayout *verticalLayout;
        QTextEdit *textEdit;
        QDockWidget *dockWidgetFault;
        QWidget *dockWidgetContents_2;
        QVBoxLayout *verticalLayout_2;
        GFaultWidget *widgetFaule;
        MainWidget *dataReviewWidget;
        GPlotWidget *dataPlotWidget;
        SARibbonCategory *categoryMain;
        SARibbonCategory *categoryDataView;
        SARibbonCategory *categoryFigure;
        SARibbonPannel *main_filePannel;
        SARibbonPannel *main_WindowPannel;
        void retranslateUi(MainWindow *w);
    };
    QScopedPointer<UI> ui;
};

#endif // MAINWINDOW_H
