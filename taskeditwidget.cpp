#include "taskeditwidget.h"
#include "ui_taskeditwidget.h"
#include "ShootTitleDataSource.h"
#include "preference.h"
#include <QStandardItemModel>
#include <QFileDialog>
#include <QStringList>
#include <QDir>
#include <QDebug>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <QLineEdit>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QUrl>
#include <QLibrary>
#include "qmediainfo.h"

class TaskEditWidgetDelegate;
class TaskEditWidget;

//TaskEditWidget* Common::taskEditCommon = new TaskEditWidget(this);
TaskEditWidget::TaskEditWidget(MainWindow *arg1) :
    window(arg1), ui(new Ui::TaskEditWidget)
//TaskEditWidget::TaskEditWidget(QWidget *parent) :
//    QDialog(parent), ui(new Ui::TaskEditWidget)
{
    ui->setupUi(this);
    _shootTitleIndex = 0;
    _shootTitleDataSource = NULL;
    _autoEditing = false;

    this->setWindowModality(Qt::NonModal);

    _pic_filterProxy.setSourceModel(&_pic_model);
    _pic_filterProxy.setDynamicSortFilter(true);

    _video_filterProxy.setSourceModel(&_video_model);
    _video_filterProxy.setDynamicSortFilter(true);

    _audio_filterProxy.setSourceModel(&_audio_model);
    _audio_filterProxy.setDynamicSortFilter(true);

    this->ui->btnNext->setVisible(false);

    this->ui->picTableView->setModel(&_pic_model);
    this->ui->picTableView->setColumnWidth(0, 100);
    this->ui->picTableView->setItemDelegate(&_comboBoxItemDelegate);
    this->ui->picTableView->verticalHeader()->hide();
    this->ui->picTableView->verticalHeader()->resizeSections(QHeaderView::Fixed);
    this->ui->picTableView->setDragEnabled(true);
    this->ui->picTableView->setAcceptDrops(true);
    this->ui->picTableView->setDropIndicatorShown(true);
    this->ui->picTableView->setDragDropMode(QAbstractItemView::InternalMove);
    this->ui->picTableView->setColumnHidden( 3, true );

    this->ui->videoTableView->setModel(&_video_model);
    this->ui->videoTableView->setColumnWidth(0, 300);
    this->ui->videoTableView->setItemDelegate(&_comboBoxItemDelegate);
    this->ui->videoTableView->verticalHeader()->hide();
    this->ui->videoTableView->verticalHeader()->resizeSections(QHeaderView::Fixed);
    this->ui->videoTableView->setDragEnabled(true);
    this->ui->videoTableView->setAcceptDrops(true);
    this->ui->videoTableView->setDropIndicatorShown(true);
    this->ui->videoTableView->setDragDropMode(QAbstractItemView::InternalMove);

    this->ui->audioTableView->setModel(&_audio_model);
    this->ui->audioTableView->setColumnWidth(0, 100);
    this->ui->audioTableView->setItemDelegate(&_comboBoxItemDelegate);
    this->ui->audioTableView->verticalHeader()->hide();
    this->ui->audioTableView->verticalHeader()->resizeSections(QHeaderView::Fixed);
    this->ui->audioTableView->setDragEnabled(true);
    this->ui->audioTableView->setAcceptDrops(true);
    this->ui->audioTableView->setDropIndicatorShown(true);
    this->ui->audioTableView->setDragDropMode(QAbstractItemView::InternalMove);
    this->ui->audioTableView->setColumnHidden( 3, true );

    this->ui->searchShootTitleEdit->setFocus();

//    connect( this->ui->searchShootTitleEdit, SIGNAL(textChanged(QString &text)), this, SLOT(searchedShootTitle( QString &)) );
//    connect( ui->searchShootTitleEdit, SIGNAL(textChanged(const QString &text)), this, SLOT(searchedShootTitle(QString text)) );

    for( int i = 0 ; i < NUMBER_OF_SUBMIT_OPTIONS ; i++ )
        this->ui->optionCombo->addItem(stringForSubmitOption(i));

    _instance = new VlcInstance(VlcCommon::args(), this);
    _player = new VlcMediaPlayer(_instance);
    _player->setVideoWidget(ui->videoPlayer);
    this->ui->videoPlayer->setMediaPlayer(_player);
    this->ui->videoSeeker->setMediaPlayer(_player);
}

TaskEditWidget::~TaskEditWidget()
{
//    delete _player;
//    delete _instance;
    delete ui;
}

// - Getter / Setter ----------------------------------------------------------------------------------------------

QString TaskEditWidget::folderURL()
{
    return _folderURL;
}

// property
void TaskEditWidget::setFolderURL(QString value)
{
    _folderURL = value;
    this->ui->folderURLEdit->setText(value);
    this->updateClips();
}

SUBMIT_OPTION TaskEditWidget::option()
{
    return _option;
}

void TaskEditWidget::setOption(SUBMIT_OPTION option)
{
    _option = option;
    this->ui->optionCombo->setCurrentIndex(option);
    this->updateClips();
}

