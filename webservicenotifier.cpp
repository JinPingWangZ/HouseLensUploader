#include "webservicenotifier.h"
#include <QDebug>
#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

WebServiceNotifier::WebServiceNotifier(QObject *parent) :
    QObject(parent)
{
    _networkReply = NULL;
    _running = false;
    _delegate = NULL;
}

WebServiceNotifier::~WebServiceNotifier()
{
    _delegate = NULL;
    if( _networkReply != NULL )
    {
        _networkReply->abort();
        delete _networkReply;
    }
}

void WebServiceNotifier::notifyBeforeUploading( QString& tokenId, QString& orderID, int totalPics, int totalVideos )
{
    _cancel = false;
    if( _delegate != NULL )
    {
        if( !_delegate->webServiceNotifierShouldBeginNotify(this) )
            return;
        _delegate->webServiceNotifierDidBeginNotify(this);
    }

    // send request
    _notificationType = 0;
    QNetworkRequest httpRequest(QUrl("https://vluploadent.myhouselens.com/GridExtendedService.svc/BOSExt_BeforeUpload"));
    httpRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    QString body = QString("{\"tokenID\":\"%1\", \"workOrderId\":\"%2\", \"noOfImages\":%3, \"noOfVideos\":%4}").arg(tokenId).arg(orderID).arg(totalPics).arg(totalVideos);
    _networkReply = _manager.post(httpRequest, body.toUtf8());
    if( _networkReply->error() != QNetworkReply::NoError )
        return;
    _running = true;

    connect(_networkReply, SIGNAL(finished()), this, SLOT(replyDidFinishLoading()));
    connect(_networkReply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(replyDidSendBodyData(qint64,qint64)));
}

void WebServiceNotifier::notifyAfterUploading( QString& tokenId, QString& orderID, int totalPics, int totalVideos, int uploadedPics, int uploadedVideos, QString editorNote, bool mailingCheck )
{
    Q_UNUSED(totalPics);
    Q_UNUSED(totalVideos);
    Q_UNUSED(mailingCheck);

    _cancel = false;
    if( _delegate != NULL )
    {
        if( !_delegate->webServiceNotifierShouldBeginNotify(this) )
            return;
        _delegate->webServiceNotifierDidBeginNotify(this);
    }

    // send request
    _notificationType = 1;
    QNetworkRequest httpRequest(QUrl("https://vluploadent.myhouselens.com/GridExtendedService.svc/BOSExt_AfterUpload"));
    httpRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    QString body = QString("{\"tokenID\":\"%1\", \"uploadId\":\"%2\", \"workOrderId\":\"%3\", \"imagesUploaded\":%4, \"videosUploaded\":%5, \"editorsNotes\":\"%6\"}").arg(tokenId).arg(_uploadId).arg(orderID).arg(uploadedPics).arg(uploadedVideos).arg(editorNote);
    qDebug() << body << "\n";
    _networkReply = _manager.post(httpRequest, body.toUtf8());
    if( _networkReply->error() != QNetworkReply::NoError )
        return;
    _running = true;

    connect(_networkReply, SIGNAL(finished()), this, SLOT(replyDidFinishLoading()));
    connect(_networkReply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(replyDidSendBodyData(qint64,qint64)));
}

void WebServiceNotifier::sendUploadingFailEmail(QString &userID, QString &shootTitle, int totalPics, int totalVideos)
{
    QNetworkRequest httpRequest(QUrl("https://testuploadservice.myhouselens.com/GridExtendedService.svc/SendUploadFailEmail"));
    httpRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    QString body = QString("{\"userID\":\"%1\", \"shootTitle\":\"%2\", \"noOfImages\":%3, \"noOfVideos\":%4}").arg(userID).arg(shootTitle).arg(totalPics).arg(totalVideos);
    _networkReply = _manager.post(httpRequest, body.toUtf8());

    if( _networkReply->error() != QNetworkReply::NoError )
        return;

}

bool WebServiceNotifier::isRunning()
{
    return _running;
}

void WebServiceNotifier::cancel()
{
    _running = false;
    _cancel = true;

    if( _networkReply == NULL )
        return;

    _networkReply->abort();
    _networkReply->deleteLater();
    _networkReply = NULL;

    if( _delegate != NULL )
        _delegate->webServiceNotifierWasCancelled(this);
}

void WebServiceNotifier::replyDidSendBodyData(qint64 bytesWritten, qint64 bytesTotal)
{
    if( _cancel )
        return;

    if( _delegate == NULL )
        return;

    int percent = 0;
    if( bytesTotal > 0 )
        percent = bytesWritten*100.0/bytesTotal;
    _delegate->webServiceNotifierDidProgressToPercent(this, percent);
}

void WebServiceNotifier::replyDidFinishLoading()
{
	_running = false;

    qDebug() << "WebServiceNotifier reply did finish loading: " << _networkReply->error() << "\n";
    _networkReply->deleteLater();

    if( _delegate == NULL )
    {
        _networkReply = NULL;
        return;
    }

    if( _networkReply->error() != QNetworkReply::NoError )
    {
        this->failedWithMessage("Webservice failed.");
        _networkReply = NULL;
        return;
    }

    QByteArray httpData = _networkReply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(httpData);
    if( jsonDoc.isEmpty() )
    {
        this->failedWithMessage("Webservice failed with no response.");
        _networkReply = NULL;
        return;
    }

    QJsonObject json = jsonDoc.object();

    if( _notificationType == 0 )
    {
        QString uploadId = json["Data"].toString();
        if( uploadId.isEmpty() )
            this->failedWithMessage(json["Message"].toString());
        else
        {
            _uploadId = uploadId;
            _delegate->webServiceNotifierDidFinishNotificationBeforeUploading(this);
        }
    }
    else
    {
        QString message = json["Message"].toString();
        if( message.contains("Success", Qt::CaseInsensitive) )
        {
            _delegate->webServiceNotifierDidFinishNotificationAfterUploading(this);
        }
        else
        {
            this->failedWithMessage(message);
        }
    }

    _networkReply = NULL;
}

void WebServiceNotifier::failedWithMessage(QString msg)
{
    QVariantMap userInfo;
    userInfo[LocalizedFailureReasonErrorKey] = msg;
    Error error(WebServiceNotifierErrorDomain, WebServiceNotifierErrorCode_ReturnFail, userInfo);
    _delegate->webServiceNotifierDidFailWithError(this, &error);
}
