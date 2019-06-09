/*

		Added by Myongsok Kim on July 30th, 2013

		Restrict image not to overlow the maximum DPI and Pixel resolution.

*/


#ifndef IMAGECOMPRESSOR_H
#define IMAGECOMPRESSOR_H

#include <QObject>
#include <QThread>
#include <QVariantList>
#include <QSize.h>
#include <QStringList>
#include "Error.h"

#define ImageCompressorErrorDomain "ImageCompressorErrorDomain"
#define ImageCompressorOldURL "ImageCompressorOldURL"
#define ImageCompressorNewURL "ImageCompressorNewURL"

class ImageCompressor;
class ImageCompressorDelegate
{
public:
    virtual bool imageCompressorShouldBegin(ImageCompressor* imageCompressor) { Q_UNUSED(imageCompressor); return true; }
    virtual void imageCompressorDidBegin(ImageCompressor* imageCompressor) { Q_UNUSED(imageCompressor); }
    virtual void imageCompressorWasCancelled(ImageCompressor* imageCompressor) { Q_UNUSED(imageCompressor); }
    virtual void imageCompressorDidFailWithError(ImageCompressor* imageCompressor, Error* error) { Q_UNUSED(imageCompressor); Q_UNUSED(error); }
    virtual void imageCompressorDidFinish(ImageCompressor* imageCompressor) { Q_UNUSED(imageCompressor); }
};

class ImageCompressorThread;
class ImageCompressor : public QObject
{
	Q_OBJECT

public:
	explicit ImageCompressor(QObject *parent = 0);
	~ImageCompressor();

private:
    ImageCompressorDelegate*    _delegate;
    QVariantList                _totalFiles;
    QStringList                 _successfulFiles;
    QThread*					_thread;
	bool						_shouldCompressImageForDPI;
	QSize						_maxDPI;
	bool						_shouldCompressImageForSize;
	QSize						_maxPixel;

public:
    void compressImages(const QVariantList & fileArray);
    bool isRunning();
    void cancel();

    void setDelegate(ImageCompressorDelegate * d) { _delegate = d; }
    ImageCompressorDelegate * getDelegate() { return _delegate; }
    int totalFileCount() { return _totalFiles.count(); }
    QVariantList totalFiles() { return _totalFiles; }
    QStringList successfulFiles() { return _successfulFiles; }
	bool shouldCompressImageForDPI() { return _shouldCompressImageForDPI; }
	void setShouldCompressImageForDPI( bool shouldCompress ) { _shouldCompressImageForDPI = shouldCompress; }
	QSize maxDPI() { return _maxDPI; }
	void setMaxDPI( QSize max ) { _maxDPI = max; }
	bool shouldCompressImageForSize() { return _shouldCompressImageForSize; }
	void setShouldCompressImageForSize( bool shouldCompress ) { _shouldCompressImageForSize = shouldCompress; }
	QSize maxPixel() { return _maxPixel; }
	void setMaxPixel( QSize max ) { _maxPixel = max; }

private Q_SLOTS:
    void imageCompressionDidFinish();
    void imageCompressionDidTerminate();

};

class ImageCompressorThread : public QThread
{
    Q_OBJECT

public:
    explicit ImageCompressorThread( ImageCompressor * imC, QVariantList* pFiles, QStringList * pSuccessfulFiles );

private:
    virtual void run();
    bool isJpegImage(const QString& url);
private:
	ImageCompressor *	_pImageCompressor;
    QVariantList *		_pImageFiles;
    QStringList *       _pSuccessfulFiles;
};

#endif // IMAGECOMPRESSOR_H
