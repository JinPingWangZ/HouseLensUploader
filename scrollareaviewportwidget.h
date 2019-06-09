#ifndef SCROLLAREAVIEWPORTWIDGET_H
#define SCROLLAREAVIEWPORTWIDGET_H

#include <QWidget>

class ScrollAreaViewPortWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScrollAreaViewPortWidget(QWidget *parent = 0);
private:
    virtual void paintEvent(QPaintEvent *e);
};

#endif // SCROLLAREAVIEWPORTWIDGET_H
