#ifndef DNGCONVERTER_H
#define DNGCONVERTER_H

#include <QObject>
#include "Error.h"
#include <QProcess>

#define DNGConverterErrorDomain "DNGConverterErrorDomain"
#define DCRAWURL "DCRAWURL"
#define DCDNGURL "DCDNGURL"
#define DCSUCESS "DCSUCCES"

class DNGConverter;
class DNGConverterDelegate
{
public:
    virtual bool dngConverterShouldBeginConverting(DNGConverter* converter) { Q_UNUSED(converter); return true; }
    virtual void dngConverterDidBeginConverting(DNGConverter* converter) { Q_UNUSED(converter); }
    virtual void dngConverterWasCancelled(DNGConverter* converter) { Q_UNUSED(converter); }
    virtual bool dngConverterShouldContinueProcessing(DNGConverter* converter) { Q_UNUSED(converter); return true; }
    virtual void dngConverterDidProgressToPercent(DNGConverter* converter, int percent) { Q_UNUSED(converter); Q_UNUSED(percent); }
    virtual void dngConverterDidFailWithError(DNGConverter* converter, Error* error) { Q_UNUSED(converter); Q_UNUSED(error); }
    virtual void dngConverterDidFinishConverting(DNGConverter* converter) { Q_UNUSED(converter); }
};

class DNGConverter : public QObject
{
    Q_OBJECT
public:
    explicit DNGConverter(QObject *parent = 0);
    virtual ~DNGConverter();

private:
    DNGConverterDelegate*   _delegate;
    QVariantList            _totalFileInfos;
    QVariantList            _resultFileInfos;
    QProcess*               _process;
    bool                    _cancel;
    int                     _currentIndex;
public:
    static bool isAvailable();

    void convertRawFiles(const QVariantList & fileInfos);
    void cancel();
    void setDelegate(DNGConverterDelegate* delegate) { _delegate = delegate; }
    DNGConverterDelegate* delegate() { return _delegate; }
    int totalFileCount() { return _totalFileInfos.count(); }
    int currentFileIndex() { return _currentIndex; }
    QVariantMap currentFileInfo() { return _totalFileInfos[_currentIndex].toMap(); }
    QVariantList totalFileInfos() { return _totalFileInfos; }
    QVariantList resultFileInfos() { return _resultFileInfos; }
    bool isRunning();

private:
     void nextConvertRawFile();

signals:

public slots:
    void dngConverterDidFinish(int retCode);
};

#endif // DNGCONVERTER_H
