/*

	preference.cpp

	Programmed by Myongsok Kim on August 1st, 2013

	Define Preference class, which manages application settings and implement data model for QTableView

*/


#include "preference.h"
#include <QSettings>

Preference::Preference(QObject *parent) :
    QObject(parent)
{

}

Preference::~Preference()
{

}

// - Preference - Related Static Methods ------------------------------------------------------------------------------------------

QString Preference::defaultFTPPassword()
{
	return QString( "3l@nc3r" );
}

int Preference::defaultHDVideoWidth()
{
	return 1280;
}

int Preference::defaultHDVideoHeight()
{
	return 720;
}

int Preference::defaultMaxImageDPI()
{
	return 300;
}

int Preference::defaultMaxImageWidth()
{
    return 5400;//3456;
}

int Preference::defaultMaxImageHeight()
{
    return 3600;//2304;
}

bool Preference::defaultShouldCompressImageForDPI()
{
	return true;
}

bool Preference::defaultShouldCompressImageForSize()
{
	return true;
}

QStringList Preference::defaultExtensionsForVideos()
{
    QStringList exts;
    exts << "mov" << "avi" << "m4v" << "mp4";
    return exts;
}

QStringList Preference::defaultExtensionsForPhotos()
{
	QStringList exts;
    exts << "jpeg" << "jpg" << "tiff" << "png";
    /*
	exts << "3fr" << "ari" << "arw" << "bay" << "crw" << "cr2" << "cap" << "dcs" << "dcr";
	exts << "dng" << "drf" << "eip" << "erf" << "fff" << "rrq" << "iiq" << "k25" << "kdc";
	exts << "mdc" << "mef" << "mos" << "mrw" << "nef" << "nrw" << "obm" << "orf" << "pef";
	exts << "ptx" << "pxn" << "r3d" << "raf" << "raw" << "rwl" << "rw2" << "rwz" << "sr2";
	exts << "srf" << "srw" << "x3f";
    */
	return exts;
}

QStringList Preference::defaultExtensionsForSounds()
{
    QStringList exts;
    exts << "mp3" << "wav" << "aiff";
    return exts;
}

QStringList Preference::defaultExtensionsForDNGs()
{
    QStringList exts;
    exts << "cr2" << "dng" << "nef" << "arw" << "rw2";
    return exts;
}

QString Preference::ftpPassword()
{
	QSettings preference;
	QString password = preference.value( "FTPPassword" ).toString();
	if( password.isEmpty() )
		return Preference::defaultFTPPassword();
	return password;
}

int Preference::hdVideoWidth()
{
	QSettings preference;
	QVariant width = preference.value( "HDM4VWidth" );
	if( width.isNull() )
		return Preference::defaultHDVideoWidth();
	return width.toInt();
}

int Preference::hdVideoHeight()
{
	QSettings preference;
	QVariant height = preference.value( "HDM4VHeight" );
	if( height.isNull() )
		return Preference::defaultHDVideoHeight();
	return height.toInt();
}

int Preference::maxImageDPI()
{
	QSettings preference;
	QVariant maxDPI = preference.value( "MaxImageDPI" );
	if( maxDPI.isNull() )
		return Preference::defaultMaxImageDPI();
	return maxDPI.toInt();
}

int Preference::maxImageWidth()
{
	QSettings preference;
	QVariant imageWidth = preference.value( "MaxImageWidth" );
	if( imageWidth.isNull() )
		return Preference::defaultMaxImageWidth();
	return imageWidth.toInt();
}

int Preference::maxImageHeight()
{
	QSettings preference;
	QVariant imageHeight = preference.value( "MaxImageHeight" );
	if( imageHeight.isNull() )
		return Preference::defaultMaxImageHeight();
	return imageHeight.toInt();
}

bool Preference::shouldCompressImageForDPI()
{
	QSettings preference;
	QVariant shouldCompressImage = preference.value( "ShouldCompressImageForDPI" );
	if( shouldCompressImage.isNull() )
		return Preference::defaultShouldCompressImageForDPI();
	return shouldCompressImage.toBool();
}

bool Preference::shouldCompressImageForSize()
{
	QSettings preference;
	QVariant shouldCompressImage = preference.value( "ShouldCompressImageForSize" );
	if( shouldCompressImage.isNull() )
		return Preference::defaultShouldCompressImageForSize();
	return shouldCompressImage.toBool();
}

QStringList Preference::extensionsForVideos()
{
    return Preference::defaultExtensionsForVideos();
}

QStringList Preference::extensionsForPhotos()
{
    return Preference::defaultExtensionsForPhotos();
}

QStringList Preference::extensionsForSounds()
{
    return Preference::defaultExtensionsForSounds();
}

QString Preference::dngConverterLocation()
{
    QSettings preference;
    QVariant location = preference.value( "DNGConverterLocation" );
    if( location.isNull() || !location.isValid() )
        return tr("");
    return location.toString();
}

QStringList Preference::extensionsForDNGs()
{
    QSettings preference;
    QVariant customExtensionsForPhotos = preference.value( "ExtensionForPhotos" );
    QStringList exts = Preference::defaultExtensionsForDNGs();
    if( customExtensionsForPhotos.isNull() )
        return exts;
    exts.append(customExtensionsForPhotos.toStringList());
    return exts;
}

