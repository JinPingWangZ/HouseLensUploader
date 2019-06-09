#include "scrollarea.h"
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QColor>
#include <QtDebug>
#include <QEvent>

ScrollArea::ScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
    _delegate = NULL;
}

bool ScrollArea::viewportEvent( QEvent * event)
{
    // Because QScrollArea installed event filter for resize event of viewport,
    // so there are no meaning to re-implement resizeEvent of viewport's subclass.
    if( event->type() == QEvent::Resize ) {
        QResizeEvent* resizeEvent = (QResizeEvent*)event;
        qDebug() << "viewport size: " << resizeEvent->size() << "\n";

        if( _delegate != NULL ) {
            _delegate->scrollAreaDidChangeViewportRect(this, this->viewport()->geometry());
        }
    }

    bool flag = QScrollArea::viewportEvent(event);
    return flag;
}