QVariantList& TaskEditWidget::clips()
{
    return _model.movies();
}

void TaskEditWidget::setClips(QVariantList& clips)
{
    _model.setMovies(clips);
}

TaskEditWidgetDelegate* TaskEditWidget::delegate()
{
    return _delegate;
}

void TaskEditWidget::setDelegate(TaskEditWidgetDelegate* delegate)
{
    _delegate = delegate;
    this->validateSubmitOption();
}

TaskEditWidgetDelegate *TaskEditWidget::getDelegate()
{
    return _delegate;
}

bool TaskEditWidget::autoEditing()
{
    return _autoEditing;
}

void TaskEditWidget::setAutoEditing(bool value)
{
    _autoEditing = value;
    //this->ui->autoEditingCheck->setChecked(value);
}

QString TaskEditWidget::shootTitleId()
{
//    qDebug() << "this->ui->shootTitleCombo->currentIndex()-----: " << this->ui->shootTitleCombo->currentIndex();
//    qDebug() << "_shootTitleDataSource->numberOfShootTitles()-----: " << _shootTitleDataSource->numberOfShootTitles();
//    for( int i = 0; i < _shootTitleDataSource->numberOfShootTitles(); i++ ){
//        qDebug() << i << ": " << _shootTitleDataSource->shootTitleIdAtIndex(i);
//    }
//    qDebug() << ui->shootTitleCombo->currentText();
    return _shootTitleDataSource->shootTitleIdAtIndex( ui->shootTitleCombo->currentText() );
//    return _shootTitleDataSource->shootTitleIdAtIndex(this->ui->shootTitleCombo->currentIndex());
}

void TaskEditWidget::setShootTitleId(QString& value)
{
    int index = _shootTitleDataSource->shootTitleIndexForId(value);
    this->ui->shootTitleCombo->setCurrentIndex(index);
}

ShootTitleDataSource* TaskEditWidget::shootTitleDataSource()
{
    return _shootTitleDataSource;
}

void TaskEditWidget::setShootTitleDataSource(ShootTitleDataSource* value)
{
    _shootTitleDataSource = value;
    int titleCount = _shootTitleDataSource->numberOfShootTitles();

    this->ui->shootTitleCombo->clear();
    _serverShootTitleList.clear();

    for( int i = 0; i < titleCount; ++i ) {
//        this->ui->shootTitleCombo->addItem(_shootTitleDataSource->shootTitleAtIndex(i));
        _serverShootTitleList.append( _shootTitleDataSource->shootTitleAtIndex(i) );
    }

    addShootTitleList( &_serverShootTitleList );
}

void TaskEditWidget::addShootTitleList(QStringList *sl )
{
    this->ui->shootTitleCombo->clear();

    for ( int i = 0; i < sl->length(); i++ ) {
        this->ui->shootTitleCombo->addItem( sl->at(i) );
    }

}

void TaskEditWidget::searchedShootTitle(QString text)
{
    QStringList lstShootTitle;

    lstShootTitle = _serverShootTitleList.filter(text, Qt::CaseInsensitive);

    this->ui->shootTitleCombo->clear();

    if ( text.isEmpty() ) {
        for( int i = 0; i < _serverShootTitleList.length(); i++ ) {
            this->ui->shootTitleCombo->addItem( _serverShootTitleList.at(i) );
        }
    } else {
        for( int i = 0; i < lstShootTitle.length(); i++ ) {
            this->ui->shootTitleCombo->addItem( lstShootTitle.at(i) );
        }
    }
}

void TaskEditWidget::assignTableModel(TableModel& toTm, TableModel& fromTm)
{
    QVariantList movies = fromTm.movies();
//    QVariantMap rows;
//    QVariantList files = 0;

//    toTm.setMovies( 0 );

//    for( int i = 0; i < movies.count(); i++ ) {
//        rows.push_back( i );
//    }
    if ( movies.count() > 0 )
        toTm.setMovies( movies );

}

bool TaskEditWidget::mailingCheck()
{
	return _mailingCheck;
}

void TaskEditWidget::setMailingCheck( bool flag )
{
	_mailingCheck = flag;
    //this->ui->mailingCheck->setChecked(flag);
}

QString TaskEditWidget::noteToEditor()
{
	return _noteToEditor;
}

void TaskEditWidget::setNoteToEditor( QString & note )
{
	_noteToEditor = note;
	this->ui->noteTextEdit->setPlainText(note);
}

