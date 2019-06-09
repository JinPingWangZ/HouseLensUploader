#include "ftpuploadermanager.h"
#include "preference.h"

FTPUploaderManager::FTPUploaderManager(QObject *parent) : QObject(parent)
{
    _currentUploader = NULL;
    _delegate = NULL;
}

FTPUploaderManager::~FTPUploaderManager()
{
    if( _currentUploader != NULL )
        delete _currentUploader;
    _currentUploader = NULL;
    _delegate = NULL;
}

void FTPUploaderManager::cancel()
{
    if( _currentUploader != NULL )
        _currentUploader->cancel();
}

bool FTPUploaderManager::isRunning()
{
    if( _currentUploader != NULL )
        return _currentUploader->isRunning();
    return false;
}

void FTPUploaderManager::uploadFiles(const QStringList& files, const QString& remoteFolder, const QString& username, const QString& password)
{
    _filePaths = files;
    _remoteFolder = remoteFolder;
    _username = username;
    _password = password;
    _availableServers = Preference::availableServers();
    _currentServerIndex = 0;

    uploadToServer();
}

void FTPUploaderManager::uploadToServer()
{
    if( _currentServerIndex < _availableServers.count() )
    {
        QString server = _availableServers[_currentServerIndex];
        if( _currentUploader != NULL )
            delete _currentUploader;
        _currentUploader = new FTPUploader();
        _currentUploader->setDelegate(this);
        _currentUploader->uploadFiles(_filePaths, server, _remoteFolder, _username, _password);
    }
    else
    {
        //Completed
        if( _delegate != NULL )
            _delegate->FTPUploaderDidFinishUpload(NULL);
    }
}

void FTPUploaderManager::FTPUploaderDidConnect(FTPUploader* uploader)
{
    if( _delegate != NULL )
        _delegate->FTPUploaderDidConnect(uploader);
}

bool FTPUploaderManager::FTPUploaderShouldBeginUploading(FTPUploader* uploader)
{
    if( _delegate != NULL )
        return _delegate->FTPUploaderShouldBeginUploading(uploader);
    return true;
}

void FTPUploaderManager::FTPUploaderDidBeginUploading(FTPUploader* uploader)
{
    if( _delegate != NULL )
        _delegate->FTPUploaderDidBeginUploading(uploader);
}

void FTPUploaderManager::FTPUploaderWasCancelled(FTPUploader* uploader)
{
    if( _delegate != NULL )
        _delegate->FTPUploaderWasCancelled(uploader);
}

void FTPUploaderManager::FTPUploaderDidFinishUpload(FTPUploader* uploader)
{
    Q_UNUSED(uploader);
    _currentServerIndex++;
    uploadToServer();
}

void FTPUploaderManager::FTPUploaderDidFailWithError(FTPUploader* uploader, Error* error)
{
    if( _delegate != NULL )
        _delegate->FTPUploaderDidFailWithError(uploader, error);
}

void FTPUploaderManager::FTPUploaderDidProgressToPercent(FTPUploader* uploader, int percent)    // percent = [1, 100]
{
    if( _delegate != NULL )
        _delegate->FTPUploaderDidProgressToPercent(uploader, percent);
}

void FTPUploaderManager::FTPUploaderIsWaitingNetworkAvailable(FTPUploader* uploader)
{
    if( _delegate != NULL )
        _delegate->FTPUploaderIsWaitingNetworkAvailable(uploader);
}
