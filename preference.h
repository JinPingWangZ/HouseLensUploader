/*

	preference.h

	Programmed by Myongsok Kim on August 1st, 2013

	Define Preference class, which manages application settings and implement data model for QTableView

*/


#ifndef PREFERENCE_H
#define PREFERENCE_H

#include <QObject>
#include <QStringList>
#include <QDateTime>

class Preference : public QObject
{
	Q_OBJECT

public:
	Preference(QObject *parent = 0);
	~Preference();

private:
	static QString defaultFTPPassword();
	static int defaultHDVideoWidth();
	static int defaultHDVideoHeight();
	static int defaultMaxImageDPI();
	static int defaultMaxImageWidth();
	static int defaultMaxImageHeight();
	static bool defaultShouldCompressImageForDPI();
	static bool defaultShouldCompressImageForSize();
    static QStringList defaultExtensionsForVideos();
    static QStringList defaultExtensionsForPhotos();
    static QStringList defaultExtensionsForSounds();
    static QStringList defaultExtensionsForDNGs();
    static QVariantList defaultServers();

public:
	static QString ftpPassword();
	static int hdVideoWidth();
	static int hdVideoHeight();
	static int maxImageDPI();
	static int maxImageWidth();
	static int maxImageHeight();
	static bool shouldCompressImageForDPI();
	static bool shouldCompressImageForSize();
    static QStringList extensionsForVideos();
    static QStringList extensionsForPhotos();
    static QStringList extensionsForSounds();
    static QStringList extensionsForDNGs();
	static QString username();
	static QString password();
	static QDateTime lastQueryDate();
    static QString dngConverterLocation();
    static int dngPreviewIndex();
    static bool dngEmbedFastLoadData();
    static bool dngUseLossyCompression();
    static int dngLossyCompressionIndex();
    static QVariantList servers();
    static QStringList availableServers();

	static void setFTPPassword( QString & password );
	static void setHDVideoWidth( int width );
	static void setHDVideoHeight( int height );
	static void setMaxImageDPI( int dpi );
	static void setMaxImageWidth( int width );
	static void setMaxImageHeight( int height );
	static void setShouldCompressImageForDPI( bool shouldCompress );
	static void setShouldCompressImageForSize( bool shouldCompress );
	static void setUsername( QString & username );
	static void setPassword( QString & password );
	static void setLastQueryDate( QDateTime & dateTime );
    static void setDngConverterLocation( QString & location );
    static void setDNGPreviewIndex( int idx );
    static void setEmbedFastLoadData( bool b );
    static void setDNGUseLossyCompression( bool b );
    static void setDNGLossyCompressionIndex( int idx );
    static void setServers( QVariantList & servers );
};

#endif // PREFERENCE_H
