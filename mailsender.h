#ifndef MAILSENDER_H
#define MAILSENDER_H

#include <QObject>
#include "Error.h"
#include <QStringList>
#include <QProcess>

#define MailSenderErrorDomain "MailSenderErrorDomain"

class MailSender;
class MailSenderDelegate
{
public:
    virtual bool mailSenderShouldBeginSend(MailSender* mailSender) { Q_UNUSED(mailSender); return true; }
    virtual void mailSenderDidBeginSend(MailSender* mailSender) { Q_UNUSED(mailSender); }
    virtual void mailSenderWasCancelled(MailSender* mailSender) { Q_UNUSED(mailSender); }
    virtual void mailSenderDidFailWithError(MailSender* mailSender, Error* error) { Q_UNUSED(mailSender); Q_UNUSED(error); }
    virtual void mailSenderDidFinishSending(MailSender* mailSender) { Q_UNUSED(mailSender); }
};

class MailSender : public QObject
{
    Q_OBJECT
public:
    explicit MailSender(QObject *parent = 0);
    virtual ~MailSender();
    void cancel();
    void sendMail(const QString& sender, const QStringList& receivers, const QStringList& ccReceivers, const QString& mailServer, const QString& serverMailAccount, const QString& serverMailPassword, const QString& subject, const QString& content);
    bool isRunning();
    void setDelegate(MailSenderDelegate* delegate) { _delegate = delegate; }
    MailSenderDelegate* delegate() { return _delegate; }

private Q_SLOTS:
    void processDidFinish(int code);
	void processHasReadStandardError();
private:
    bool _cancel;
    QProcess* _process;
    MailSenderDelegate* _delegate;
};

#endif // MAILSENDER_H
