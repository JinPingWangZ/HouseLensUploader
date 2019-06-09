#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QScrollArea>

class ScrollArea;
class ScrollAreaDelegate
{
public:
    virtual void scrollAreaDidChangeViewportRect(ScrollArea*, QRect) {}
};

class ScrollArea : public QScrollArea
{
    Q_OBJECT

    Q_PROPERTY(ScrollAreaDelegate* delegate READ delegate WRITE setDelegate)
public:
    void setDelegate(ScrollAreaDelegate* d) { _delegate = d; }
    ScrollAreaDelegate* delegate() { return _delegate; }
private:
    ScrollAreaDelegate* _delegate;
public:
    explicit ScrollArea(QWidget *parent = 0);

private:
    virtual bool viewportEvent(QEvent*);
};

#endif // SCROLLAREA_H
