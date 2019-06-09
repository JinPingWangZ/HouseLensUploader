#include "scrollareaviewportwidget.h"
#include <QPainter>
#include <QColor>
#include <QtDebug>
#include <QEvent>

ScrollAreaViewPortWidget::ScrollAreaViewPortWidget(QWidget *parent) :
    QWidget(parent)
{
}

void ScrollAreaViewPortWidget::paintEvent(QPaintEvent *e)
{
    return QWidget::paintEvent(e);
//    QPainter myPainter(this);
//    myPainter.fillRect(this->geometry(), QBrush(QColor::fromRgb(255,0,0,255)));
}
