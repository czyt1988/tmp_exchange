#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "GHvacDataInfo.h"
#include "GTemplate.h"
#include "GPlotWidget.h"
#include "SARibbonMainWindow.h"
#include "GDataReviewWidget.h"
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
#include <QtWidgets/QActionGroup>
#include <QScopedPointer>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPannel.h"
#include "SARibbonGallery.h"
#include "SARibbonLineWidgetContainer.h"
#include "SARibbonLineEdit.h"
#include "GProjectArchivesWidget.h"
#include <functional>
#include "SACustomPlotTreeModel.h"
#include "GMonitorWidget.h"
#include <QTreeView>
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
    void onReviewTemplateChanged(GTemplate *temp);
    void onMonitorTemplateChanged(GTemplate *temp);
    void onFaultWidgetIndexReques(int index);

    void onActionOpenTriggered();
    void onActionArchivesWindowTriggered();
    void onActionDataViewWindowTriggered();
    void onActionFigureWindowTriggered();
    void onActionMessageViewWindowTriggered(bool c);
    void onActionFaultViewWindowTriggered(bool c);
    void onOpenFile(const QString& filename);
    void onOpenFailed();
    void onActionRunOrStopDataViewTriggered();
    void onActionGroupRunDataViewSpeedTriggered(QAction *action);
    void onGalleryTemplateActionTriggered(QAction *action);
    void onTabWidgetCurrentChanged(int index);
    void onActionFigureLegendTriggered(bool c);
    void onActionFigureWheelZoomableTriggered(bool c);
    void onActionFigureRectSelectZoomTriggered(bool c);
    void onActionFigureRangeDragTriggered(bool c);
    void onActionFigureAxesSelectTriggered(bool c);
    void onActionFigureLegendSelectTriggered(bool c);
    void onActionFigureInstallYTracerTriggered(bool c);
    void onActionAllMonitorListTriggered();
    void onGalleryMonitorTemplateActionTriggered(QAction *action);
    void onActionStartStopMonitorTriggered(bool c);
    void onUpdateMonitorLastTime(const QString& mac);

private:
    void init();

private:
    class UI {
public:
        QAction *actionOpen;
        QAction *actionDataViewWindow;
        QAction *actionMessageViewWindow;
        QAction *actionFaultViewWindow;
        QAction *actionFigureWindow;
        QAction *actionArchivesWindow;
        QAction *actionRunOrStopDataView;
        QAction *actionRunDataViewSpeed1;
        QAction *actionRunDataViewSpeed2;
        QAction *actionRunDataViewSpeed3;
        QAction *actionRunDataViewSpeedMax;
        QAction *actionFigureLegend;
        QAction *actionFigureWheelZoomable;
        QAction *actionFigureRectSelectZoom;
        QAction *actionFigureRangeDrag;
        QAction *actionFigureAxesSelect;
        QAction *actionFigureLegendSelect;
        QAction *actionFigureInstallYTracer;
        QAction *actionAllMonitorList;          ///< 列举所有监控信息
        QAction *actionStartStopMonitor;        ///< 监控控制
        QActionGroup *actionGroupRunDataViewSpeed;
        QWidget *centralwidget;
        QVBoxLayout *verticalLayout_3;
        QTabWidget *tabWidget;
        QStatusBar *statusbar;
        QDockWidget *dockWidgetMessage;
        QDockWidget *dockWidgetPlotSet;
        QTreeView *treeviewPlotSet;
        QWidget *dockWidgetContents;
        QVBoxLayout *verticalLayout;
        QTextEdit *textEdit;
        QDockWidget *dockWidgetFault;
        QWidget *dockWidgetContents_2;
        QVBoxLayout *verticalLayout_2;
        GProjectArchivesWidget *projectArchivesWidget;
        GFaultWidget *widgetFaule;
        GDataReviewWidget *dataReviewWidget;
        GPlotWidget *figureWidget;
        GMonitorWidget *monitorWidget;
        SARibbonCategory *categoryMain;
        SARibbonCategory *categoryDataView;
        SARibbonCategory *categoryFigure;
        SARibbonCategory *categoryMonitor;
        SARibbonPannel *pannelMainFile;
        SARibbonPannel *pannelMainWindowList;
        SARibbonPannel *pannelMainDataTemplate;
        SARibbonGallery *galleryDataTemplate;
        SARibbonPannel *pannelDataViewOpetion;
        SARibbonPannel *pannelFigureOpetion;
        SARibbonPannel *pannelMonitor;
        SARibbonPannel *pannelMonitorTemplate;
        SARibbonLineWidgetContainer *monitorMacWidgetContainer;
        SARibbonLineEdit *monitorMacLineEdit;
        QLabel *monitorLastTime;
        SARibbonGallery *galleryMonitorTemplate;
        QList<QAction *> templateActionList;
        QList<QAction *> templateMonitorActionList;
        SACustomPlotTreeModel *plotTreeModel;
        void retranslateUi(MainWindow *w);
    };
    QScopedPointer<UI> ui;
};

#endif // MAINWINDOW_H
