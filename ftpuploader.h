#ifndef FTPUPLOADER_H
#define FTPUPLOADER_H

#include <QObject>
#include "Error.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include <QElapsedTimer>
#include <QProcess>

class FTPUploader;
class FTPUploaderDelegate
{
public:
    virtual void FTPUploaderDidConnect(FTPUploader* uploader) { Q_UNUSED(uploader) }
    virtual bool FTPUploaderShouldBeginUploading(FTPUploader* uploader) { Q_UNUSED(uploader); return true; }
    virtual void FTPUploaderDidBeginUploading(FTPUploader* uploader) { Q_UNUSED(uploader) }
    virtual void FTPUploaderWasCancelled(FTPUploader* uploader) { Q_UNUSED(uploader) }
    virtual void FTPUploaderDidFinishUpload(FTPUploader* uploader) { Q_UNUSED(uploader) }
    virtual void FTPUploaderDidFailWithError(FTPUploader* uploader, Error* error) { Q_UNUSED(uploader);Q_UNUSED(error) }
    // percent = [1, 100]
    virtual void FTPUploaderDidProgressToPercent(FTPUploader* uploader, int percent) { Q_UNUSED(uploader); Q_UNUSED(percent) }
	virtual void FTPUploaderIsWaitingNetworkAvailable(FTPUploader* uploader) { Q_UNUSED(uploader) }
};

#define FTPUploaderErrorDomain "FTPUploaderErrorDomain"

class FTPUploader : public QObject
{
    Q_OBJECT
public:
    explicit FTPUploader(QObject *parent = 0);
    virtual ~FTPUploader();
    void setDelegate(FTPUploaderDelegate* delegate) { _delegate = delegate; }
    FTPUploaderDelegate* delegate() { return _delegate; }
    void cancel();
    bool isRunning();
    QString currentFileURL();
    int currentFileIndex();
    int totalFileCount();
    QStringList totalFileURLs();
    double leftSeconds();
    void uploadFiles(const QStringList& files, const QString& server, const QString& remoteFolder, const QString& username, const QString& password);
	void uploadFile( QString & file );

    QString server() { return _server; }

private Q_SLOTS:
    void curlHasReadStandardError();
    void curlDidFinish(int code);
	void restartUploading();
private:
    FTPUploaderDelegate*    _delegate;
	QString					_server;
	QString					_remoteFolder;
	QString					_username;
	QString					_password;

    int                     _currentFileIndex;
    QProcess*               _process;
    bool                    _cancel;
    qint64                  _totalUploadSize;
    qint64                  _uploadedSize;
    QStringList             _filePaths;
    QElapsedTimer*          _elapsedTimer;
    bool                    _isConnected;
    int                     _totalPercent;
    QList<qint64>           _fileSizes;
    int                    _restartUpLoadingEnable = 0;
};

#endif // FTPUPLOADER_H
