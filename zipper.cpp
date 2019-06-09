#include "zipper.h"
#include <QApplication>
#include <QDebug>
#include <QFile>

Zipper::Zipper(QObject *parent) :
    QObject(parent)
{
    _process = NULL;
    _delegate = NULL;
}

Zipper::~Zipper()
{
    if( this->isRunning() ) {
        this->cancel();
    }
}

void Zipper::zipFiles(const QStringList& fileNames, const QString& folder, const QString& archiveName)
{
    _cancel = false;
    _sourceFileNames = fileNames;
    _directory = folder;
    _archiveName = archiveName;

    if( _delegate != NULL && !_delegate->zipperShouldBeginZip(this) )
        return;

    if( _delegate != NULL )
        _delegate->zipperDidBeginZip(this);

    QString archivePath = QString(folder) + QString("/") + archiveName;
    if( QFile::exists(archivePath) && !QFile::remove(archivePath) ) {

        if( _delegate != NULL ) {
			QVariantMap userInfo;
			userInfo[LocalizedDescriptionKey] = "Zip failed";
			_delegate->zipperDidFailWithError(this, new Error(ZipperErrorDomain, 0, userInfo));
		}

		return;
	}

    if( _process != NULL )
        delete _process;

    _process = new QProcess;

    connect(_process, SIGNAL(finished(int)), this, SLOT(zipperDidFinish(int)));

    // zip -r "archive.zip" "a.mov" "b.mov" "c.mov"
    QString commandPath = qApp->applicationDirPath() + "/bin/zip.exe";
    QStringList arguments;
    arguments << "-r";
    arguments << this->archiveName();

    Q_FOREACH( QString aFileName, _sourceFileNames ) {
        arguments << aFileName;
    }

    qDebug() << "zipping " << arguments << "\n";
    _process->setWorkingDirectory(this->directory());

    qDebug() << "zip start " << _sourceFileNames << "\n";
    _process->start(commandPath, arguments);
    return;
}

void Zipper::zipperDidFinish(int retCode)
{
    _process->deleteLater();
    _process = NULL;

    qDebug() << "zip return " << retCode << "\n";

    if( _delegate != NULL ) {
        if( _cancel ) {
            _delegate->zipperWasCancelled(this);
            return;
        }

        if( retCode == 0 )
            _delegate->zipperDidFinishZip(this);
        else {
            QVariantMap userInfo;
            userInfo[LocalizedDescriptionKey] = "Zip failed";
            _delegate->zipperDidFailWithError(this, new Error(ZipperErrorDomain, 0, userInfo));
        }
    }
}

void Zipper::cancel()
{
    if( _process == NULL )
        return;

    _cancel = true;
    if( _process->state() != QProcess::NotRunning ) {
        _process->kill();
        _process->waitForFinished(-1);
    }
}

bool Zipper::isRunning()
{
    if( _process == NULL )
        return false;

    return _process->state() != QProcess::NotRunning;
}

