#ifndef WEBSERVICENOTIFIER_H
#define WEBSERVICENOTIFIER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "Error.h"
#include "qglobal.h"

enum {
    WebServiceNotifierErrorCode_NotReachToService = 0,// notification was not reached to web service.
    WebServiceNotifierErrorCode_ReturnFail,// web service return "Fail"
};

typedef int WebServiceNotifierErrorCode;

#define WebServiceNotifierErrorDomain "WebServiceNotifierErrorDomain"

class WebServiceNotifier;
class WebServiceNotifierDelegate
{
public:
    virtual bool webServiceNotifierShouldBeginNotify(WebServiceNotifier* notifier) { Q_UNUSED(notifier); return true; }
    virtual void webServiceNotifierDidBeginNotify(WebServiceNotifier* notifier)  { Q_UNUSED(notifier); }
    virtual void webServiceNotifierWasCancelled(WebServiceNotifier* notifier) { Q_UNUSED(notifier); }
    virtual void webServiceNotifierDidFailWithError(WebServiceNotifier* notifier, Error* error) { Q_UNUSED(notifier);Q_UNUSED(error); }
    // percent = [1, 100]
    virtual void webServiceNotifierDidProgressToPercent(WebServiceNotifier* notifier, int percent) { Q_UNUSED(notifier); Q_UNUSED(percent); }
    virtual void webServiceNotifierDidFinishNotificationBeforeUploading(WebServiceNotifier* notifier) { Q_UNUSED(notifier); }
    virtual void webServiceNotifierDidFinishNotificationAfterUploading(WebServiceNotifier* notifier) { Q_UNUSED(notifier); }
};

class WebServiceNotifier : public QObject
{
    Q_OBJECT
public:
    explicit WebServiceNotifier(QObject *parent = 0);
    virtual ~WebServiceNotifier();
    void cancel();
    void setDelegate(WebServiceNotifierDelegate* delegate) { _delegate = delegate; }
    WebServiceNotifierDelegate* delegate() { return _delegate; }
    bool isRunning();

    void notifyBeforeUploading( QString& tokenId, QString& orderID, int totalPics, int totalVideos );
    void notifyAfterUploading( QString& tokenId, QString& orderID, int totalPics, int totalVideos, int uploadedPics, int uploadedVideos, QString editorNote, bool mailingCheck );

    void sendUploadingFailEmail( QString& userID, QString& shootTitle, int totalPics, int totalVideos );

private Q_SLOTS:
    void replyDidFinishLoading();
    void replyDidSendBodyData(qint64 bytesWritten, qint64 bytesTotal);
private:
    QNetworkAccessManager       _manager;
    WebServiceNotifierDelegate* _delegate;
    bool                        _cancel;
    bool                        _running;
    QNetworkReply*              _networkReply;
    QString                     _uploadId;

    // 0 means "BeforeUploading"
    // 1 means "AfterUploading"
    int                         _notificationType;

    void failedWithMessage(QString msg);
};

#endif // WEBSERVICENOTIFIER_H
