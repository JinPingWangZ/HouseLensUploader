#include "mailsender.h"
#include <QApplication>
#include <QDebug>
#include <QDateTime>

MailSender::MailSender(QObject *parent) :
    QObject(parent)
{
    _process = NULL;
    _delegate = NULL;
}

MailSender::~MailSender()
{
    _delegate = NULL;

    if( this->isRunning() ) {
        this->cancel();
    }
}

void MailSender::cancel()
{
    if( _process == NULL )
        return;

    _cancel = true;

    if( _process->state() == QProcess::Running ) {
        _process->kill();
        _process->waitForFinished(1000);
    }
}

bool MailSender::isRunning()
{
    if( _process == NULL )
        return false;

    return true;
}

void MailSender::sendMail(const QString& sender, const QStringList& receivers, const QStringList& ccReceivers, const QString& mailServer, const QString& serverMailAccount, const QString& serverMailPassword, const QString& subject, const QString& content)
{
    if( _delegate != NULL ) {
        if( !_delegate->mailSenderShouldBeginSend(this) )
            return;

        _delegate->mailSenderDidBeginSend(this);
    }

    _cancel = false;


    QString processPath = qApp->applicationDirPath() + "/bin/" + QString("sendEmail.exe");
    // sendEmail -f sender -t receiver1 receiver2 -u subject -m content -s mailServer -xu serverMailAccount -xp serverMailPassword

    QStringList arguments;
    // sender
    arguments << QString("-f");
    arguments << sender;
    // receivers
    arguments << QString("-t");

    Q_FOREACH( QString aReceiver, receivers) {
        arguments << aReceiver;
    }

    // cc receivers
    arguments << QString("-cc");

    Q_FOREACH( QString aReceiver, ccReceivers) {
        arguments << aReceiver;
    }

    // subject
    arguments << QString("-u");
    arguments << subject;
    // content
    arguments << QString("-m");
    arguments << content;
    // server address
    arguments << QString("-s");
    arguments << mailServer;
    // server mail account
    arguments << QString("-xu");
    arguments << serverMailAccount;
    // server mail password
    arguments << QString("-xp");
    arguments << serverMailPassword;

    if( _process != NULL )
        delete _process;

    _process = new QProcess;

	connect(_process, SIGNAL(finished(int)), this, SLOT(processDidFinish(int)));
	connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(processHasReadStandardError()));

    qDebug() << "sendEmail start" << "\n";
    qDebug() << "start time = " << QDateTime::currentDateTime() << "\n";

    _process->start(processPath, arguments);
    return;
}

void MailSender::processHasReadStandardError()
{
	if( _delegate == NULL )
		return;

	QByteArray stderrData = _process->readAllStandardError();
	QString logContent(stderrData);
	qDebug() << "logdata: " << logContent << "\n";
}

void MailSender::processDidFinish(int code)
{
	_process->deleteLater();
	_process = NULL;

    if( _delegate == NULL )
        return;

    qDebug() << "sendEmail return " << code << "\n";

    if( _cancel ) {
        _delegate->mailSenderWasCancelled(this);
        return;
    }

    // success
    if( code == 0 ) {
        _delegate->mailSenderDidFinishSending(this);
    }
    else {
        Error error(MailSenderErrorDomain, 0);
        _delegate->mailSenderDidFailWithError(this, &error);
    }
}

