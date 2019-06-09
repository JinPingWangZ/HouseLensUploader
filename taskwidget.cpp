#include "taskwidget.h"
#include "ui_taskwidget.h"
#include <QPaintEvent>
#include <QPainter>
#include "shoottitledatasource.h"
#include <QFileInfo>
#include <QDir>
#include "error.h"
#include <QDebug>
#include "preference.h"

#define	AUTOEDIT_FILENAME                   "autoedit.txt"

#define PROCESSING_LEVEL_FRESH              0
#define PROCESSING_LEVEL_MOVIE              1
#define PROCESSING_LEVEL_PHOTO              2
#define PROCESSING_LEVEL_SOUND              3
#define PROCESSING_LEVEL_DNG                4

TaskWidget::TaskWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskWidget)
{
    ui->setupUi(this);
    ui->autoEditing->setVisible(false);
    ui->autoEditingLabel->setVisible(false);
    this->setState(TaskWidget::READY);
    _delegate = NULL;

    _shouldRun = false;
    _running = false;

    _fileCopier.setDelegate(this);
    _movieConverter.setDelegate(this);
    _uploaderManager.setDelegate(this);
    _uploader.setDelegate(this);//No Need
    _zipper.setDelegate(this);
    _webServiceNotifier.setDelegate(this);
	_imageCompressor.setDelegate(this);
    _dngConverter.setDelegate(this);

    pMyTempoaryDir = NULL;
}

TaskWidget::~TaskWidget()
{
	_fileCopier.setDelegate( NULL );
	_movieConverter.setDelegate( NULL );
	_uploader.setDelegate( NULL );
    _uploaderManager.setDelegate( NULL );
	_zipper.setDelegate( NULL );
	_webServiceNotifier.setDelegate( NULL );
	_imageCompressor.setDelegate( NULL );
    _dngConverter.setDelegate( NULL );

    if( _movieConverter.isRunning() )
        _movieConverter.cancel();

    if( _fileCopier.isRunning() )
        _fileCopier.cancel();

    if( _uploader.isRunning() )
        _uploader.cancel();

    if( _uploaderManager.isRunning() )
        _uploaderManager.cancel();

    if( _zipper.isRunning() )
        _zipper.cancel();

    if( _webServiceNotifier.isRunning() )
        _webServiceNotifier.cancel();

	if( _imageCompressor.isRunning() )
		_imageCompressor.cancel();

    if( _dngConverter.isRunning() )
        _dngConverter.cancel();

     if( pMyTempoaryDir != NULL )
         delete pMyTempoaryDir;

    delete ui;
}

void TaskWidget::setState(TaskWidget::TaskState state)
{
    switch( state )
    {
        case TaskWidget::RUNNING:
        {
            this->setStyleSheet("TaskWidget {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(138, 202, 131, 202), stop: 1 rgba(60, 209, 63, 202));border-radius:6px;}");
        }
        break;

        case TaskWidget::DONE:
        {
            this->setStyleSheet("TaskWidget {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(125, 162, 202, 202), stop: 1 rgba(68, 131, 209, 202));border-radius:6px;}");
        }
        break;

        case TaskWidget::FAIL:
        {
            this->setStyleSheet("TaskWidget {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(201, 129, 117, 202), stop: 1 rgba(221, 74, 64, 202));border-radius:6px;}");
        }
        break;

        default:
        {   // TASKSTATE_READY
            this->setStyleSheet("TaskWidget {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgba(242, 242, 242, 202), stop: 1 rgba(166, 166, 166, 202));border-radius:6px;}");
        }
    }
}

void TaskWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}




// - Events ------------------------------------------------------------------------------------------------------------------

void TaskWidget::on_removeButton_clicked()
{
    if( _delegate != NULL )
        _delegate->taskWidgetRemove(this);
}

void TaskWidget::on_editButton_clicked()
{
    if( _delegate != NULL )
        _delegate->taskWidgetEdit(this);
}

void TaskWidget::on_stopButton_clicked()
{
    this->start_stop();
}



// - Getter / Setter -----------------------------------------------------------------------------------------------------------

void TaskWidget::setShootTitleId(QString& titleId)
{
    _shootTitleId = titleId;
    QString shootTitle = _shootTitleDataSource->shootTitleForId(titleId);
    this->ui->shootTitleLabel->setText(shootTitle);
}

QString TaskWidget::shootTitleId()
{
    return _shootTitleId;
}

QString TaskWidget::shootTitle()
{
    return _shootTitleDataSource->shootTitleForId(_shootTitleId);
}

void TaskWidget::setShootTitleDataSource(ShootTitleDataSource* value)
{
    _shootTitleDataSource = value;
}

ShootTitleDataSource* TaskWidget::shootTitleDataSource()
{
    return _shootTitleDataSource;
}

bool TaskWidget::autoEditing()
{
    return _autoEditing;
}

void TaskWidget::setAutoEditing(bool value)
{
    _autoEditing = value;
    this->ui->autoEditing->setText(value?"YES":"NO");
}

void TaskWidget::setShouldRun(bool flag)
{
    _shouldRun = flag;

    if( !flag ) {
        this->ui->stopButton->setText("Start");
    }
    else {
        this->ui->stopButton->setText("Stop");
    }

    this->ui->stopButton->setEnabled(_shouldRun==_running);
}

