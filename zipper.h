#ifndef ZIPPER_H
#define ZIPPER_H

#include <QObject>
#include "Error.h"
#include <QStringList>
#include <QProcess>

#define ZipperErrorDomain "ZipperErrorDomain"

class Zipper;
class ZipperDelegate {
public:
    virtual bool zipperShouldBeginZip(Zipper* zipper) { Q_UNUSED(zipper);return true; }
    virtual void zipperDidBeginZip(Zipper* zipper) { Q_UNUSED(zipper) }
    virtual void zipperWasCancelled(Zipper* zipper) { Q_UNUSED(zipper) }
    virtual void zipperDidFinishZip(Zipper* zipper) { Q_UNUSED(zipper) }
    virtual void zipperDidFailWithError(Zipper* zipper, Error* error) { Q_UNUSED(zipper); Q_UNUSED(error) }
};

class Zipper : public QObject
{
    Q_OBJECT
public:
    explicit Zipper(QObject *parent = 0);
    virtual ~Zipper();
    void setDelegate(ZipperDelegate* delegate) { _delegate = delegate; }
    ZipperDelegate* delegate() { return _delegate; }
    QString archiveName() { return _archiveName; }
    QString directory() { return _directory; }
    void zipFiles(const QStringList& fileNames, const QString& folder, const QString& archiveName);
    void cancel();
    bool isRunning();
private Q_SLOTS:
    void zipperDidFinish(int retCode);
private:
    ZipperDelegate* _delegate;
    QStringList		_sourceFileNames;
    QString         _archiveName;
    QString         _directory;
    bool            _cancel;
    QProcess*       _process;
};

#endif // ZIPPER_H
