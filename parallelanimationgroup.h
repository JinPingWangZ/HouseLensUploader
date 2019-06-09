#ifndef ParallelANIMATIONGROUP_H
#define ParallelANIMATIONGROUP_H

#include <QParallelAnimationGroup>
#include <QEventLoop>

class ParallelAnimationGroup;
class ParallelAnimationGroupDelegate
{
public:
    virtual void animationDidEnd(ParallelAnimationGroup*) {}
    virtual void animationDidStart(ParallelAnimationGroup*) {}
};

class ParallelAnimationGroup : public QParallelAnimationGroup
{
    Q_OBJECT
public:
    enum BlockingMode {
       Blocking = 0,
       Nonblocking,
    };

    Q_PROPERTY(ParallelAnimationGroupDelegate* delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(ParallelAnimationGroup::BlockingMode animationBlockingMode READ animationBlockingMode WRITE setAnimationBlockingMode)

// Parallel {
private:
    ParallelAnimationGroupDelegate* _delegate;
    ParallelAnimationGroup::BlockingMode _blockingMode;
public:
    void setDelegate(ParallelAnimationGroupDelegate* delegate) { _delegate = delegate; }
    ParallelAnimationGroupDelegate* delegate() { return _delegate; }

    void setAnimationBlockingMode(ParallelAnimationGroup::BlockingMode mode) { _blockingMode = mode; }
    ParallelAnimationGroup::BlockingMode animationBlockingMode() { return _blockingMode; }

// } Parallel

public:
    explicit ParallelAnimationGroup(QObject *parent = 0);
    ~ParallelAnimationGroup();
    void start(QAbstractAnimation::DeletionPolicy policy = KeepWhenStopped);
private slots:
    void animationDidEnd();
};

#endif // ParallelANIMATIONGROUP_H
