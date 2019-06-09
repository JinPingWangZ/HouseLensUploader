#include "ftpuploader.h"
#include <QFileInfo>
#include <QDebug>
#include <QApplication>
#include <QTimer>
#include <QThread>

FTPUploader::FTPUploader(QObject *parent) :
    QObject(parent)
{
    _currentFileIndex = -1;
    _process = NULL;
    _elapsedTimer = NULL;
    _delegate = NULL;
}

FTPUploader::~FTPUploader()
{
    _delegate = NULL;
    if( this->isRunning() )
        this->cancel();

    delete _elapsedTimer;
}

void FTPUploader::cancel()
{
    if( _process == NULL )
        return;

    _cancel = true;

    if( _process->state() == QProcess::Running )
	{
        _process->kill();
        _process->waitForFinished(-1);
	}

	_process->deleteLater();
	_process = NULL;
}

bool FTPUploader::isRunning()
{
    if( _process == NULL )
        return false;

    return true;
}

void FTPUploader::uploadFiles(const QStringList& files, const QString& server, const QString& remoteFolder, const QString& username, const QString& password)
{
    if( files.count() == 0 )
        return;

    _server = server;
    _remoteFolder = remoteFolder;
    _username = username;
    _password = password;

    if( _delegate != NULL ) 
	{
        if( !_delegate->FTPUploaderShouldBeginUploading(this) )
            return;

        _delegate->FTPUploaderDidBeginUploading(this);
    }

	if( _server.isEmpty() || _remoteFolder.isEmpty() || _username.isEmpty() || _password.isEmpty() )
	{
        Error error("FTP Server Info is wrong", 0);
        _delegate->FTPUploaderDidFailWithError(this, &error);
		return;
	}

    _fileSizes.clear();
    _totalUploadSize = 0;
    Q_FOREACH( QString aFile, files ) 
	{
        QFileInfo fileInfo(aFile);
        qint64 fileSize = fileInfo.size();
        _fileSizes.push_back(fileSize);
        _totalUploadSize += fileSize;
    }
    _totalPercent = 0;
    _cancel = false;
    _filePaths = files;

	if( _totalUploadSize == 0 )
	{
        Error error("There is no file to upload", 0);
        _delegate->FTPUploaderDidFailWithError(this, &error);
		return;
	}

	_currentFileIndex = 0;
	this->uploadFile( this->currentFileURL() );
}

void FTPUploader::uploadFile( QString & file )
{
    _isConnected = false;
    _uploadedSize = 0;

    // curl -T "{/volumes/data/1.tiff,/volumes/data/2.tiff}" -u "Editors:3l@nc3r" "ftp://videolens.dyndns.tv/VideoLensNAS/Shoots - Shared/Residential/05 Test,Matiur - matiur rahman/" -v

    QString curlPath = qApp->applicationDirPath() + QString("/bin/curl.exe");
    QStringList arguments;
	arguments << "-T" << QString("{%1}").arg(file) << "-u" << QString("%1:%2").arg(_username).arg(_password);
	arguments << "--ftp-create-dirs" << QString("ftp://%1%2/").arg(_server).arg(_remoteFolder) << "-v";

    qDebug() << "arguments:"<< arguments << "\n";

    if( _process != NULL )
		_process->deleteLater();

    _process = new QProcess;

    connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(curlHasReadStandardError()));
    connect(_process, SIGNAL(finished(int)), this, SLOT(curlDidFinish(int)));

    qDebug() << "curl start" << "\n";
    qDebug() << "start time = " << QDateTime::currentDateTime() << "\n";

    _process->start(curlPath, arguments);
    return;
}

QString FTPUploader::currentFileURL()
{
    return _filePaths[_currentFileIndex];
}

int FTPUploader::currentFileIndex()
{
    return _currentFileIndex;
}

int FTPUploader::totalFileCount()
{
    return _filePaths.count();
}

QStringList FTPUploader::totalFileURLs()
{
    return _filePaths;
}

double FTPUploader::leftSeconds()
{
    if( _elapsedTimer == NULL )
        return -1.0;

    qint64 uploadedBytes = 0;
    for( int index = 0; index < _currentFileIndex; ++index ) 
        uploadedBytes += _fileSizes[index];

    uploadedBytes += _uploadedSize;
    if( uploadedBytes == 0 )
        return -1.0;

    // sentBytes:totalBytes = elapsedTime:totalTime;
    double totalTime = (double)_totalUploadSize * _elapsedTimer->elapsed() / (double)uploadedBytes;
    return (totalTime - _elapsedTimer->elapsed()) / 1000;
}

