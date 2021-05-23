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
#include <functional>
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
        QActionGroup *actionGroupRunDataViewSpeed;
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
        GDataReviewWidget *dataReviewWidget;
        GPlotWidget *figureWidget;
        SARibbonCategory *categoryMain;
        SARibbonCategory *categoryDataView;
        SARibbonCategory *categoryFigure;
        SARibbonPannel *pannelMainFile;
        SARibbonPannel *pannelMainWindowList;
        SARibbonPannel *pannelMainDataTemplate;
        SARibbonGallery *galleryDataTemplate;
        SARibbonPannel *pannelDataViewOpetion;
        SARibbonPannel *pannelFigureOpetion;
        QList<QAction *> templateActionList;
        void retranslateUi(MainWindow *w);

//        void createAction(QAction* act
//                          ,const QString& objName
//                          ,const QString& text
//                          ,const QIcon& icon
//                          ,std::function<void(bool)>
//                          ,bool isCheackable
//                          ,bool isChecked
//                          ,SARibbonPannel* pannel
//                          ,SARibbonPannelItem::RowProportion rp = SARibbonPannelItem::Large
//                          );
    };
    QScopedPointer<UI> ui;
};

#endif // MAINWINDOW_H