bool TaskWidget::shouldRun()
{
    return _shouldRun;
}

void TaskWidget::setRunning(bool flag)
{
    _running = flag;
    this->ui->stopButton->setEnabled(_shouldRun==_running);
    this->ui->editButton->setEnabled(!_running);
    this->ui->removeButton->setEnabled(!_running);
    if( _running )
        this->ui->progressIndicator->startAnimation();
    else
        this->ui->progressIndicator->stopAnimation();
}

bool TaskWidget::running()
{
    return _running;
}

void TaskWidget::setOption(SUBMIT_OPTION option)
{
    _option = option;
    this->ui->optionLabel->setText(stringForSubmitOption(option));
}

SUBMIT_OPTION TaskWidget::option()
{
    return _option;
}

void TaskWidget::setFolderURL(QString& url)
{
    _folderURL = url;
    this->ui->pathLabel->setText(url);
}

QString TaskWidget::folderURL()
{
    return _folderURL;
}

void TaskWidget::setClips(QVariantList& list)
{
    _clips = list;
}

QVariantList TaskWidget::clips()
{
    return _clips;
}

bool TaskWidget::mailingCheck()
{
	return _mailingCheck;
}

void TaskWidget::setMailingCheck( bool flag )
{
	_mailingCheck = flag;
}

QString TaskWidget::noteToEditor()
{
	return _noteToEditor;
}

void TaskWidget::setNoteToEditor( QString & note )
{
	_noteToEditor = note;
}

// - Public Methods -----------------------------------------------------------------------------------------------

bool TaskWidget::isRunningState()
{
    return (_running&&_running==_shouldRun);
}

bool TaskWidget::isReadyState()
{
    return (!_running&&_running==_shouldRun);
}

QString TaskWidget::zipName()
{
    return QString("M4V_Files.zip");
}

void TaskWidget::start_stop()
{
    this->setShouldRun(!this->shouldRun());

    if( this->shouldRun() ) 
	{
        this->setRunning(true);
        if( _delegate != NULL )
            _delegate->taskWidgetDidBegin(this);
        this->setState(RUNNING);

        /*
        if( hasConvertOption(_option) ) 
		{
            QSize videoSize(480, 270);

			if( isHDM4VConvert(_option) && _delegate != NULL )
                videoSize = _delegate->hdm4vSize();
            this->convertMovieWithSize(videoSize);
        }
        else 
		{
            this->orderMedias();
        }
        */


        _processingLevel = PROCESSING_LEVEL_FRESH;

        myUploadingFiles.clear();

        if( pMyTempoaryDir != NULL )
            delete pMyTempoaryDir;
        pMyTempoaryDir = new QTemporaryDir;

        this->processMedias();
    }
    else 
	{
        this->setState(READY);

        if( _delegate != NULL )
            _delegate->taskWidgetWillBeCancelled(this);

        if( _movieConverter.isRunning() )
            _movieConverter.cancel();

		if( _uploader.isRunning() )
            _uploader.cancel();

        if( _uploaderManager.isRunning() )
            _uploaderManager.cancel();

        if( _fileCopier.isRunning() )
            _fileCopier.cancel();

        if( _zipper.isRunning() )
            _zipper.cancel();

        if( _webServiceNotifier.isRunning() )
            _webServiceNotifier.cancel();
	
		if( _imageCompressor.isRunning() )
			_imageCompressor.cancel();

        if( _dngConverter.isRunning() )
            _dngConverter.cancel();
    }
}



// - Private Methods ----------------------------------------------------------------------
void TaskWidget::processMedias()
{
    if( _processingLevel < PROCESSING_LEVEL_MOVIE )
    {
        //Process Movies
        _processingLevel = PROCESSING_LEVEL_MOVIE;

        if( hasMovies() )
        {
            if( hasConvertOption(_option) )
                convertMovies();
            else
                orderMovies();
            return;
        }
    }

    if( _processingLevel < PROCESSING_LEVEL_PHOTO )
    {
        //Process Photos
        _processingLevel = PROCESSING_LEVEL_PHOTO;

        if( hasImages() )
        {
            if( hasConvertOption(_option) )
                compressImages();
            else
                orderImages();
            return;
        }
    }

    if( _processingLevel < PROCESSING_LEVEL_SOUND )
    {
        //Process Sounds
        _processingLevel = PROCESSING_LEVEL_SOUND;

        if( hasSounds() )
        {
            orderSounds();
            return;
        }
    }

    if( _processingLevel < PROCESSING_LEVEL_DNG )
    {
        //Process DNG
        _processingLevel = PROCESSING_LEVEL_DNG;

        if( hasDNGs() )
        {
            if( hasConvertDNGOption(_option) && DNGConverter::isAvailable() )
                convertDNGs();
            else
                orderDNGs();
            return;
        }
    }

    //AutoEditing
    /*
    if( hasUploadOption(_option) && this->autoEditing() )
    {
        this->createAutoeditTextFileAtDirectory();
        myUploadingFiles.push_back(this->autoeditTextFileURL());
    }
    else
    {
        this->deleteAutoeditTextFileAtDirectory();
    }
    */

    //Upload
    if( hasUploadOption(_option) )
    {
        //qSort(myUploadingFiles.begin(), myUploadingFiles.end());
        int numOfPics = this->numberOfPicsFromFiles( myUploadingFiles );
        int numOfVideos = this->numberOfVideosFromFiles( myUploadingFiles );
        _webServiceNotifier.notifyBeforeUploading( this->shootTitleDataSource()->userTokenId(), this->shootTitleId(), numOfPics, numOfVideos );
    }
}

