#include "filecopier.h"
#include <QFileInfo>

class FileCopyThread;
typedef FileCopyThread* FileCopyThreadPtr;

FileCopier::FileCopier(QObject *parent) :
    QObject(parent)
{
    _delegate = NULL;
    _thread = NULL;
}

FileCopier::~FileCopier()
{
    _delegate = NULL;

    if( _thread != NULL ) {
        _thread->terminate();
        _thread->wait();
    }
}

void FileCopier::copyFiles(const QVariantList& copyFileArray)
{
    if( _delegate != NULL ) {
        if( !_delegate->fileCopierShouldBeginCopy(this) )
            return;

        _delegate->fileCopierDidBeginCopy(this);
    }

    _totalFileInfos = copyFileArray;
    _successfulFiles.clear();

    if( _thread != NULL)
        delete _thread;

    _thread = new FileCopyThread(this, &_totalFileInfos, &_successfulFiles);
    connect(_thread, SIGNAL(finished()),this, SLOT(fileCopyDidFinish()));
    connect(_thread, SIGNAL(terminate()),this, SLOT(fileCopyDidTerminate()));
    _thread->start();
}

void FileCopier::cancel()
{
    if( _thread != NULL )
	{
        _thread->terminate();
		_thread->wait();
		_thread = NULL;
	}
}

bool FileCopier::isRunning()
{
    return _thread != NULL;
}

void FileCopier::fileCopyDidFinish()
{
    _thread->deleteLater();
    _thread = NULL;

    if( _delegate != NULL )
        _delegate->fileCopierDidFinishCopy(this);
}

void FileCopier::fileCopyDidTerminate()
{
    _thread->deleteLater();
    _thread = NULL;

    if( _delegate != NULL )
        _delegate->fileCopierWasCancelled(this);
}

// FileCopyThread
FileCopyThread::FileCopyThread(FileCopier* fileCopier, QVariantList* pTotalFileInfos, QStringList * pSuccessfulFiles) : QThread()
{
    _fileCopier = fileCopier;
    _pTotalFileInfos = pTotalFileInfos;
    _pSuccessfulFiles = pSuccessfulFiles;
}

void FileCopyThread::run()
{
    int count = _pTotalFileInfos->count();

    for( int index = 0; index < count; ++index )
    {
        QVariantMap dictionary = _pTotalFileInfos->at(index).toMap();

        QString oldURL = dictionary.value(FileCopierOldURL).toString();
        QString newURL = dictionary.value(FileCopierNewURL).toString();

        if( QFile::copy(oldURL, newURL) )
            _pSuccessfulFiles->append(newURL);
    }
}