void TaskEditWidget::updateClips()
{
    if( _folderURL.isEmpty() )
        return;

    QVariantList allFiles;
    if ( _option == SUBMIT_COMPRESS_VIDEO || _option == SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND ) {

        QStringList extensions = this->fileExtensionsForOption( SUBMIT_COMPRESS_VIDEO );
        if( !extensions.isEmpty() )
        {
            QStringList fileURLs = this->fileURLsAtDirectoryPath(_folderURL, extensions);
            QVariantList files;
            if( !fileURLs.isEmpty() )
            {
                this->ui->btnNext->setVisible(true);
                Q_FOREACH( QString aFileURL, fileURLs )
                {
                    QVariantMap aMovieInfo;
                    aMovieInfo[AppKey_URL] = aFileURL;
                    aMovieInfo[AppKey_RemoveAudio] = true;
                    files.push_back(aMovieInfo);
                    allFiles.push_back(aMovieInfo);
                }
                _video_model.setMovies(files);
            }
        }
    }

    if ( _option == SUBMIT_CONVERT_DNG_PHOTO || _option == SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND ) {

        QStringList extensions = this->fileExtensionsForOption( SUBMIT_CONVERT_DNG_PHOTO );
        if( !extensions.isEmpty() )
        {
            QStringList fileURLs = this->fileURLsAtDirectoryPath(_folderURL, extensions);
            QVariantList files;
            if( !fileURLs.isEmpty() )
            {
                Q_FOREACH( QString aFileURL, fileURLs )
                {
                    QVariantMap aMovieInfo;
                    aMovieInfo[AppKey_URL] = aFileURL;
                    aMovieInfo[AppKey_RemoveAudio] = true;
                    files.push_back(aMovieInfo);
                    allFiles.push_back(aMovieInfo);
                }
                _pic_model.setMovies(files);
            }
        }
    }

    if ( _option == SUBMIT_COMPRESS_AUDIO || _option == SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND ) {

        QStringList extensions = this->fileExtensionsForOption( SUBMIT_COMPRESS_AUDIO );
        if( !extensions.isEmpty() )
        {
            QStringList fileURLs = this->fileURLsAtDirectoryPath(_folderURL, extensions);
            QVariantList files;
            if( !fileURLs.isEmpty() )
            {
                Q_FOREACH( QString aFileURL, fileURLs )
                {
                    QVariantMap aMovieInfo;
                    aMovieInfo[AppKey_URL] = aFileURL;
                    aMovieInfo[AppKey_RemoveAudio] = true;
                    files.push_back(aMovieInfo);
                    allFiles.push_back(aMovieInfo);
                }
                _audio_model.setMovies(files);
            }
        }
    }

    _model.setMovies(allFiles);

}

QStringList TaskEditWidget::fileURLsAtDirectoryPath(QString directoryPath, QStringList extensions)
{
    QString nameFilter;
    QString aExtension;
    Q_FOREACH( aExtension, extensions )
	{
        nameFilter = nameFilter + "*." + aExtension + " ";
    }

    qDebug() << "filter = " << nameFilter << "\n";

    QDir directory(directoryPath, nameFilter, QDir::Unsorted, QDir::Files|QDir::NoDotAndDotDot|QDir::Readable);

    QFileInfoList files = directory.entryInfoList();
    QStringList fullPaths;
    QFileInfo aFileInfo;
    Q_FOREACH( aFileInfo, files ) 
	{
        fullPaths.push_back(aFileInfo.absoluteFilePath());
    }

    qDebug() << "files = " << fullPaths << "\n";
    return fullPaths;
}

QStringList TaskEditWidget::fileExtensionsForOption( SUBMIT_OPTION opt )
{
    QStringList extensions;
    switch( opt ) 
	{
        case SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND:
            extensions.append(Preference::extensionsForVideos());
            extensions.append(Preference::extensionsForPhotos());
            extensions.append(Preference::extensionsForSounds());
            extensions.append(Preference::extensionsForDNGs());
			break;

        case SUBMIT_COMPRESS_VIDEO:
            extensions.append(Preference::extensionsForVideos());
			break;

        case SUBMIT_COMPRESS_AUDIO:
            extensions.append(Preference::extensionsForSounds());
            break;

        case SUBMIT_CONVERT_DNG_PHOTO:
            extensions.append(Preference::extensionsForPhotos());
            extensions.append(Preference::extensionsForDNGs());
			break;
    }

    return extensions;
}

void TaskEditWidget::uploadClips(QString fpath)
{
    if ( fpath.isEmpty() ) {

        // Just Clips
        if( _pic_model.movies().count() == 0 && _video_model.movies().count() == 0 && _audio_model.movies().count() == 0 ) //???
            return;

        QVariantList files;
        Q_FOREACH( QVariant aMovieInfo, _pic_model.movies() )
        {
            files.push_back( aMovieInfo );
        }
        Q_FOREACH( QVariant aMovieInfo, _audio_model.movies() )
        {
            files.push_back( aMovieInfo );
        }
        Q_FOREACH( QVariant aMovieInfo, _video_model.movies() )
        {
            files.push_back( aMovieInfo );
        }
        _model.setMovies(files);

        if( hasUploadOption(_option) && ui->shootTitleCombo->currentText().isEmpty() )
            return;

//        if( _delegate != NULL )
//            _delegate->taskEditWidgetDidEndEditing(this);

        pw->activateWindow();
        QDialog::accept();


    } else {

        // All Clips

        QVariantList files;
        Q_FOREACH( QVariant aMovieInfo, _pic_model.movies() )
        {
            files.push_back( aMovieInfo );
        }
        Q_FOREACH( QVariant aMovieInfo, _audio_model.movies() )
        {
            files.push_back( aMovieInfo );
        }
        Q_FOREACH( QVariant aMovieInfo, _video_model.movies() )
        {
            files.push_back( aMovieInfo );
        }

        QVariantMap aMovieInfo;
        aMovieInfo[AppKey_URL] = fpath;
        aMovieInfo[AppKey_RemoveAudio] = true;
        files.push_back(aMovieInfo);

        _model.setMovies(files);


        if( hasUploadOption(_option) && ui->shootTitleCombo->currentText().isEmpty() )
            return;

//        if( _delegate != NULL )
//            _delegate->taskEditWidgetDidEndEditing(this);

        QDialog::accept();

    }

}