bool TaskWidget::isMovie(const QString& url)
{
    int location = url.lastIndexOf(".");
    if( location == - 1 )
        return false;

    QString pathExtension = url.mid(location+1).toLower();
    QStringList movieExtensions = Preference::extensionsForVideos();
    //movieExtensions.push_back("mov");
    //movieExtensions.push_back("m4v");
    //movieExtensions.push_back("avi");
    return movieExtensions.contains(pathExtension);
}

bool TaskWidget::isImage(const QString& url)
{
    int location = url.lastIndexOf(".");
    if( location == - 1 )
        return false;

    QString pathExtension = url.mid(location+1).toLower();
    QStringList exts = Preference::extensionsForPhotos();
    //exts.push_back("jpeg");
    //exts.push_back("jpg");

    return exts.contains(pathExtension);
}

bool TaskWidget::isSound(const QString &url)
{
    int location = url.lastIndexOf(".");
    if( location == - 1 )
        return false;

    QString pathExtension = url.mid(location+1).toLower();
    QStringList exts = Preference::extensionsForSounds();

    return exts.contains(pathExtension);
}

bool TaskWidget::isDNG(const QString &url)
{
    int location = url.lastIndexOf(".");
    if( location == - 1 )
        return false;

    QString pathExtension = url.mid(location+1).toLower();
    QStringList exts = Preference::extensionsForDNGs();

    return exts.contains(pathExtension);
}

bool TaskWidget::hasMovies()
{
    int count = _clips.count();

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalURL = mediaDict.value(AppKey_URL).toString();

        if( this->isMovie(originalURL) )
            return true;
    }

    return false;
}

bool TaskWidget::hasImages()
{
    int count = _clips.count();

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalURL = mediaDict.value(AppKey_URL).toString();

        if( this->isImage(originalURL) )
            return true;
    }

    return false;
}

bool TaskWidget::hasSounds()
{
    int count = _clips.count();

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalURL = mediaDict.value(AppKey_URL).toString();

        if( this->isSound(originalURL) )
            return true;
    }

    return false;
}

bool TaskWidget::hasDNGs()
{
    int count = _clips.count();

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalURL = mediaDict.value(AppKey_URL).toString();

        if( this->isDNG(originalURL) )
            return true;
    }

    return false;
}

void TaskWidget::orderMedias()
{
    QVariantList copyFileArray;
    int count = _clips.count();
    int imageIndex = 1;
    int movieIndex = 1;
    int soundIndex = 1;
    int dngIndex = 1;

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalFilePath = mediaDict[AppKey_URL].toString();
        QFileInfo fileInfo(originalFilePath);
        QString newDirPath = pMyTempoaryDir->path();//fileInfo.absoluteDir().absolutePath();
        QString newFileName;
        if( this->isMovie(originalFilePath) )
            //Movie
            newFileName = QString("video_%1.%2").arg(movieIndex++, 4, 10, QChar('0')).arg(fileInfo.suffix());
        else if( this->isImage(originalFilePath))
            //Image
            newFileName = QString("image_%1.%2").arg(imageIndex++, 4, 10, QChar('0')).arg(fileInfo.suffix());
        else if( this->isSound(originalFilePath))
            //Sound
            newFileName = QString("sound_%1.%2").arg(soundIndex++, 4, 10, QChar('0')).arg(fileInfo.suffix());
        else if( this->isDNG(originalFilePath) )
            //DNG
            newFileName = QString("raw_%1.%2").arg(dngIndex++, 4, 10, QChar('0')).arg(fileInfo.suffix());
        else
            continue;

        QString newFilePath = newDirPath + "/" + newFileName;
        QVariantMap myDict;
        myDict[FileCopierOldURL] = originalFilePath;
        myDict[FileCopierNewURL] = newFilePath;
        copyFileArray.push_back(myDict);
    }
    _fileCopier.copyFiles(copyFileArray);
}

void TaskWidget::orderMovies()
{
    QVariantList copyFileArray;
    int count = _clips.count();
    int movieIndex = 1;

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalFilePath = mediaDict[AppKey_URL].toString();

        if( !this->isMovie(originalFilePath) )
            continue;

        QFileInfo fileInfo(originalFilePath);
        QString newDirPath = pMyTempoaryDir->path();//fileInfo.absoluteDir().absolutePath();
        QString newFileName = QString("video_%1.%2").arg(movieIndex++, 4, 10, QChar('0')).arg(fileInfo.suffix());
        QString newFilePath = newDirPath + "/" + newFileName;
        QVariantMap myDict;
        myDict[FileCopierOldURL] = originalFilePath;
        myDict[FileCopierNewURL] = newFilePath;
        copyFileArray.push_back(myDict);
    }

    _fileCopier.copyFiles(copyFileArray);
}

