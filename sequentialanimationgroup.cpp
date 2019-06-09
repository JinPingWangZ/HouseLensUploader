#include "sequentialanimationgroup.h"
#include <QtDebug>
#include <QEvent>
#include <QApplication>

SequentialAnimationGroup::SequentialAnimationGroup(QObject *parent) :
    QSequentialAnimationGroup(parent)
{
    _delegate = NULL;
    connect(this, SIGNAL(finished()), this, SLOT(animationDidEnd()));
    _blockingMode = Nonblocking;
}

SequentialAnimationGroup::~SequentialAnimationGroup()
{
//    qDebug() << "~MySequentialAnimationGroup" << "\n";
}

void SequentialAnimationGroup::animationDidEnd()
{
    if( _delegate != NULL )
        _delegate->animationDidEnd(this);
}

void SequentialAnimationGroup::start(QAbstractAnimation::DeletionPolicy policy)
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

