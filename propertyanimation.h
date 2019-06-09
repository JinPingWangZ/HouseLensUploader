#ifndef PROPERTYANIMATION_H
#define PROPERTYANIMATION_H

#include <QPropertyAnimation>
#include <QEventLoop>

class PropertyAnimation;
class PropertyAnimationDelegate
{
public:
    virtual void animationDidEnd(PropertyAnimation*) {}
    virtual void animationDidStart(PropertyAnimation*) {}
};

class PropertyAnimation : public QPropertyAnimation
{
    Q_OBJECT
public:
    enum BlockingMode {
       Blocking = 0,
       Nonblocking,
    };

    Q_PROPERTY(PropertyAnimationDelegate* delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(PropertyAnimation::BlockingMode animationBlockingMode READ animationBlockingMode WRITE setAnimationBlockingMode)

// Property {
private:
    PropertyAnimationDelegate* _delegate;
    PropertyAnimation::BlockingMode _blockingMode;

public:
    void setDelegate(PropertyAnimationDelegate* delegate) { _delegate = delegate; }
    PropertyAnimationDelegate* delegate() { return _delegate; }

    void setAnimationBlockingMode(PropertyAnimation::BlockingMode mode) { _blockingMode = mode; }
    PropertyAnimation::BlockingMode animationBlockingMode() { return _blockingMode; }
// } Property

public:
    PropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent = 0);
    ~PropertyAnimation();
    void start(QAbstractAnimation::DeletionPolicy policy = KeepWhenStopped);
private slots:
    void animationDidEnd();
};

#endif // PROPERTYANIMATION_H
