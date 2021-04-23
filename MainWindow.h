#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
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

private:
    Ui::MainWindow *ui;
    MainWidget *mMainWidget;
};

#endif // MAINWINDOW_H
