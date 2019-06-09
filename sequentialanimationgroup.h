#ifndef SEQUENTIALANIMATIONGROUP_H
#define SEQUENTIALANIMATIONGROUP_H

#include <QSequentialAnimationGroup>

class SequentialAnimationGroup;
class SequentialAnimationGroupDelegate
{
public:
    virtual void animationDidEnd(SequentialAnimationGroup*) {}
    virtual void animationDidStart(SequentialAnimationGroup*) {}
};

class SequentialAnimationGroup : public QSequentialAnimationGroup
{
    Q_OBJECT
public:
    enum BlockingMode {
       Blocking = 0,
       Nonblocking,
    };

    Q_PROPERTY(SequentialAnimationGroupDelegate* delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(SequentialAnimationGroup::BlockingMode animationBlockingMode READ animationBlockingMode WRITE setAnimationBlockingMode)
// Property {
private:
    SequentialAnimationGroupDelegate* _delegate;
    SequentialAnimationGroup::BlockingMode _blockingMode;
public:
    void setDelegate(SequentialAnimationGroupDelegate* delegate) { _delegate = delegate; }
    SequentialAnimationGroupDelegate* delegate() { return _delegate; }

    void setAnimationBlockingMode(SequentialAnimationGroup::BlockingMode mode) { _blockingMode = mode; }
    SequentialAnimationGroup::BlockingMode animationBlockingMode() { return _blockingMode; }
// } Property

public:
    explicit SequentialAnimationGroup(QObject *parent = 0);
    ~SequentialAnimationGroup();
    void start(QAbstractAnimation::DeletionPolicy policy = KeepWhenStopped);
private slots:
    void animationDidEnd();
};

#endif // MYSEQUENTIALANIMATIONGROUP_H
