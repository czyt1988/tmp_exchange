#ifndef GITEMDISPLAYWIDGET_H
#define GITEMDISPLAYWIDGET_H


#include <QtWidgets/QWidget>
class QPaintEvent;

namespace Ui {
class GItemDisplayWidget;
}

class GItemDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GItemDisplayWidget(QWidget *parent = 0);
    ~GItemDisplayWidget();
    void setValue(const QString& value);
    void setName(const QString& name);
    void setSuffix(const QString& sf);

protected:
    void paintEvent(QPaintEvent *e) override;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::GItemDisplayWidget *ui;
    QBrush m_background;
};

#endif // GITEMDISPLAYWIDGET_H
