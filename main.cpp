#include "GDataReviewWidget.h"
#include <QApplication>
#include "MainWindow.h"
#include "GBigDataAPI.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    int r = a.exec();

    GLOBAL_API_MGR.dstory();
    return (r);
}