void TaskEditWidget::setParentMainwindow(QWidget *pm)
{
    pmw = pm;
}

QStringList TaskEditWidget::getVideoFiles()
{
    return videoFileList;
}

bool TaskEditWidget::getRemoveAudio()
{
    return bRemoveAudio;
}

void TaskEditWidget::_videoEidtWidgetView()
{
//    if ( _delegate != NULL ) {
//        _delegate->videoEditWidgetView( this );
//    }

    window->videoEditWidgetView( getVideoFiles(), getRemoveAudio(), updateFiles,
                                 this->folderURL(), this->option(), this->autoEditing(),
                                 this->shootTitleId(), this->mailingCheck(), this->noteToEditor() );
}

void TaskEditWidget::setDataModel(QVariantList picModel, QVariantList videoModel, QVariantList audioModel)
{
    if (!picModel.isEmpty())
        _pic_model.setMovies(picModel);
    if (!videoModel.isEmpty()){
        _video_model.setMovies(videoModel);
        ui->btnNext->setVisible(true);
    }
    if (!audioModel.isEmpty())
        _audio_model.setMovies(audioModel);
}

void TaskEditWidget::setEnvironment(QString fURL, SUBMIT_OPTION opt, bool autoEdit, bool mailCheck, QString shootID, QString noteEdit)
{
    _folderURL = fURL;
    _option = opt;
    _autoEditing = autoEdit;
    _mailingCheck = mailCheck;
//    ui->shootTitleCombo->setCurrentText(shootID);
    ui->shootTitleCombo->setCurrentIndex(0);
    _noteToEditor = noteEdit;

}

//void TaskEditWidget::_videoPlayWidgetView(VideoEditWidget *editWidget)
//{
//    if ( _delegate != NULL ) {
//        _delegate->videoPlayWidgetView(editWidget);
//    }
//}

//void TaskEditWidget::_videoEditReEditView()
//{
//    if ( _delegate != NULL ) {
//        _delegate->taskEditWidgetView();
//    }
//}

//void TaskEditWidget::_videoEditWidgetCancelView()
//{
//    if ( _delegate != NULL ) {
//        _delegate->videoEditWidgetCancelView(this);
//    }
//}

void TaskEditWidget::validateSubmitOption()
{
    QStandardItemModel* model = (QStandardItemModel*)this->ui->optionCombo->model();
    int count = ui->optionCombo->count();
    for( int index = 0; index < count; ++index ) 
	{
        QModelIndex modelIndex = model->index(index, this->ui->optionCombo->modelColumn(), this->ui->optionCombo->rootModelIndex());
        QStandardItem* item = model->itemFromIndex(modelIndex);
        item->setEnabled(window->taskEditWidgetCanUseSubmitOption(/*this, */index));
    }
}


// - Slots ------------------------------------------------------------------------------------------------------
void TaskEditWidget::on_addButton_clicked()
{
    QString nameFilter;
    QString aExtension;
	QStringList extensions = this->fileExtensionsForOption( _option );
	if( extensions.count() == 0 )
		return;

	Q_FOREACH( aExtension, extensions )
        nameFilter = nameFilter + "*." + aExtension + " ";
	
	QStringList fileNames = QFileDialog::getOpenFileNames( this, tr("Select Files"), QString(), nameFilter );
	
	QVariantList files;
    if( !fileNames.isEmpty() ) 
	{
        Q_FOREACH( QVariant aMovieInfo, _pic_model.movies() ) //???
		{
			files.push_back( aMovieInfo );

			QVariantMap myMovieInfo = aMovieInfo.toMap();
			QString fileName = myMovieInfo[AppKey_URL].toString();

			if( fileNames.contains(fileName) )
				fileNames.removeOne(fileName);
		}

//        Q_FOREACH( QVariant aMovieInfo, _video_model.movies() ) //???
//		{
//			files.push_back( aMovieInfo );

//			QVariantMap myMovieInfo = aMovieInfo.toMap();
//			QString fileName = myMovieInfo[AppKey_URL].toString();

//			if( fileNames.contains(fileName) )
//				fileNames.removeOne(fileName);
//		}

		Q_FOREACH( QString aFileURL, fileNames ) 
		{
			QVariantMap aMovieInfo;
			aMovieInfo[AppKey_URL] = aFileURL;
			aMovieInfo[AppKey_RemoveAudio] = true;
            files.push_back(aMovieInfo);
		}

        _pic_model.setMovies(files); //???
	}

}

