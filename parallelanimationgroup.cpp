#include "parallelanimationgroup.h"
#include <QApplication>
#include <QtDebug>

ParallelAnimationGroup::ParallelAnimationGroup(QObject *parent) :
    QParallelAnimationGroup(parent)
{
    _delegate = NULL;
    connect(this, SIGNAL(finished()), this, SLOT(animationDidEnd()));
    _blockingMode = Nonblocking;
}

ParallelAnimationGroup::~ParallelAnimationGroup()
{
//    qDebug() << "~ParallelAnimationGroup" << "\n";
}

void ParallelAnimationGroup::animationDidEnd()
{
    if( _delegate != NULL )
        _delegate->animationDidEnd(this);
}

void ParallelAnimationGroup::start(QAbstractAnimation::DeletionPolicy policy)
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
