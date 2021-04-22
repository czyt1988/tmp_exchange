#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include <QtWidgets/QWidget>
#include "GTemplate.h"
#include "GModuleValueView.h"
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

private:
    void deleteTemplates();

private:
    Ui::MainWidget *ui;
    QList<GTemplate *> mTemplate;
    GTemplate *mCurrentTemplate;
    GModuleValueView *mModuleWidget;
};

#endif // MAINWIDGET_H