void TaskEditWidget::on_deleteButton_clicked()
{

    QItemSelectionModel* selectionPicModel = ui->picTableView->selectionModel();
    QItemSelectionModel* selectionVideoModel = ui->videoTableView->selectionModel();

    QModelIndexList pic_rows = selectionPicModel->selectedRows();
    QModelIndexList video_rows = selectionVideoModel->selectedRows();

    QList<int> pic_rows2;
    QList<int> video_rows2;

    Q_FOREACH(QModelIndex aRow, pic_rows)
	{
        pic_rows2.push_back(aRow.row());
    }

    _pic_model.removeMoviesAtRows(pic_rows2);

    Q_FOREACH( QModelIndex aRow, video_rows )
    {
        video_rows2.push_back( aRow.row() );
    }

    _video_model.removeMoviesAtRows( video_rows2 );

    VlcMedia * currentMedia = _player->currentMedia();
    if( currentMedia != NULL )
    {
        if( currentMedia->state() == Vlc::State::Playing )
            _player->stop();
    }
}

void TaskEditWidget::on_btnNext_clicked()
{

    if( _pic_model.movies().count() == 0 && _video_model.movies().count() == 0 && _audio_model.movies().count() == 0 ) //???
        return;

//    QVariantList files;
    updateFiles.clear();

    QVariantList sendPic, sendVideo, sendAudio;

    Q_FOREACH( QVariant aMovieInfo, _pic_model.movies() )
    {
        updateFiles.push_back( aMovieInfo );
        sendPic.push_back(aMovieInfo);
    }
    Q_FOREACH( QVariant aMovieInfo, _audio_model.movies() )
    {
        updateFiles.push_back( aMovieInfo );
        sendAudio.push_back(aMovieInfo);
    }
    Q_FOREACH( QVariant aMovieInfo, _video_model.movies() )
    {
        updateFiles.push_back( aMovieInfo );
        sendVideo.push_back(aMovieInfo);
    }
    _model.setMovies(updateFiles);

    window->setDataModel(sendPic, sendVideo, sendAudio);

    // if option include ftp upload, we should select shoot title.
//    qDebug() << "shootTitleCombo.currentIndex()-----: " << ui->shootTitleCombo->currentIndex() << ui->shootTitleCombo->currentText().isEmpty();
//    if( hasUploadOption(_option) && ui->shootTitleCombo->currentIndex() == 0 )
    if( hasUploadOption(_option) && ui->shootTitleCombo->currentText().isEmpty() )
        return;

    if ( _video_model.movies().count() > 0 ) {

//        QDialog::accept();

        videoFileList.clear();

        bRemoveAudio = true;

        for ( int i = 0; i < _video_model.movies().count(); i++ ) {
            QVariantMap videoFileInfo = _video_model.movies().at(i).toMap();
            videoFileList.append( videoFileInfo[AppKey_URL].toString() );

            if ( videoFileInfo[AppKey_RemoveAudio].toBool() == false )
                bRemoveAudio = false;
        }

//        _player->pause();
        _player->stop();
        delete _player;
        delete _instance;

        this->_videoEidtWidgetView();

        QDialog::accept();
//        QDialog::accept();
//        VideoEditWidget vEditWidget(this);
//        vEditWidget.setVideoFileList( videoFileList, bRemoveAudio );
//        vEditWidget.setParentTaskEditWidget(this);
//        vEditWidget.exec();


    }
}

void TaskEditWidget::on_cancelButton_clicked()
{
//    if( _delegate != NULL )
//        _delegate->taskEditWidgetDidCancel(this);
    window->taskEditWidgetDidCancel();
    QDialog::reject();
}

void TaskEditWidget::on_browse_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                               tr("Select folder containing movies"), QString(""), QFileDialog::ReadOnly);

    if( directory.isEmpty() )
        return;

    this->setFolderURL(directory);
    this->updateClips();
}