void TaskWidget::orderImages()
{
    QVariantList copyFileArray;
    int count = _clips.count();
    int imageIndex = 1;

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalFilePath = mediaDict[AppKey_URL].toString();

        if( !this->isImage(originalFilePath) )
            continue;

        QFileInfo fileInfo(originalFilePath);
        QString newDirPath = pMyTempoaryDir->path();//fileInfo.absoluteDir().absolutePath();
        QString newFileName = QString("image_%1.%2").arg(imageIndex++, 4, 10, QChar('0')).arg(fileInfo.suffix());
        QString newFilePath = newDirPath + "/" + newFileName;
        QVariantMap myDict;
        myDict[FileCopierOldURL] = originalFilePath;
        myDict[FileCopierNewURL] = newFilePath;
        copyFileArray.push_back(myDict);
    }

    _fileCopier.copyFiles(copyFileArray);
}

void TaskWidget::orderSounds()
{
    QVariantList copyFileArray;
    int count = _clips.count();
    int soundIndex = 1;

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalFilePath = mediaDict[AppKey_URL].toString();

        if( !this->isSound(originalFilePath) )
            continue;

        QFileInfo fileInfo(originalFilePath);
        QString newDirPath = pMyTempoaryDir->path();//fileInfo.absoluteDir().absolutePath();
        QString newFileName = QString("sound_%1.%2").arg(soundIndex++, 4, 10, QChar('0')).arg(fileInfo.suffix());
        QString newFilePath = newDirPath + "/" + newFileName;
        QVariantMap myDict;
        myDict[FileCopierOldURL] = originalFilePath;
        myDict[FileCopierNewURL] = newFilePath;
        copyFileArray.push_back(myDict);
    }

    _fileCopier.copyFiles(copyFileArray);
}

void TaskWidget::orderDNGs()
{
    QVariantList copyFileArray;
    int count = _clips.count();
    int dngIndex = 1;

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalFilePath = mediaDict[AppKey_URL].toString();

        if( !this->isDNG(originalFilePath) )
            continue;

        QFileInfo fileInfo(originalFilePath);
        QString newDirPath = pMyTempoaryDir->path();//fileInfo.absoluteDir().absolutePath();
        QString newFileName = QString("raw_%1.%2").arg(dngIndex++, 4, 10, QChar('0')).arg(fileInfo.suffix());
        QString newFilePath = newDirPath + "/" + newFileName;
        QVariantMap myDict;
        myDict[FileCopierOldURL] = originalFilePath;
        myDict[FileCopierNewURL] = newFilePath;
        copyFileArray.push_back(myDict);
    }

    _fileCopier.copyFiles(copyFileArray);
}

void TaskWidget::convertMovies()
{
    QSize videoSize(480, 270);
    if( isHDM4VConvert(_option) && _delegate != NULL )
        videoSize = _delegate->hdm4vSize();
    _movieConverter.setVideoSize(videoSize);

    QVariantList mediaInfoArray;
    int count = _clips.count();
    int movieIndex = 1;

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalURL = mediaDict[AppKey_URL].toString();

        if( !this->isMovie(originalURL) )
            continue;

        bool removeAudio = mediaDict.value(AppKey_RemoveAudio).toBool();
        QString newMovieName = QString("video_%1.m4v").arg(movieIndex++, 4, 10, QChar('0'));
        //QFileInfo fileInfo(originalURL);
        //QDir dir = fileInfo.absoluteDir();
        QString newMovieURL = pMyTempoaryDir->path() + "/" + newMovieName;//dir.absoluteFilePath(newMovieName);
        QVariantMap myDict;
        myDict[MCOldMovieURL] = originalURL;
        myDict[MCNewMovieURL] = newMovieURL;
        myDict[MCRemoveAudio] = removeAudio;
        mediaInfoArray.push_back(myDict);
    }

    _movieConverter.convertMovies(mediaInfoArray);
}

void TaskWidget::compressImages()
{
    QVariantList mediaInfoArray;
    int count = _clips.count();
    int imageIndex = 1;

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalFilePath = mediaDict[AppKey_URL].toString();

        if( !this->isImage(originalFilePath) )
            continue;

        QFileInfo fileInfo(originalFilePath);
        QString newDirPath = pMyTempoaryDir->path();//fileInfo.absoluteDir().absolutePath();
        QString newFileName = QString("image_%1.%2").arg(imageIndex++, 4, 10, QChar('0')).arg(fileInfo.suffix());
        QString newFilePath = newDirPath + "/" + newFileName;
        QVariantMap myDict;
        myDict[ImageCompressorOldURL] = originalFilePath;
        myDict[ImageCompressorNewURL] = newFilePath;
        mediaInfoArray.push_back(myDict);
    }

    _imageCompressor.setShouldCompressImageForDPI( _delegate->shouldCompressImageForDPI() );
    _imageCompressor.setMaxDPI( QSize( _delegate->maxDPI(), _delegate->maxDPI() ) );
    _imageCompressor.setShouldCompressImageForSize( _delegate->shouldCompressImageForSize() );
    _imageCompressor.setMaxPixel( _delegate->maxImageSize() );
    _imageCompressor.compressImages(mediaInfoArray);
}

void TaskWidget::convertDNGs()
{
    QVariantList mediaInfoArray;
    int count = _clips.count();
    int dngIndex = 1;

    for( int index = 0; index < count; ++index )
    {
        QVariantMap mediaDict = _clips[index].toMap();
        QString originalURL = mediaDict[AppKey_URL].toString();

        if( !this->isDNG(originalURL) )
            continue;

        QString newDNGName = QString("dng_%1.dng").arg(dngIndex++, 4, 10, QChar('0'));
        //QFileInfo fileInfo(originalURL);
        //QDir dir = fileInfo.absoluteDir();
        QString newDNGURL = pMyTempoaryDir->path() + "/" + newDNGName;//dir.absoluteFilePath(newDNGName);
        QVariantMap myDict;
        myDict[DCRAWURL] = originalURL;
        myDict[DCDNGURL] = newDNGURL;
        myDict[DCSUCESS] = false;
        mediaInfoArray.push_back(myDict);
    }

    _dngConverter.convertRawFiles(mediaInfoArray);
}