QVariantList Preference::defaultServers()
{
    QVariantList defaultServers;
    QVariantMap server1;
    server1["enabled"] = true;
    server1["host"] = "houselenscloud.dyndns.tv123";
    server1["description"] = "Primary";
    defaultServers.push_back(server1);

    QVariantMap server2;
    server2["enabled"] = false;
    server2["host"] = "houselenshqmirror.dyndns.tv";
    server2["description"] = "Backup";
    defaultServers.push_back(server2);

    QVariantMap server3;
    server3["enabled"] = false;
    server3["host"] = "houselensnas1.dyndns.tv";
    server3["description"] = "Backup";
    defaultServers.push_back(server3);

    QVariantMap server4;
    server4["enabled"] = false;
    server4["host"] = "houselensnas2.dyndns.tv";
    server4["description"] = "Backup";
    defaultServers.push_back(server4);

    QVariantMap server5;
    server5["enabled"] = false;
    server5["host"] = "videolens.dyndns.tv";
    server5["description"] = "Backup";
    defaultServers.push_back(server5);

    return defaultServers;
}

QString Preference::username()
{
	QSettings preference;
	return preference.value( "Username" ).toString();
}

QString Preference::password()
{
	QSettings preference;
	return preference.value( "Password" ).toString();
}

QDateTime Preference::lastQueryDate()
{
	QSettings preference;
	return preference.value( "LastQueryDate" ).toDateTime();
}

int Preference::dngPreviewIndex()
{
    QSettings preference;
    QVariant height = preference.value( "DNGPreviewIndex", 0 );
    return height.toInt();
}

bool Preference::dngEmbedFastLoadData()
{
    QSettings preference;
    QVariant height = preference.value( "DNGEmbedFastLoadData", false );
    return height.toBool();
}

bool Preference::dngUseLossyCompression()
{
    QSettings preference;
    QVariant height = preference.value( "DNGUseLossyCompression", false );
    return height.toBool();
}

int Preference::dngLossyCompressionIndex()
{
    QSettings preference;
    QVariant height = preference.value( "DNGLossyCompressionIndex", 0 );
    return height.toInt();
}

QVariantList Preference::servers()
{
    QSettings preference;
    QVariant serverInfo = preference.value( "servers" );
//    if( serverInfo.isNull() || !serverInfo.isValid() )
//        return Preference::defaultServers();
//    QVariant serverInfo;
    return serverInfo.toList();
}

QStringList Preference::availableServers()
{
    QStringList servers;
    QVariantList serverInfos = Preference::servers();
    for( int i = 0 ; i < serverInfos.count() ; i++ )
    {
        QVariantMap serverInfo = serverInfos[i].toMap();
        if( serverInfo["enabled"].toBool() )
        {
            QString host = serverInfo["host"].toString();
            if( !host.isEmpty() )
                servers.append(host);
        }
    }
    return servers;
}

void Preference::setFTPPassword( QString & password )
{
	QSettings preference;
	preference.setValue( "FTPPassword", password );
}

void Preference::setHDVideoWidth( int width )
{
	QSettings preference;
	if( width > 0 )
		preference.setValue( "HDM4VWidth", width );
	else
		preference.remove( "HDM4VWidth" );
}

void Preference::setHDVideoHeight( int height )
{
	QSettings preference;
	if( height > 0 )
		preference.setValue( "HDM4VHeight", height );
	else
		preference.remove( "HDM4VHeight" );
}

void Preference::setMaxImageDPI( int dpi )
{
	QSettings preference;
	if( dpi > 0 )
		preference.setValue( "MaxImageDPI", dpi );
	else
		preference.remove( "MaxImageDPI" );
}

void Preference::setMaxImageWidth( int width )
{
	QSettings preference;
	if( width > 0 )
		preference.setValue( "MaxImageWidth", width );
	else
		preference.remove( "MaxImageWidth" );
}

void Preference::setMaxImageHeight( int height )
{
	QSettings preference;
	if( height > 0 )
		preference.setValue( "MaxImageHeight", height );
	else
		preference.remove( "MaxImageHeight" );
}

void Preference::setShouldCompressImageForDPI( bool shouldCompress )
{
	QSettings preference;
	preference.setValue( "ShouldCompressImageForDPI", shouldCompress );
}

void Preference::setShouldCompressImageForSize( bool shouldCompress )
{
	QSettings preference;
	preference.setValue( "ShouldCompressImageForSize", shouldCompress );
}

void Preference::setUsername( QString & username )
{
	QSettings preference;
	preference.setValue( "Username", username );
}

void Preference::setPassword( QString & password )
{
	QSettings preference;
	preference.setValue( "Password", password );
}

void Preference::setLastQueryDate( QDateTime & dateTime )
{
	QSettings preference;
	preference.setValue( "LastQueryDate", dateTime );
}

void Preference::setDngConverterLocation( QString & location )
{
    QSettings preference;
    preference.setValue( "DNGConverterLocation", location );
}

void Preference::setDNGPreviewIndex( int idx )
{
    QSettings preference;
    preference.setValue( "DNGPreviewIndex", idx );
}

void Preference::setEmbedFastLoadData( bool b )
{
    QSettings preference;
    preference.setValue( "DNGEmbedFastLoadData", b );
}

void Preference::setDNGUseLossyCompression( bool b )
{
    QSettings preference;
    preference.setValue( "DNGUseLossyCompression", b );
}

void Preference::setDNGLossyCompressionIndex( int idx )
{
    QSettings preference;
    preference.setValue( "DNGLossyCompressionIndex", idx );
}

void Preference::setServers(QVariantList & servers )
{
    QSettings preference;
    preference.setValue( "servers", servers );
}