void TaskEditWidget::on_optionCombo_currentIndexChanged(int index)
{
    _option = index;

    this->setFolderURL(_folderURL);
    this->updateClips();
    if ( _option == SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND ) {

        /*
        if ( _old_pic_model.movies().count() == 0 && _old_video_model.movies().count() == 0 ) {

            this->setFolderURL(_folderURL);
            this->updateClips();

            assignTableModel( _old_pic_model, _pic_model );
            assignTableModel( _old_video_model, _video_model );

        } else {


            assignTableModel( _pic_model, _old_pic_model );
            assignTableModel( _video_model, _old_video_model );

        }*/

    } else if ( _option == SUBMIT_COMPRESS_VIDEO ) {

//        assignTableModel( _old_pic_model, _pic_model );

        QVariantList movies = _pic_model.movies();
        QList<int> rowsToRemove;

        for( int i = 0; i < movies.count(); i++ ) {
            QVariantMap aFileInfo = movies.at( i ).toMap();
            QString aFilePath = aFileInfo[AppKey_URL].toString();
            QFileInfo fileInfo(aFilePath);
            QString ext = fileInfo.suffix();
            if( ext.isEmpty() )
                continue;
            rowsToRemove.push_back( i );
        }
        _pic_model.removeMoviesAtRows(rowsToRemove);
//        assignTableModel( _video_model, _old_video_model );


    } else if ( _option == SUBMIT_CONVERT_DNG_PHOTO ) {

//        assignTableModel( _old_video_model, _video_model );

        QVariantList movies = _video_model.movies();
        QList<int> rowsToRemove;

        for( int i = 0; i < movies.count(); i++ ) {
            QVariantMap aFileInfo = movies.at( i ).toMap();
            QString aFilePath = aFileInfo[AppKey_URL].toString();
            QFileInfo fileInfo(aFilePath);
            QString ext = fileInfo.suffix();
            if( ext.isEmpty() )
                continue;
            rowsToRemove.push_back( i );
        }
        _video_model.removeMoviesAtRows(rowsToRemove);
//        assignTableModel( _pic_model, _old_pic_model );
    }

    /*
	QVariantList movies = _model.movies();
	QStringList extensions = this->fileExtensionsForOption( _option );
    QList<int> rowsToRemove;
	
	for( int i = 0 ; i < movies.count() ; i++ )
	{
		QVariantMap aFileInfo = movies.at( i ).toMap();
		QString aFilePath = aFileInfo[AppKey_URL].toString();
		QFileInfo fileInfo(aFilePath);
		QString ext = fileInfo.suffix();
		if( ext.isEmpty() )
			continue;
		if( extensions.contains( ext, Qt::CaseInsensitive ) == false )
			rowsToRemove.push_back( i );
	}

    _model.removeMoviesAtRows(rowsToRemove);
*/
}

void TaskEditWidget::on_autoEditingCheck_clicked()
{
    //_autoEditing = this->ui->autoEditingCheck->isChecked();
}

void TaskEditWidget::on_mailingCheck_clicked()
{
    //_mailingCheck = this->ui->mailingCheck->isChecked();
}

void TaskEditWidget::on_noteTextEdit_textChanged()
{
	_noteToEditor = this->ui->noteTextEdit->toPlainText();
}

void TaskEditWidget::on_picTableView_clicked(const QModelIndex &index)
{
	if( this->ui->previewCheckBox->isChecked() == false )
		return;

    QVariantMap movieFileInfo = _pic_model.movies().at( index.row() ).toMap();
	QString fileName = movieFileInfo[AppKey_URL].toString();

	QFileInfo fileInfo(fileName);
	QString ext = fileInfo.suffix().toLower();
	QStringList imageExts;
	imageExts << "jpg" << "jpeg" << "png" << "bmp" << "tiff";
	if( imageExts.contains(ext) )
	{
		this->ui->imageViewer->setHidden( false );
		this->ui->imageViewer->setImage( fileName );
        /*
		Phonon::MediaObject * pMediaObject = this->ui->videoPlayer->mediaObject();
		pMediaObject->stop();
		pMediaObject->clearQueue();
        */

        _player->stop();
	}
	else
	{
		this->ui->imageViewer->setHidden( true );
		
		QStringList movieExts;
        //movieExts << "mov" << "m4v" << "avi";
        movieExts.append(Preference::extensionsForVideos());
        if( movieExts.contains( ext ) )
		{
            /*
			Phonon::MediaObject * pMediaObject = this->ui->videoPlayer->mediaObject();
			if( pMediaObject == NULL )
			{
				pMediaObject->clearQueue();
			}

			Phonon::MediaSource mediaSource( fileName );
			this->ui->videoPlayer->play( mediaSource );
			this->ui->seekSlider->setMediaObject( pMediaObject );
            */

            _media = new VlcMedia(fileName, true, _instance);
            _player->open(_media);
		}
		else
		{
            /*
			Phonon::MediaObject * pMediaObject = this->ui->videoPlayer->mediaObject();
			pMediaObject->stop();
			pMediaObject->clearQueue();
            */

            _player->stop();
		}
	}
}

void TaskEditWidget::on_previewCheckBox_clicked(bool checked)
{
	if( checked == false )
	{
        /*
		Phonon::MediaObject * pMediaObject = this->ui->videoPlayer->mediaObject();
		pMediaObject->clearQueue();
		this->ui->videoPlayer->stop();
		this->ui->seekSlider->setMediaObject( pMediaObject );
        */

		this->ui->imageViewer->setHidden( true );
	}
}

void TaskEditWidget::on_playVideo_clicked()
{
    //this->ui->videoPlayer->play();
    if( _player->state() == Vlc::Paused )
        _player->resume();
    else if( _player->state() == Vlc::Ended )
    {
        _player->stop();
        _player->play();
    }
}