void TaskWidget::createAutoeditTextFileAtDirectory()
{
    QDir folder(_folderURL);
    QString fullPath = folder.absoluteFilePath(AUTOEDIT_FILENAME);

    QFile newFile(fullPath);
    if( newFile.exists() )
        return;

    newFile.open(QIODevice::WriteOnly);
    newFile.setPermissions((QFile::Permission)0x7777);
}

QString TaskWidget::autoeditTextFileURL()
{
    QDir folder(_folderURL);
    return folder.absoluteFilePath(AUTOEDIT_FILENAME);
}

void TaskWidget::deleteAutoeditTextFileAtDirectory()
{
    QDir folder(_folderURL);
    QString fullPath = folder.absoluteFilePath(AUTOEDIT_FILENAME);

    QFile editingFile(fullPath);
    if( !editingFile.exists() )
        return;

    editingFile.remove();
}

QString TaskWidget::ftpDirectory()
{
    QString ftpBaseDirectory = _delegate->ftpBaseDirectory();

    // remove frontmost backslash.
    if( ftpBaseDirectory.length() > 0 && ftpBaseDirectory.left(1) != QString("/") ) {
        ftpBaseDirectory = QString("/") + ftpBaseDirectory;
    }

    // remove endmost backslash.
    if( ftpBaseDirectory.length() > 0 && ftpBaseDirectory.right(1) == QString("/") ) {
        if( ftpBaseDirectory.length() > 1 )
            ftpBaseDirectory = ftpBaseDirectory.left(ftpBaseDirectory.length()-1);
        else
            ftpBaseDirectory = "";
    }

    QString ftpSubDirectory = this->ui->shootTitleLabel->text();

    if( ftpSubDirectory.length() > 0 && ftpSubDirectory.left(1) == QString("/") ) {
        if( ftpSubDirectory.length() > 1 )
            ftpSubDirectory = ftpSubDirectory.mid(1);
        else
            ftpSubDirectory = "";
    }

    // remove endmost backslash.
    if( ftpSubDirectory.length() > 0 &&  ftpSubDirectory.right(1) == QString("/") ) {
        if( ftpSubDirectory.length() > 1 )
            ftpSubDirectory = ftpSubDirectory.left(ftpSubDirectory.length()-1);
        else
            ftpSubDirectory = "";
    }

    ftpSubDirectory = ftpSubDirectory + QString(" - %1").arg(_delegate->artistName());
    QString fullPath = ftpBaseDirectory + QString("/") + ftpSubDirectory;
    qDebug() << "fullPath = " << fullPath << "\n";
    return fullPath;
}

//Added by Myongsok Kim on July 2013
int TaskWidget::numberOfPicsFromFiles( QStringList & files )
{
	int n = 0;
	for( int i = 0 ; i < files.count() ; i++ )
	{
		QString aFile = files.at(i);

        if( this->isImage( aFile ) || this->isDNG( aFile ) )
			n++;
	}

	return n;
}

int TaskWidget::numberOfVideosFromFiles( QStringList & files )
{
	int n = 0;
	for( int i = 0 ; i < files.count() ; i++ )
	{
		QString aFile = files.at(i);
	
		if( this->isMovie( aFile ) )
			n++;
	}

	return n;
}



// - Movie Converter Delegate Methods --------------------------------------------------------------------------------------

bool TaskWidget::movieConverterShouldBeginConverting(MovieConverter* converter)
{
    Q_UNUSED(converter);
    return true;
}

void TaskWidget::movieConverterDidBeginConverting(MovieConverter* converter)
{
    Q_UNUSED(converter);
    this->ui->progressLabel->setText("Converting ...");
}

void TaskWidget::movieConverterWasCancelled(MovieConverter* converter)
{
    Q_UNUSED(converter);
    this->ui->progressLabel->setText("Converting cancelled");
    this->setRunning(false);

    if( _delegate != NULL )
        _delegate->taskWidgetWasCancelled(this);
}

void TaskWidget::movieConverterDidProgressToPercent(MovieConverter* converter, float percent)
{
    Q_UNUSED(converter);
    Q_UNUSED(percent);
    QVariantMap aMovieInfo = _movieConverter.currentMovieInfo();
    QFileInfo fileInfo(aMovieInfo.value(MCOldMovieURL).toString());
    QString currentMovieName = fileInfo.fileName();

    this->ui->progressLabel->setText(QString("Converting %1 %2% (total:%3 current:%4 success:%5 fail:%6)").arg(currentMovieName).arg(_movieConverter.currentMovieProgress()).arg(_movieConverter.totalMovieCount()).arg(_movieConverter.currentMovieIndex()+1).arg(_movieConverter.successedMovieCount()).arg(_movieConverter.failedMovieCount()));
}

