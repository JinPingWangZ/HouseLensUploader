#ifndef FTPUPLOADERMANAGER_H
#define FTPUPLOADERMANAGER_H

#include <QObject>
#include "ftpuploader.h"

class FTPUploaderManager : public QObject, public FTPUploaderDelegate
{
    Q_OBJECT
public:
    explicit FTPUploaderManager(QObject *parent = 0);
    virtual ~FTPUploaderManager();

    void setDelegate(FTPUploaderDelegate* delegate) { _delegate = delegate; }
    FTPUploaderDelegate* delegate() { return _delegate; }
    void cancel();
    bool isRunning();
    void uploadFiles(const QStringList& files, const QString& remoteFolder, const QString& username, const QString& password);

private:
    FTPUploader *           _currentUploader;
    FTPUploaderDelegate*    _delegate;
    QString					_remoteFolder;
    QString					_username;
    QString					_password;
    QStringList             _filePaths;

    QStringList             _availableServers;
    int                     _currentServerIndex;

    void uploadToServer();

    // FTPUploaderDelegate
    virtual void FTPUploaderDidConnect(FTPUploader* uploader);
    virtual bool FTPUploaderShouldBeginUploading(FTPUploader* uploader);
    virtual void FTPUploaderDidBeginUploading(FTPUploader* uploader);
    virtual void FTPUploaderWasCancelled(FTPUploader* uploader);
    virtual void FTPUploaderDidFinishUpload(FTPUploader* uploader);
    virtual void FTPUploaderDidFailWithError(FTPUploader* uploader, Error* error);
    virtual void FTPUploaderDidProgressToPercent(FTPUploader* uploader, int percent);    // percent = [1, 100]
    virtual void FTPUploaderIsWaitingNetworkAvailable(FTPUploader* uploader);

signals:

public slots:
};

#endif // FTPUPLOADERMANAGER_H