void TaskEditWidget::on_stopVideo_clicked()
{
    //this->ui->videoPlayer->pause();

    _player->pause();
}

void TaskEditWidget::on_searchShootTitleEdit_textChanged(const QString &arg1)
{
    searchedShootTitle(arg1);
}

void TaskEditWidget::on_videoTableView_clicked(const QModelIndex &index)
{
    if( this->ui->previewCheckBox->isChecked() == false )
        return;

    QVariantMap movieFileInfo = _video_model.movies().at( index.row() ).toMap();
    QString fileName = movieFileInfo[AppKey_URL].toString();

    QFileInfo fileInfo(fileName);
    QString ext = fileInfo.suffix().toLower();
    QStringList imageExts;
    imageExts << "jpg" << "jpeg" << "png" << "bmp" << "tiff";
    if( imageExts.contains(ext) )
    {
        this->ui->imageViewer->setHidden( false );
        this->ui->videoPlayer->setHidden( true );
        this->ui->imageViewer->setImage( fileName );
        /*
        Phonon::MediaObject * pMediaObject = this->ui->videoPlayer->mediaObject();
        pMediaObject->stop();
        pMediaObject->clearQueue();
        */

        _player->stop();
    }
    else
    {
        this->ui->imageViewer->setHidden( true );
        this->ui->videoPlayer->setHidden( false );

        QStringList movieExts, audioExts;
        //movieExts << "mov" << "m4v" << "avi";
        movieExts.append( Preference::extensionsForVideos() );
        if( movieExts.contains( ext ) )
        {
            /*
            Phonon::MediaObject * pMediaObject = this->ui->videoPlayer->mediaObject();
            if( pMediaObject == NULL )
            {
                pMediaObject->clearQueue();
            }

            Phonon::MediaSource mediaSource( fileName );
            this->ui->videoPlayer->play( mediaSource );
            this->ui->seekSlider->setMediaObject( pMediaObject );
            */

            _media = new VlcMedia(fileName, true, _instance);
            _player->open(_media);
        }
        else
        {
            /*
            Phonon::MediaObject * pMediaObject = this->ui->videoPlayer->mediaObject();
            pMediaObject->stop();
            pMediaObject->clearQueue();
            */

            _player->stop();
        }
    }
    QMediaInfo mi;
    mi.Open(fileName);
    qDebug() << "@@@ " << fileInfo.filePath() << " " << fileInfo.fileName();
}

void TaskEditWidget::on_audioTableView_clicked(const QModelIndex &index)
{
    if( this->ui->previewCheckBox->isChecked() == false )
        return;

    this->ui->imageViewer->setHidden( true );
    this->ui->videoPlayer->setHidden( true );

    QVariantMap movieFileInfo = _audio_model.movies().at( index.row() ).toMap();
    QString fileName = movieFileInfo[AppKey_URL].toString();

    _media = new VlcMedia(fileName, true, _instance);
    _player->open(_media);

}

void TaskEditWidget::on_addPicButton_clicked()
{
    QString nameFilter;
    QString aExtension;
    QStringList extensions = this->fileExtensionsForOption( SUBMIT_CONVERT_DNG_PHOTO );
    if( extensions.count() == 0 )
        return;

    Q_FOREACH( aExtension, extensions )
        nameFilter = nameFilter + "*." + aExtension + " ";

    QStringList fileNames = QFileDialog::getOpenFileNames( this, tr("Select Files"), QString(), nameFilter );

    QVariantList files;
    if( !fileNames.isEmpty() )
    {
        Q_FOREACH( QVariant aMovieInfo, _pic_model.movies() )
        {
            files.push_back( aMovieInfo );

            QVariantMap myMovieInfo = aMovieInfo.toMap();
            QString fileName = myMovieInfo[AppKey_URL].toString();

            if( fileNames.contains(fileName) )
                fileNames.removeOne(fileName);
        }

        Q_FOREACH( QString aFileURL, fileNames )
        {
            QVariantMap aMovieInfo;
            aMovieInfo[AppKey_URL] = aFileURL;
            aMovieInfo[AppKey_RemoveAudio] = true;
            files.push_back(aMovieInfo);
        }

        _pic_model.setMovies(files);
    }
}

void TaskEditWidget::on_deletePicButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->picTableView->selectionModel();
    QModelIndexList rows = selectionModel->selectedRows();
    QList<int> rows2;

    Q_FOREACH(QModelIndex aRow, rows)
    {
        rows2.push_back(aRow.row());
    }

    _pic_model.removeMoviesAtRows(rows2);

    VlcMedia * currentMedia = _player->currentMedia();
    if( currentMedia != NULL )
    {
        if( currentMedia->state() == Vlc::State::Playing )
            _player->stop();
    }
}

