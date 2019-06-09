#ifndef FILECOPIER_H
#define FILECOPIER_H

#include <QThread>
#include <QVariantList>
#include <QProcess>
#include "Error.h"

#define FileCopierErrorDomain "FileCopierErrorDomain"
#define FileCopierOldURL "FileCopierOldURL"
#define FileCopierNewURL "FileCopierNewURL"

class FileCopier;
class FileCopierDelegate
{
public:
    virtual bool fileCopierShouldBeginCopy(FileCopier* fileCopier) { Q_UNUSED(fileCopier); return true; }
    virtual void fileCopierDidBeginCopy(FileCopier* fileCopier) { Q_UNUSED(fileCopier); }
    virtual void fileCopierWasCancelled(FileCopier* fileCopier) { Q_UNUSED(fileCopier); }
    virtual void fileCopierDidFailWithError(FileCopier* fileCopier, Error* error) { Q_UNUSED(fileCopier); Q_UNUSED(error); }
    virtual void fileCopierDidFinishCopy(FileCopier* fileCopier) { Q_UNUSED(fileCopier); }
};

class FileCopier : public QObject
{
    Q_OBJECT
public:
    explicit FileCopier(QObject *parent = 0);
    virtual ~FileCopier();
private:
    FileCopierDelegate*     _delegate;
    QVariantList            _totalFileInfos;
    QThread*                _thread;
    QStringList             _successfulFiles;
public:
    void copyFiles(const QVariantList& fileCopyArray);
    void cancel();
    void setDelegate(FileCopierDelegate* delegate) { _delegate = delegate; }
    FileCopierDelegate* delegate() { return _delegate; }
    int totalFileCount() { return _totalFileInfos.count(); }
    QVariantList totalFileInfos() { return _totalFileInfos; }
    QStringList successfulFiles() { return _successfulFiles; }
    bool isRunning();
private Q_SLOTS:
    void fileCopyDidFinish();
    void fileCopyDidTerminate();
private:
    friend class FileCopyThread;
};

class FileCopyThread : public QThread
{
    Q_OBJECT
public:
    explicit FileCopyThread(FileCopier* fileCopier, QVariantList* pTotalFileInfos, QStringList * pSuccessfulFiles);
private:
    virtual void run();
private:
    FileCopier* _fileCopier;
    QVariantList* _pTotalFileInfos;
    QStringList * _pSuccessfulFiles;
};
#endif // FILECOPIER_H