void TaskWidget::movieConverterDidFailWithError(MovieConverter* converter, Error* error)
{
    Q_UNUSED(converter);
    this->ui->progressLabel->setText(error->localizedDescription());
    this->setRunning(false);
    this->setShouldRun(false);

    this->setState(TaskWidget::FAIL);

    if( _delegate != NULL )
        _delegate->taskWidgetDidFail(this, error);
}

void TaskWidget::movieConverterDidFinishConvert(MovieConverter* converter)
{
    Q_UNUSED(converter);
    this->ui->progressLabel->setText(QString("Converting complete (total:%1  success:%2  fail:%3)").arg(_movieConverter.totalMovieCount()).arg(_movieConverter.successedMovieCount()).arg(_movieConverter.failedMovieCount()));

    if( hasZipOption(_option) )
    {
        QVariantList successMovieInfos = _movieConverter.successedMovieInfos();
        QVariantMap aMovieInfo = successMovieInfos[0].toMap();
        QString fileURL = aMovieInfo.value(MCNewMovieURL).toString();
        QFileInfo fileInfo(fileURL);
        QString folder = fileInfo.dir().absolutePath();

        QStringList fileNames;
        QVariantList movieInfos = _movieConverter.successedMovieInfos();

        Q_FOREACH( QVariant aMovieInfo, movieInfos ) 
		{
            QFileInfo fileInfo(aMovieInfo.toMap().value(MCNewMovieURL).toString());
            fileNames.push_back(fileInfo.fileName());
        }

        _zipper.zipFiles(fileNames, folder, this->zipName());

        return;
    }

    if( hasUploadOption(_option) )
    {
        QVariantList successMovieInfos = _movieConverter.successedMovieInfos();
        Q_FOREACH( QVariant aMovieInfo, successMovieInfos )
        {
            myUploadingFiles.push_back(aMovieInfo.toMap().value(MCNewMovieURL).toString());
        }
    }

    this->processMedias();
}

// - File Copier Delegate Methods -----------------------------------------------------------------------------------

bool TaskWidget::fileCopierShouldBeginCopy(FileCopier* fileCopier)
{
    Q_UNUSED(fileCopier);
    return true;
}

void TaskWidget::fileCopierDidBeginCopy(FileCopier* fileCopier)
{
    Q_UNUSED(fileCopier);
    ui->progressLabel->setText("Ordering ...");
}

void TaskWidget::fileCopierWasCancelled(FileCopier* fileCopier)
{
    Q_UNUSED(fileCopier);
    this->setRunning(false);
    ui->progressLabel->setText("Ordering cancelled");

    if( _delegate != NULL ) {
        _delegate->taskWidgetWasCancelled(this);
    }
}

void TaskWidget::fileCopierDidFailWithError(FileCopier* fileCopier, Error* error)
{
    Q_UNUSED(fileCopier);
    this->setRunning(false);
    this->setShouldRun(false);

    ui->progressLabel->setText("Ordering failed");
    this->setState(FAIL);

    if( _delegate != NULL )
        _delegate->taskWidgetDidFail(this, error);
}

void TaskWidget::fileCopierDidFinishCopy(FileCopier* fileCopier)
{
    Q_UNUSED(fileCopier);
    ui->progressLabel->setText("Ordering complete");

    if( hasUploadOption(_option) )
    {
        QStringList successfulFileInfos = _fileCopier.successfulFiles();
        Q_FOREACH( QString aFileInfo, successfulFileInfos )
        {
            myUploadingFiles.push_back(aFileInfo);
        }
    }

    this->processMedias();
}


// - Zipper Delegate Methods ----------------------------------------------------------------------------------------

bool TaskWidget::zipperShouldBeginZip(Zipper* zipper)
{
    Q_UNUSED(zipper);
    return true;
}

void TaskWidget::zipperDidBeginZip(Zipper* zipper)
{
    Q_UNUSED(zipper);
    ui->progressLabel->setText(QString("Zipping files to %1 now").arg(_zipper.directory() + QString("/") + _zipper.archiveName()));
}

void TaskWidget::zipperWasCancelled(Zipper* zipper)
{
    Q_UNUSED(zipper);
    this->setRunning(false);

    ui->progressLabel->setText("Zip cancelled");

    if( _delegate != NULL )
        _delegate->taskWidgetWasCancelled(this);
}

void TaskWidget::zipperDidFinishZip(Zipper* zipper)
{
    Q_UNUSED(zipper);
    this->setRunning(false);
    this->setShouldRun(false);

    ui->progressLabel->setText(QString("Zip complete to %1").arg(_zipper.directory() + "/" + _zipper.archiveName()));
    this->setState(TaskWidget::DONE);

    if( _delegate != NULL )
        _delegate->taskWidgetDidFinish(this);
}

void TaskWidget::zipperDidFailWithError(Zipper* zipper, Error* error)
{
    Q_UNUSED(zipper);
    this->setRunning(false);
    this->setShouldRun(false);

    ui->progressLabel->setText("Zip failed");
    this->setState(TaskWidget::FAIL);

    if( _delegate != NULL )
        _delegate->taskWidgetDidFail(this, error);
}



// - FTP Uploader Delegate Methods -------------------------------------------------------------------------------------------

void TaskWidget::FTPUploaderDidConnect(FTPUploader* uploader)
{
    Q_UNUSED(uploader);
    this->ui->progressLabel->setText(QString("Connecting(%1) success").arg(uploader->server()));
}

