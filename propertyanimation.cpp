#include "propertyanimation.h"
#include <QtDebug>
#include <QEvent>
#include <QApplication>

PropertyAnimation::PropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent) : QPropertyAnimation(target, propertyName, parent)
{
    _delegate = NULL;
    connect(this, SIGNAL(finished()), this, SLOT(animationDidEnd()));
    _blockingMode = Nonblocking;
}

PropertyAnimation::~PropertyAnimation()
{
//    qDebug() << "~PropertyAnimation" << "\n";
}

void PropertyAnimation::animationDidEnd()
{
    if( _delegate != NULL )
        _delegate->animationDidEnd(this);
}

void PropertyAnimation::start(QAbstractAnimation::DeletionPolicy policy)
{
    QAbstractAnimation::start(policy);

    if( _delegate != NULL )
        _delegate->animationDidStart(this);

    if( _blockingMode == Blocking ) {
        while( this->state() == QAbstractAnimation::Running ) {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }
}
