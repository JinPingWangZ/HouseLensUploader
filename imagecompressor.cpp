/*

		Added by Myongsok Kim on July 30th, 2013

		Restrict image not to overlow the maximum DPI and Pixel resolution.

*/


#include "imagecompressor.h"
#include <QImage>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

ImageCompressor::ImageCompressor(QObject *parent)
	: QObject(parent)
{
	_delegate = NULL;
	_thread = NULL;
}

ImageCompressor::~ImageCompressor()
{
    _delegate = NULL;

    if( _thread != NULL ) 
	{
        _thread->terminate();
        _thread->wait();
    }
}


// - Public Methods -------------------------------------------------------------------

void ImageCompressor::compressImages(const QVariantList & fileArray)
{
    if( _delegate != NULL ) 
	{
        if( !_delegate->imageCompressorShouldBegin(this) )
            return;
        _delegate->imageCompressorDidBegin(this);
    }

    _totalFiles = fileArray;
    _successfulFiles.clear();

	if( _thread != NULL)
        delete _thread;

    _thread = new ImageCompressorThread(this, &_totalFiles, &_successfulFiles);
    connect(_thread, SIGNAL(finished()),this, SLOT(imageCompressionDidFinish()));
    connect(_thread, SIGNAL(terminated()),this, SLOT(imageCompressionDidTerminate()));
    _thread->start();
}

bool ImageCompressor::isRunning()
{
    return _thread != NULL;
}

void ImageCompressor::cancel()
{
    if( _thread != NULL )
	{
        _thread->terminate();
		_thread->wait();
		_thread = NULL;
	}
}


// - Singnals ---------------------------------------------------------------------------

void ImageCompressor::imageCompressionDidFinish()
{
    _thread->deleteLater();
    _thread = NULL;

    if( _delegate != NULL )
        _delegate->imageCompressorDidFinish(this);
}

void ImageCompressor::imageCompressionDidTerminate()
{
    _thread->deleteLater();
    _thread = NULL;

    if( _delegate != NULL )
        _delegate->imageCompressorWasCancelled(this);
}



// - ImageCompressorThread ---------------------------------------------------------------

ImageCompressorThread::ImageCompressorThread( ImageCompressor * imC, QVariantList * pFiles, QStringList * pSuccessfulFiles ) : QThread()
{
	_pImageCompressor = imC;
	_pImageFiles = pFiles;
    _pSuccessfulFiles = pSuccessfulFiles;
}

bool ImageCompressorThread::isJpegImage(const QString& url)
{
    int location = url.lastIndexOf(".");
    if( location == - 1 )
        return false;

    QString pathExtension = url.mid(location+1).toLower();
    QStringList exts;
    exts.push_back("jpeg");
    exts.push_back("jpg");

    return exts.contains(pathExtension);
}

void ImageCompressorThread::run()
{
	int count = _pImageFiles->count();

    for( int index = 0 ; index < count ; index++ ) 
	{
        QVariantMap dictionary = _pImageFiles->at(index).toMap();
        QString oldImageFile = dictionary.value(ImageCompressorOldURL).toString();
        QString newImageFile = dictionary.value(ImageCompressorNewURL).toString();

        if( !isJpegImage(oldImageFile) )
        {
            if( QFile::copy(oldImageFile, newImageFile) )
                _pSuccessfulFiles->append(newImageFile);
            continue;
        }

        QImage * pImage = new QImage( oldImageFile );
		if( pImage->isNull() )
		{
			delete pImage;
			continue;
		}

		QImage newImage;

		//Check Image Size
		if( ( pImage->width() > _pImageCompressor->maxPixel().width() || pImage->height() > _pImageCompressor->maxPixel().height() ) && _pImageCompressor->shouldCompressImageForSize() )
		{
			QSize newImageSize = pImage->size();
			if( pImage->width() > _pImageCompressor->maxPixel().width() )
			{
				newImageSize.setWidth( _pImageCompressor->maxPixel().width() );
				newImageSize.setHeight( (float)pImage->height() / (float)pImage->width() * newImageSize.width() );
			}

			if( newImageSize.height() > _pImageCompressor->maxPixel().height() )
			{
				newImageSize.setHeight( _pImageCompressor->maxPixel().height() );
				newImageSize.setWidth( (float)pImage->width() / (float)pImage->height() * newImageSize.height() );
			}

			newImage = pImage->scaled( newImageSize, Qt::KeepAspectRatio );
		}
		else
		{
			newImage = *pImage;
		}

		//Check DPI
		QSize imageDPI( newImage.dotsPerMeterX() * 100.0f / 2.54f, newImage.dotsPerMeterY() * 100.0f / 2.54f );
		if( (imageDPI.width() > _pImageCompressor->maxDPI().width() ) && _pImageCompressor->shouldCompressImageForDPI() )
			newImage.setDotsPerMeterX( _pImageCompressor->maxDPI().width() / 2.54f * 100.0f );
		if( ( imageDPI.height() > _pImageCompressor->maxDPI().height() ) && _pImageCompressor->shouldCompressImageForDPI() )
			newImage.setDotsPerMeterY( _pImageCompressor->maxDPI().height() / 2.54f * 100.0f );

		delete pImage;

		//Save
        /*
        QFileInfo fileInfo( imageFile );
        QDir dir = fileInfo.dir();
        QString secondFileName = QString( "tmp_%1" ).arg( fileInfo.fileName() );
        QString secondFilePath = dir.filePath( secondFileName );

        if( QFile::rename( imageFile, secondFilePath ) )
        {
            if( newImage.save( imageFile ) )
            {
                qDebug() << imageFile << "saved successfully.\n";
                QFile::remove( secondFilePath );
            }
            else
            {
                qDebug() << imageFile << "failed to save.\n";
                QFile::rename( secondFilePath, imageFile );
            }
        }
        */

        if( newImage.save(newImageFile) )
            _pSuccessfulFiles->append(newImageFile);
    }	
}