bool TaskWidget::FTPUploaderShouldBeginUploading(FTPUploader* uploader)
{
    Q_UNUSED(uploader);
    return true;
}

void TaskWidget::FTPUploaderDidBeginUploading(FTPUploader* uploader)
{
    Q_UNUSED(uploader);
    this->ui->progressLabel->setText(QString("Connecting FTP Server(%1) ...").arg(uploader->server()));
}

void TaskWidget::FTPUploaderWasCancelled(FTPUploader* uploader)
{
    Q_UNUSED(uploader);
    this->setRunning(false);
    this->ui->progressLabel->setText("Uploading cancelled");

    if( _delegate != NULL )
        _delegate->taskWidgetWasCancelled(this);
}

void TaskWidget::FTPUploaderDidFinishUpload(FTPUploader* uploader)
{
    Q_UNUSED(uploader);
    this->ui->progressLabel->setText("Uploading complete");

	int numOfPics = this->numberOfPicsFromFiles( myUploadingFiles );
	int numOfVideos = this->numberOfVideosFromFiles( myUploadingFiles );

    _webServiceNotifier.notifyAfterUploading(this->shootTitleDataSource()->userTokenId(), this->shootTitleId(), numOfPics, numOfVideos, numOfPics, numOfVideos, _noteToEditor, _mailingCheck );
}

void TaskWidget::FTPUploaderDidFailWithError(FTPUploader* uploader, Error* error)
{
    Q_UNUSED(uploader);

    this->setRunning(false);
    this->setShouldRun(false);
    this->ui->progressLabel->setText(QString("Uploading(%1) failed").arg(uploader->server()));

    this->setState(TaskWidget::FAIL);

    // - send Fail Email Uploading-------------------------------------
    int numOfPics = this->numberOfPicsFromFiles( myUploadingFiles );
    int numOfVideos = this->numberOfVideosFromFiles( myUploadingFiles );
//    QString tokenID = this->shootTitleDataSource()->userTokenId();
    QString userID = this->shootTitleDataSource()->userId();
    QString shootTitle = _shootTitleDataSource->shootTitleForId(_shootTitleId);

    _webServiceNotifier.sendUploadingFailEmail( userID, shootTitle, numOfPics, numOfVideos);
    //-----------------------------------------------------------------
    if( _delegate )
        _delegate->taskWidgetDidFail(this, error);
}

// percent = [1, 100]
void TaskWidget::FTPUploaderDidProgressToPercent(FTPUploader* uploader, int percent)
{
    Q_UNUSED(uploader);

    QFileInfo currentFile(uploader->currentFileURL());
    QString fileName = currentFile.fileName();

    int totalSeconds = _uploader.leftSeconds();
    int leftSeconds = totalSeconds % 60;
    int leftMinutes = (totalSeconds / 60) % 60;
    int leftHours = (totalSeconds / 3600) % 24;
    int leftDays = totalSeconds / (24*3600);

    if( leftDays == 0 ) {
        ui->progressLabel->setText(QString("Uploading %1, %2% (%3/%4, ETA-%5:%6:%7)").arg(fileName).arg(percent).arg(_uploader.currentFileIndex()+1).arg(_uploader.totalFileCount()).arg(leftHours, 2, 10, QChar('0')).arg(leftMinutes, 2, 10, QChar('0')).arg(leftSeconds, 2, 10, QChar('0')));
    }
    else {
        ui->progressLabel->setText(QString("Uploading %1, %2% (%3/%4, ETA-%5days%6:%7:%8)").arg(fileName).arg(percent).arg(_uploader.currentFileIndex()+1).arg(_uploader.totalFileCount()).arg(leftDays).arg(leftHours, 2, 10, QChar('0')).arg(leftMinutes, 2, 10, QChar('0')).arg(leftSeconds, 2, 10, QChar('0')));
    }
}

void TaskWidget::FTPUploaderIsWaitingNetworkAvailable(FTPUploader* uploader)
{
    Q_UNUSED(uploader);

    this->ui->progressLabel->setText("upload is waiting available network...");
}



// - WebService Notifier Delegate Methods --------------------------------------------------------------------------------------------

bool TaskWidget::webServiceNotifierShouldBeginNotify(WebServiceNotifier* notifier)
{
    Q_UNUSED(notifier);
    return true;
}

void TaskWidget::webServiceNotifierDidBeginNotify(WebServiceNotifier* notifier)
{
    Q_UNUSED(notifier);
    ui->progressLabel->setText("Notifying to Web Service now ...");
}

void TaskWidget::webServiceNotifierWasCancelled(WebServiceNotifier* notifier)
{
    Q_UNUSED(notifier);
    this->setRunning(false);
    ui->progressLabel->setText("Notifying cancelled");

    if( _delegate != NULL )
        _delegate->taskWidgetWasCancelled(this);
}

void TaskWidget::webServiceNotifierDidFailWithError(WebServiceNotifier* notifier, Error* error)
{
    Q_UNUSED(notifier);

    this->setRunning(false);
    this->setShouldRun(false);

    ui->progressLabel->setText(QString("Notifying failed (reason : %1)").arg(error->localizedFailureReason()));
    this->setState(FAIL);

    if( _delegate != NULL )
        _delegate->taskWidgetDidFail(this, error);
}