void TaskEditWidget::on_addVideoButton_clicked()
{
    QString nameFilter;
    QString aExtension;
    QStringList extensions = this->fileExtensionsForOption( SUBMIT_COMPRESS_VIDEO );
    if( extensions.count() == 0 )
        return;

    Q_FOREACH( aExtension, extensions )
        nameFilter = nameFilter + "*." + aExtension + " ";

    QStringList fileNames = QFileDialog::getOpenFileNames( this, tr("Select Files"), QString(), nameFilter );

    QVariantList files;
    if( !fileNames.isEmpty() )
    {
        this->ui->btnNext->setVisible(true);
        Q_FOREACH( QVariant aMovieInfo, _video_model.movies() )
        {
            files.push_back( aMovieInfo );

            QVariantMap myMovieInfo = aMovieInfo.toMap();
            QString fileName = myMovieInfo[AppKey_URL].toString();

            if( fileNames.contains(fileName) )
                fileNames.removeOne(fileName);
        }

        Q_FOREACH( QString aFileURL, fileNames )
        {
            QVariantMap aMovieInfo;
            aMovieInfo[AppKey_URL] = aFileURL;
            aMovieInfo[AppKey_RemoveAudio] = true;
            files.push_back(aMovieInfo);
        }

        _video_model.setMovies(files);
    }
}

void TaskEditWidget::on_deleteVideoButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->videoTableView->selectionModel();
    QModelIndexList rows = selectionModel->selectedRows();
    QList<int> rows2;

    Q_FOREACH(QModelIndex aRow, rows)
    {
        rows2.push_back(aRow.row());
    }

    _video_model.removeMoviesAtRows(rows2);

    VlcMedia * currentMedia = _player->currentMedia();
    if( currentMedia != NULL )
    {
        if( currentMedia->state() == Vlc::State::Playing )
            _player->stop();
    }

    if (_video_model.movies().count() == 0 )
        this->ui->btnNext->setVisible(false);
}

void TaskEditWidget::on_addAudioButton_clicked()
{
    QString nameFilter;
    QString aExtension;
    QStringList extensions = this->fileExtensionsForOption( SUBMIT_COMPRESS_AUDIO );
    if( extensions.count() == 0 )
        return;

    Q_FOREACH( aExtension, extensions )
        nameFilter = nameFilter + "*." + aExtension + " ";

    QStringList fileNames = QFileDialog::getOpenFileNames( this, tr("Select Files"), QString(), nameFilter );

    QVariantList files;
    if( !fileNames.isEmpty() )
    {
        Q_FOREACH( QVariant aMovieInfo, _audio_model.movies() )
        {
            files.push_back( aMovieInfo );

            QVariantMap myMovieInfo = aMovieInfo.toMap();
            QString fileName = myMovieInfo[AppKey_URL].toString();

            if( fileNames.contains(fileName) )
                fileNames.removeOne(fileName);
        }

        Q_FOREACH( QString aFileURL, fileNames )
        {
            QVariantMap aMovieInfo;
            aMovieInfo[AppKey_URL] = aFileURL;
            aMovieInfo[AppKey_RemoveAudio] = true;
            files.push_back(aMovieInfo);
        }

        _audio_model.setMovies(files);
    }
}

void TaskEditWidget::on_deleteAudioButton_clicked()
{
    QItemSelectionModel* selectionModel = ui->audioTableView->selectionModel();
    QModelIndexList rows = selectionModel->selectedRows();
    QList<int> rows2;

    Q_FOREACH(QModelIndex aRow, rows)
    {
        rows2.push_back(aRow.row());
    }

    _audio_model.removeMoviesAtRows(rows2);

    VlcMedia * currentMedia = _player->currentMedia();
    if( currentMedia != NULL )
    {
        if( currentMedia->state() == Vlc::State::Playing )
            _player->stop();
    }
}

void TaskEditWidget::on_btnDone_clicked()
{
    if( _pic_model.movies().count() == 0 && _video_model.movies().count() == 0 && _audio_model.movies().count() == 0 ) //???
        return;

    QVariantList files;
    QVariantList sendPic, sendVideo, sendAudio;
    Q_FOREACH( QVariant aMovieInfo, _pic_model.movies() )
    {
        files.push_back( aMovieInfo );
        sendPic.push_back(aMovieInfo);
    }
    Q_FOREACH( QVariant aMovieInfo, _audio_model.movies() )
    {
        files.push_back( aMovieInfo );
        sendAudio.push_back(aMovieInfo);
    }
    Q_FOREACH( QVariant aMovieInfo, _video_model.movies() )
    {
        files.push_back( aMovieInfo );
        sendVideo.push_back(aMovieInfo);
    }
    _model.setMovies(files);

    window->setDataModel(sendPic, sendVideo, sendAudio);

    if( hasUploadOption(_option) && ui->shootTitleCombo->currentText().isEmpty() )
        return;

//    if( _delegate != NULL )
//        _delegate->taskEditWidgetDidEndEditing(this);

    window->taskEditWidgetDidEndEditing(this->folderURL(), this->option(), this->clips(), this->autoEditing(),
                                        this->shootTitleId(), this->mailingCheck(), this->noteToEditor());

    QDialog::accept();
}