void FTPUploader::curlHasReadStandardError()
{
    if( _delegate == NULL )
        return;

    QByteArray stderrData = _process->readAllStandardError();
    QString logContent(stderrData);

	QStringList logLines =logContent.split( "\n" );
	for( int i = 0 ; i < logLines.count() ; i++ )
	{
		QString logLine = logLines.at( i );
        qDebug() << "LogLine( i =" << i << " ): " << logLine << "\n";
        QThread *qh = new QThread;
//        qh->msleep(500);
        if( logLine == "* couldn't connect to host\r " )
            break;

		if( _isConnected == false )
		{
			if( logLine.indexOf( "< 230" ) >= 0 )
			{
				_isConnected = true;
		        if( _elapsedTimer != NULL )
			        delete _elapsedTimer;

				_elapsedTimer = new QElapsedTimer;
				_elapsedTimer->start();
				_delegate->FTPUploaderDidConnect(this);
			}

			continue;
		}

	    // < 150 Opening BINARY mode data connection for '0001.m4v'.
		//if( logContent.indexOf("< 150") != -1 ) 
		//{
		//	_uploadedSize = 0;
		//	_currentFileIndex++;
		//	continue;
		//}		


		// 100  220k    0     0  100  220k      0  30673  0:00:07  0:00:07 --:--:--
	    QStringList stringComponents = logLine.split(" ");
		int count = stringComponents.count();
		for( int index = count-1; index >= 0; index-- )
		{
			QString aComponent = stringComponents[index];
			if( aComponent.isEmpty() )
				stringComponents.removeAt(index);
		}

        if( stringComponents.count() != 13 )
            continue;


		
		QString sizeString = stringComponents[6];
	    int sizeInBytes;
		bool ok;
		if( sizeString.right(1) == QString("M") ) 
		{
	        sizeInBytes = sizeString.left(sizeString.length()-1).toDouble(&ok);
			if( ok ) 
		        sizeInBytes = sizeInBytes * 1024 * 1024;
			else
				continue;
	    }
	    else if( sizeString.right(1) == QString("k") ) 
		{
	        sizeInBytes = sizeString.left(sizeString.length()-1).toDouble(&ok);
	        if( ok )
				sizeInBytes *= 1024;
			else
				continue;
		}
	    else 
		{
	        sizeInBytes = sizeString.toDouble(&ok);
			if( !ok )
				continue;
	    }

		_uploadedSize = sizeInBytes;
	    if( _uploadedSize > _totalUploadSize )
		    _uploadedSize = _totalUploadSize;

	    QString percentString = stringComponents[1];
	    int percent = percentString.toInt(&ok);
	    if( ok && percent >= 0 && percent <= 100 ) 
		{
			_totalPercent = _fileSizes[_currentFileIndex] * percent / (double)_totalUploadSize;
	        _delegate->FTPUploaderDidProgressToPercent(this, _totalPercent);
		}
	}

}

void FTPUploader::curlDidFinish(int code)
{
    Q_UNUSED(code);

    qDebug() << "curl finish:" << code << "\n";

    if( _delegate == NULL )
        return;

    if( _cancel ) 
	{
        _delegate->FTPUploaderWasCancelled(this);
        return;
    }

    if( code == 0 )
	{
		_currentFileIndex++;
        _delegate->FTPUploaderDidFinishUpload(this);
        //this->restartUploading();
	}
    else 
	{
		_delegate->FTPUploaderIsWaitingNetworkAvailable(this);
//        _restartUpLoadingEnable = !(_restartUpLoadingEnable);
        if ( _restartUpLoadingEnable < 3 ){
            qDebug() << "---- restartUpLoading!";
            _restartUpLoadingEnable++;
            QTimer::singleShot( 2000, this, SLOT(restartUploading()) );

        }else{
            qDebug() << "----stop restartUpLoading!";
            _restartUpLoadingEnable = 0;
            Error error(FTPUploaderErrorDomain, 0);
            _delegate->FTPUploaderDidFailWithError(this, &error);
        }
    }
}

void FTPUploader::restartUploading()
{
	if( _currentFileIndex < _filePaths.count() )
	{
		this->uploadFile( this->currentFileURL() );
	}
	else
	{
		_delegate->FTPUploaderDidFinishUpload(this);
	}
}