void TaskWidget::webServiceNotifierDidFinishNotificationBeforeUploading( WebServiceNotifier * notifier )
{
    Q_UNUSED(notifier);
    //_uploader.uploadFiles(myUploadingFiles, FTP_ADDRESS, this->ftpDirectory(), FTP_USER, _delegate->ftpPassword());
    _uploaderManager.uploadFiles(myUploadingFiles, this->ftpDirectory(), FTP_USER, _delegate->ftpPassword());
}

void TaskWidget::webServiceNotifierDidFinishNotificationAfterUploading( WebServiceNotifier * notifier )
{
    Q_UNUSED(notifier);

    ui->progressLabel->setText("Notification has been sent to Web Service");
	this->setRunning(false);
	this->setShouldRun(false);
	this->setState(DONE);

    if( _delegate == NULL ) 
		_delegate->taskWidgetDidFinish(this);
}

// percent = [1, 100]
void TaskWidget::webServiceNotifierDidProgressToPercent(WebServiceNotifier* notifier, int percent)
{
    Q_UNUSED(notifier);
    ui->progressLabel->setText(QString("Notifying to Web Service %1%").arg(percent));
}


// - Image Compressor Delegate Methods --------------------------------------------------------------------------------------------

bool TaskWidget::imageCompressorShouldBegin(ImageCompressor* imageCompressor)
{
    Q_UNUSED(imageCompressor);
    return true;
}

void TaskWidget::imageCompressorDidBegin(ImageCompressor* imageCompressor)
{
    Q_UNUSED(imageCompressor);
    ui->progressLabel->setText("Compressing Images ...");
}

void TaskWidget::imageCompressorWasCancelled(ImageCompressor* imageCompressor)
{
    Q_UNUSED(imageCompressor);
    this->setRunning(false);
    ui->progressLabel->setText("Compressing cancelled");

    if( _delegate != NULL )
        _delegate->taskWidgetWasCancelled(this);
}

void TaskWidget::imageCompressorDidFailWithError(ImageCompressor* imageCompressor, Error* error)
{
    Q_UNUSED(imageCompressor);
    this->setRunning(false);
    this->setShouldRun(false);

    ui->progressLabel->setText("Compressing failed");
    this->setState(FAIL);

    if( _delegate != NULL )
        _delegate->taskWidgetDidFail(this, error);
}

void TaskWidget::imageCompressorDidFinish(ImageCompressor* imageCompressor)
{
    Q_UNUSED(imageCompressor);
    ui->progressLabel->setText("Compressing complete");

    if( hasUploadOption(_option) )
    {
        QStringList successfulFileInfos = _imageCompressor.successfulFiles();
        Q_FOREACH( QString aFileInfo, successfulFileInfos )
        {
            myUploadingFiles.push_back(aFileInfo);
        }
    }

    this->processMedias();
}

// - DNG Converter Delegate Methods --------------------------------------------------------------------------------------------
bool TaskWidget::dngConverterShouldBeginConverting(DNGConverter* converter)
{
    Q_UNUSED(converter);
    return true;
}

void TaskWidget::dngConverterDidBeginConverting(DNGConverter* converter)
{
    Q_UNUSED(converter);
    this->ui->progressLabel->setText("Converting Raw files...");
}

void TaskWidget::dngConverterWasCancelled(DNGConverter* converter)
{
    Q_UNUSED(converter);
    this->ui->progressLabel->setText("Converting cancelled");
    this->setRunning(false);
    if( _delegate != NULL )
        _delegate->taskWidgetWasCancelled(this);
}

bool TaskWidget::dngConverterShouldContinueProcessing(DNGConverter* converter)
{
    Q_UNUSED(converter);
    return true;
}

void TaskWidget::dngConverterDidProgressToPercent(DNGConverter* converter, int percent)
{
    Q_UNUSED(converter);
    Q_UNUSED(percent);
    QVariantMap currentFileInfo = _dngConverter.currentFileInfo();
    QFileInfo fileInfo(currentFileInfo[DCRAWURL].toString());
    QString currentFileName = fileInfo.fileName();
    this->ui->progressLabel->setText(QString("Converting RAW file: %1 (total:%2 current:%3)").arg(currentFileName).arg(_dngConverter.totalFileCount()).arg(_dngConverter.currentFileIndex()+1));
}

void TaskWidget::dngConverterDidFailWithError(DNGConverter* converter, Error* error)
{
    Q_UNUSED(converter);
    Q_UNUSED(error);

    this->ui->progressLabel->setText(error->localizedDescription());
    this->setRunning(false);
    this->setShouldRun(false);
    this->setState(TaskWidget::FAIL);
    if( _delegate != NULL )
        _delegate->taskWidgetDidFail(this, error);
}

void TaskWidget::dngConverterDidFinishConverting(DNGConverter* converter)
{
    Q_UNUSED(converter);
    this->ui->progressLabel->setText(QString("Raw file converting completed."));

    if( hasUploadOption(_option) )
    {
        QVariantList fileInfos = _dngConverter.resultFileInfos();
        Q_FOREACH( QVariant fileInfo, fileInfos )
        {
            if( fileInfo.toMap()[DCSUCESS].toBool() )
                myUploadingFiles.push_back(fileInfo.toMap()[DCDNGURL].toString());
            else
                myUploadingFiles.push_back(fileInfo.toMap()[DCRAWURL].toString());
        }
    }

    this->processMedias();
}
