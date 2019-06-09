#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QColor>
#include <QPalette>
#include <QDateTime>
#include <QtDebug>
#include <QMessageBox>
#include <QSound>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "propertyanimation.h"
#include "sequentialanimationgroup.h"
#include "parallelanimationgroup.h"
#include "scrollareaviewportwidget.h"
#include "taskeditwidget.h"
#include "preference.h"
#include "preferencedialog.h"
#include "videoeditwidget.h"

#include <QLineEdit>
#include <QDirIterator>
#include <QDir>
#include <QFile>
#include <QMessageBox>

#define NoneTitle ""
#define NotFound -1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    //,_audioOutput(Phonon::MusicCategory)
{
    ui->setupUi(this);

    QRect scrollAreaContentRect = this->ui->scrollAreaMain->contentsRect();
    QRect scrollAreaRect = this->ui->scrollAreaMain->geometry();

    QRect scrollAreaWidgetContentRect = this->ui->scrollAreaWidgetContents->contentsRect();
    QRect scrollAreaWidgetRect = this->ui->scrollAreaWidgetContents->geometry();

    _scrollWidget = new QWidget;
    this->ui->scrollAreaMain->setWidget(_scrollWidget);
    this->ui->scrollAreaMain->setDelegate(this);
    _scrollWidget->setObjectName("scrollWidget");
    _scrollWidget->setGeometry(scrollAreaContentRect.left(), scrollAreaContentRect.top(), 900, 100);
	//_scrollWidget->setStyleSheet("QWidget { border:1px solid red; }");

    this->ui->scrollAreaMain->viewport()->setStyleSheet("background-color:rgba(0,0,0,0);border:none;");
    this->ui->scrollAreaMain->setWidgetResizable(false);

    _readyTaskCount = 0;
    _runningTaskCount = 0;
    _unstableTaskCount = 0;

    this->_shootTitles.append(NoneTitle);
    this->_shootIds.append(NotFound);

    this->initBindingControls();

    connect(this, SIGNAL(hasRunningTaskChanged(bool)), this, SLOT(on_hasRunningTaskChanged(bool)));
    connect(this, SIGNAL(headerDescriptionChanged(QString&)), this, SLOT(on_headerDescriptionChanged(QString&)));

    _editingWidget = NULL;
    _networkReply = NULL;

	connect( &_networkConfigurationManager, SIGNAL(onlineStateChanged(bool)), this, SLOT(onlineStateChanged(bool)) );
	this->onlineStateChanged( _networkConfigurationManager.isOnline() );

//    PreferenceDialog pfd(this);
//    pfd.getServerList();
    delTempPath();

    vbox = new QVBoxLayout(this);
}

MainWindow::~MainWindow()
{
    delTempPath();

    delete ui;
}

// - Private Methods ------------------------------------------------------------------------------------------------------------------
bool MainWindow::isOnline()
{
	return _networkConfigurationManager.isOnline();
}

void MainWindow::onlineStateChanged( bool isOnline )
{	
	if( isOnline )
	{
		QIcon icon( ":/image/Images/network-connect.tiff" );
		this->ui->networkAccessibilityButton->setIcon( icon );
		this->ui->networkAccessibilityButton->setText( "Network Access Available" );
		this->ui->networkAccessibilityButton->setStyleSheet( "border:none;color:rgba(0,255,0,255);" );
	}
	else
	{
		QIcon icon( ":/image/Images/network-disconnect.tiff" );
		this->ui->networkAccessibilityButton->setIcon( icon );
		this->ui->networkAccessibilityButton->setText( "Network Access Not Available" );
		this->ui->networkAccessibilityButton->setStyleSheet( "border:none;color:rgba(255,0,0,255);" );
    }
}

void MainWindow::initBindingControls()
{
	this->ui->_userNameTextEdit->setText(Preference::username());
	this->ui->_passwordTextEdit->setText(Preference::password());
	this->showLastQueryDate(Preference::lastQueryDate());
}

QString MainWindow::tokenID()
{
    return _tokenID;
}

void MainWindow::setTokenID(QString& tokenId)
{
    _tokenID = tokenId;
}

void MainWindow::setUserID(QString& userID)
{
    _userID = userID;
}

void MainWindow::setArtistName(QString& name)
{
    _artistName = name;
}

void MainWindow::setEmailAddress(QString& email)
{
    _emailAddress = email;
}

void MainWindow::setFtpBaseDirectory(QString& directory)
{
    _ftpBaseDirectory = directory;
}

QVariantList MainWindow::shootTitles()
{
    return _shootTitles;
}

void MainWindow::setShootTitles(QVariantList& titles)
{
    _shootTitles = titles;
}

QVariantList MainWindow::shootIds()
{
    return _shootIds;
}

void MainWindow::setShootIds(QVariantList& ids)
{
    _shootIds = ids;
}

QDateTime MainWindow::lastQueryDate()
{
	return Preference::lastQueryDate();
}

void MainWindow::showLastQueryDate(QDateTime& date)
{
    if( date.isValid() )
        ui->_lastQueryDateField->setText("Last Query Date : " + date.toString("ddd dd/MM/yy h:mm ap"));
    else
        ui->_lastQueryDateField->setText("");
}

void MainWindow::addTaskWidget(TaskWidget* widget, bool animate)
{
    widget->setDelegate(this);

    int count = _taskWidgets.count();
    int newHeightOfBackground = (count+1) * widget->geometry().height();
    QRect scrollWidgetRect = _scrollWidget->geometry();

	//qDebug() << "before change,view port width = " << this->ui->scrollArea->viewport()->width() << "\n";
    _scrollWidget->setGeometry(scrollWidgetRect.x(), scrollWidgetRect.y(), scrollWidgetRect.width(), newHeightOfBackground);

    // Below line make scroll area viewport's resize event occur immediately.
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    // viewport size has been changed by above line.

    //qDebug() << "after change, view port width = " << this->ui->scrollArea->viewport()->width() << "\n";

    QRect frame(-_scrollWidget->width(), 0, _scrollWidget->width(), widget->height());
    widget->setGeometry(frame);
    widget->setParent(_scrollWidget);
    widget->show();

    // add view to scrollview.
    _taskWidgets.insert(_taskWidgets.begin(), widget);
    ++count;

    // shift all views to down.
    if( animate )
	{
        if( count > 1 ) 
		{
            ParallelAnimationGroup* parallelAnimationGroup = new ParallelAnimationGroup;

            for( int i = 1; i < count; ++i ) 
			{
                TaskWidget* taskWidget = _taskWidgets[i];
                QRect originFrame = taskWidget->geometry();
                QRect newFrame = originFrame.translated(0, originFrame.height());
                PropertyAnimation* animation = new PropertyAnimation(taskWidget, "geometry");
                animation->setStartValue(originFrame);
                animation->setEndValue(newFrame);
                animation->setEasingCurve(QEasingCurve::InOutQuad);
                animation->setDuration(250);
                parallelAnimationGroup->addAnimation(animation);
            }

            parallelAnimationGroup->setAnimationBlockingMode(ParallelAnimationGroup::Blocking);
            parallelAnimationGroup->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }
    else
	{
        for( int i = 1; i < count; ++i ) 
		{
            TaskWidget* taskWidget = _taskWidgets[i];
            QRect originFrame = taskWidget->geometry();
            QRect newFrame = originFrame.translated(0, originFrame.height());
            taskWidget->setGeometry(newFrame);
        }
    }

	//animate = true;
    // bring view from left to right.
    if( animate )
    {
		//qDebug() << "bring task widget from left to right start" << "\n";
        QRect originFrame = widget->geometry();
        QRect newFrame = originFrame.translated(originFrame.width(), 0);
        newFrame.setWidth(_scrollWidget->width());

        PropertyAnimation* animation = new PropertyAnimation(widget, "geometry");
        animation->setDelegate(this);
        animation->setStartValue(originFrame);
        animation->setEndValue(newFrame);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
        animation->setDuration(250);
        animation->setAnimationBlockingMode(PropertyAnimation::Blocking);

		//qDebug() << "before animation: geometry=" << widget->geometry() << "\n";
        animation->start(QAbstractAnimation::DeleteWhenStopped);
		//qDebug() << "after animation: geometry=" << widget->geometry() << "\n";
    }
    else 
	{
        QRect originFrame = widget->geometry();
        QRect newFrame = originFrame.translated(originFrame.width(), 0);
        widget->setGeometry(newFrame);
    }

    this->increaseReadyTaskCount();
}

void MainWindow::removeTaskWidget(TaskWidget* widget, bool animate)
{
    int index = _taskWidgets.indexOf(widget);
    int count = _taskWidgets.count();

    // move away selected view to left.
    if( animate )
    {
        QRect originFrame = widget->geometry();
        QRect newFrame = originFrame.translated(-originFrame.width(), 0);

        PropertyAnimation* animation = new PropertyAnimation(widget, "geometry");
        animation->setStartValue(originFrame);
        animation->setEndValue(newFrame);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
        animation->setDuration(250);
        animation->setAnimationBlockingMode(PropertyAnimation::Blocking);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    // remove widget from scroll area.
    widget->setParent(NULL);
    delete widget;

    // shift remain views to up.
    if( animate ) 
	{
        ParallelAnimationGroup* parallelAnimationGroup = new ParallelAnimationGroup;

        for( int i = index+1; i < count; ++i ) 
		{
            TaskWidget* taskWidget = _taskWidgets[i];

            QRect originFrame = taskWidget->geometry();
            QRect newFrame = originFrame.translated(0, -originFrame.height());

            PropertyAnimation* animation = new PropertyAnimation(taskWidget, "geometry");
            animation->setStartValue(originFrame);
            animation->setEndValue(newFrame);
            animation->setEasingCurve(QEasingCurve::InOutQuad);
            animation->setDuration(250);

            parallelAnimationGroup->addAnimation(animation);
        }

        parallelAnimationGroup->setAnimationBlockingMode(ParallelAnimationGroup::Blocking);
        parallelAnimationGroup->start(QAbstractAnimation::DeleteWhenStopped);
    }

    _taskWidgets.removeAt(index);

    if( count == 1 ) 
	{
        _scrollWidget->setGeometry(_scrollWidget->x(), _scrollWidget->y(), _scrollWidget->width(), 0);
    }
    else 
	{
        TaskWidget* lastWidget = _taskWidgets.last();
        //NOTE:Cocoa's NSHeight and NSWidth differ from QRect::Width() and QRect::Height().
        // NSHeight = QRect::Height()+1
        _scrollWidget->setGeometry(_scrollWidget->x(), _scrollWidget->y(), _scrollWidget->width(), lastWidget->geometry().bottom()+1);
    }

    this->decreaseReadyTaskCount();
}

void MainWindow::editTaskWidget(TaskWidget* widget)
{
    TaskEditWidget editWidget(this);
    //editWidget.setDelegate(this);
    editWidget.setShootTitleDataSource(this);
    editWidget.setFolderURL(widget->folderURL());
    editWidget.setAutoEditing(widget->autoEditing());
    editWidget.setOption(widget->option());
    QVariantList vl = widget->clips();
    editWidget.setClips(vl);
    editWidget.setShootTitleId(widget->shootTitleId());
	editWidget.setMailingCheck(widget->mailingCheck());
	editWidget.setNoteToEditor(widget->noteToEditor());

    _editingWidget = widget;

    editWidget.exec();
//    editWidget.show();
}

void MainWindow::increaseRunningTaskCount()
{
    ++_runningTaskCount;
    this->ui->stopTaskButton->setEnabled(_runningTaskCount>0);
    this->ui->queryButton->setEnabled(_runningTaskCount==0);
}

void MainWindow::decreaseRunningTaskCount()
{
    --_runningTaskCount;
    this->ui->stopTaskButton->setEnabled(_runningTaskCount>0);
    this->ui->queryButton->setEnabled(_runningTaskCount==0);
}

void MainWindow::increaseReadyTaskCount()
{
    ++_readyTaskCount;
    this->ui->runTaskButton->setEnabled(_readyTaskCount>0);
    this->ui->removeTaskButton->setEnabled(_readyTaskCount>0);
}

void MainWindow::decreaseReadyTaskCount()
{
    --_readyTaskCount;
    this->ui->runTaskButton->setEnabled(_readyTaskCount>0);
    this->ui->removeTaskButton->setEnabled(_readyTaskCount>0);
}

void MainWindow::increaseUnstableTaskCount()
{
    ++_unstableTaskCount;
}

void MainWindow::decreaseUnstableTaskCount()
{
    --_unstableTaskCount;
}

void MainWindow::playFinishSoundIfNoRunningTask()
{
    if( _runningTaskCount != 0 )
        return;

//    _mediaObject.play();
    QSound::play(qApp->applicationDirPath() + "/Complete.wav");
}

void MainWindow::setHeaderDescription(QString value)
{
    this->ui->headerDescriptionLabel->setText(value);
}

void MainWindow::setDataModel(QVariantList _picModel, QVariantList _videoModel, QVariantList _audioModel)
{
    if (!_picModel.isEmpty())
        picModel.setMovies(_picModel);
    if (!_videoModel.isEmpty())
        videoModel.setMovies(_videoModel);
    if (!_audioModel.isEmpty())
        audioModel.setMovies(_audioModel);
}

void MainWindow::delTempPath()
{

    QString root = QDir::tempPath();
    qDebug() << QDir::tempPath();
    QDir temp( root );
    QStringList qsl = temp.entryList();
    for( int i = 0; i < qsl.count(); i++) {

        QString delPath = root + "/" + qsl.at(i);
        if ( delPath.contains("HouseLens Uploader", Qt::CaseInsensitive) == true ) {
            QDir dirDel( delPath );
            dirDel.removeRecursively();
        }
        if ( delPath.contains("lnk{", Qt::CaseInsensitive) == true ) {
            QFile::remove( delPath );
        }
    }
}


// - Slot ----------------------------------------------------------------------------------------------

void MainWindow::on_addTaskButton_clicked()
{
//    if ( _videoEditWidget != NULL ) {
//        _videoEditWidget->cancelVideoEditWidget();
//        _videoEditWidget = NULL;
//    }
    TaskEditWidget editWidget(this);
    //editWidget.setDelegate(this);
    editWidget.setShootTitleDataSource(this);
    editWidget.setOption(SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND);
    editWidget.exec();

}

void MainWindow::on_runTaskButton_clicked()
{
    int count = _taskWidgets.count();
    for( int i = count-1; i >= 0; --i ) 
	{
        TaskWidget* taskWidget = _taskWidgets[i];
        if( taskWidget->isReadyState() )
		{
            taskWidget->start_stop();
        }
    }
}

void MainWindow::on_stopTaskButton_clicked()
{
    int count = _taskWidgets.count();
    for( int i = count-1; i >= 0; --i ) 
	{
        TaskWidget* taskWidget = _taskWidgets[i];
        if( taskWidget->isRunningState() ) 
		{
            taskWidget->start_stop();
        }
    }
}

void MainWindow::on_removeTaskButton_clicked()
{
    int count = _taskWidgets.count();
    for( int i = count-1; i >= 0; --i ) 
	{
        TaskWidget* taskWidget = _taskWidgets[i];
        if( taskWidget->isReadyState() ) 
		{
            this->removeTaskWidget(taskWidget, false);
        }
    }
}

void MainWindow::on_settingButton_clicked()
{
    PreferenceDialog preferenceDialog(this);
    preferenceDialog.exec();
}

void MainWindow::on__userNameTextEdit_editingFinished()
{
	Preference::setUsername( this->ui->_userNameTextEdit->text() );
}

void MainWindow::on__passwordTextEdit_editingFinished()
{
	Preference::setPassword( this->ui->_passwordTextEdit->text() );
}

void MainWindow::on_queryButton_clicked()
{
    if( _networkReply != NULL )
        return;

    QString username = ui->_userNameTextEdit->text();
	QString password = ui->_passwordTextEdit->text();

	if( username.isEmpty() || password.isEmpty() )
	{
        qDebug() << "Correct your username and password" << "\n";
        return;
    }

    QString loginUrl = QString("https://vluploadent.myhouselens.com/GridExtendedService.svc/BOS_AuthenticateUser?username=%1&password=%2").arg(username).arg(password);

    QUrl url(loginUrl);

    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");

    _networkReply = _manager.get(request);
    if( _networkReply->error() != QNetworkReply::NoError )
    {
        _networkReply->deleteLater();
        _networkReply = NULL;
        return;
    }

	//ui->queryButton->setEnabled(false);
    connect(_networkReply, SIGNAL(finished()), this, SLOT(readHttpData()));
}

void MainWindow::readHttpData()
{
//    ui->queryButton->setEnabled(true);
    _networkReply->deleteLater();
    qDebug() << "reply finished" << "\n";

//    QByteArray htpData = _networkReply->readAll();
//    QJsonDocument jsDoc = QJsonDocument::fromJson(htpData);
//    QJsonObject jsn = jsDoc.object();

    if( _networkReply->error() != QNetworkReply::NoError )
	{
//        this->setHeaderDescription("Can not connect to WebService");
        this->setHeaderDescription("User does not exist");
        qDebug() << "network error: " << _networkReply->error() << "\n";
//        qDebug() << jsn["Message"].toString();
        _networkReply = NULL;
        return;
    }

    QByteArray httpData = _networkReply->readAll();
    _networkReply = NULL;

    if( httpData.isEmpty() ) 
	{
        this->setHeaderDescription("Web Service doesn't work normally.");
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(httpData);
    if( jsonDoc.isEmpty() )
    {
        this->setHeaderDescription("Web Service doesn't work normally.");
        return;
    }

    QJsonObject json = jsonDoc.object();

    if( json.isEmpty() )
	{
        this->setHeaderDescription("Web Service doesn't work normally.");
        return;
    }

    this->setHeaderDescription(json["Message"].toString());

    //TokenID
    if( !json.contains("TokenID") )
        return;
    QString tokenID = json["TokenID"].toString();
    this->setTokenID(tokenID);

    QString errorMsg("There are no work orders assigned to you at this time.\nPlease email info@video.tv for support.");
    QJsonObject profileJson = json["Profile"].toObject();
    if( profileJson.isEmpty() )
	{
        this->setHeaderDescription(errorMsg);
        return;
    }

    //UserID
    if( !profileJson.contains("UserID") )
	{
        this->setHeaderDescription(errorMsg);
        return;
	}
    QString userID = profileJson["UserID"].toString();
    this->setUserID(userID);

    //Artist Name
    QString name = QString("%1 %2").arg(profileJson["FirstName"].toString()).arg(profileJson["LastName"].toString());
    this->setArtistName(name);

    //Email Address
    if( !profileJson.contains("EmailID") )
    {
        this->setHeaderDescription(errorMsg);
        return;
    }
    QString email = profileJson["EmailID"].toString();
    this->setEmailAddress(email);

    QString workOrderUrl = QString("https://vluploadent.myhouselens.com/GridExtendedService.svc/GetShoots?tokenID=%1&UserID=%2").arg(tokenID).arg(userID);
    QUrl url(workOrderUrl);
    QNetworkRequest httpRequest(url);
    httpRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    _networkReply = _manager.get(httpRequest);

    //----------------------------
//    QByteArray httpData1 = _networkReply->readAll();
//    QJsonDocument jsonDoc1 = QJsonDocument::fromJson(httpData1);
//    QJsonObject json1 = jsonDoc1.object();
//    QString str = json1["Name"].toString();
//    qDebug() << "Name----: " << str;
    //----------------------------
    if( _networkReply->error() != QNetworkReply::NoError )
    {
        _networkReply->deleteLater();
        _networkReply = NULL;
        return;
    }

    //ui->queryButton->setEnabled(false);
    connect(_networkReply, SIGNAL(finished()), this, SLOT(readWorkorderHttpData()));
}

/*
void MainWindow::readServerListHttpData()
{
    _networkReply->deleteLater();

    if( _networkReply->error() != QNetworkReply::NoError )
    {
        this->setHeaderDescription("Can not connect to WebService");
        qDebug() << "network error: " << _networkReply->error() << "\n";
//        qDebug() << jsn["Message"].toString();
        _networkReply = NULL;
        return;
    }

    QByteArray httpData = _networkReply->readAll();
    _networkReply = NULL;

    if( httpData.isEmpty() )
    {
        this->setHeaderDescription("Web Service doesn't work normally.");
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(httpData);
    if( jsonDoc.isEmpty() )
    {
        this->setHeaderDescription("Web Service doesn't work normally.");
        return;
    }

    QJsonObject json = jsonDoc.object();

    if( json.isEmpty() )
    {
        this->setHeaderDescription("Web Service doesn't work normally.");
        return;
    }

    QJsonArray shoots = json["Data"].toArray();

}
*/
void MainWindow::readWorkorderHttpData()
{
    _networkReply->deleteLater();
    qDebug() << "reply finished" << "\n";
    if( _networkReply->error() != QNetworkReply::NoError )
    {
        this->setHeaderDescription("Can not connect to WebService");
        qDebug() << "network error: " << _networkReply->error() << "\n";
        _networkReply = NULL;
        return;
    }

    QByteArray httpData = _networkReply->readAll();
    _networkReply = NULL;

    if( httpData.isEmpty() )
    {
        this->setHeaderDescription("Web Service doesn't work normally.");
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(httpData);
    if( jsonDoc.isEmpty() )
    {
        this->setHeaderDescription("Web Service doesn't work normally.");
        return;
    }

    QJsonObject json = jsonDoc.object();

    if( json.isEmpty() )
    {
        this->setHeaderDescription("Web Service doesn't work normally.");
        return;
    }

    if( !json.contains("Data") )
    {
        this->setHeaderDescription("There are no workorders assigned to you at this time.");
        return;
    }

    QJsonArray shoots = json["Data"].toArray();
    QVariantList shootTitles;
    QVariantList shootIds;

    QVariantList buf_shootTitles;
    QVariantList buf_shootIds;

    for( int i = 0 ; i < shoots.count() ; i++ )
    {
        QJsonObject shootJson = shoots[i].toObject();
        if( shootJson.contains("Id") && shootJson.contains("Name") )
        {
            qDebug() << shootJson["Time"].toString();
            qDebug() << shootJson["EndDate"].toString();
            qDebug() << shootJson["EndTime"].toString();
            qDebug() << shootJson["StartDate"].toString();
            qDebug() << shootJson["StartTime"].toString();

            if( shootJson["Status"] == "Scheduled" ){//Scheduled

                shootTitles << shootJson["Name"].toString();
                shootIds << shootJson["Id"].toString();
            } else {

                buf_shootTitles << shootJson["Name"].toString();
                buf_shootIds << shootJson["Id"].toString();
            }
        }
    }
    for( int i = 0; i < buf_shootTitles.length(); i++ ){

//        qDebug() << "===== " << buf_shootTitles.at(i).toString() << "===" << buf_shootIds.at(i).toString();
        shootTitles << buf_shootTitles.at(i).toString();
        shootIds << buf_shootIds.at(i).toString();
    }
//    for( int i = 0; i < shootIds.length(); i++ ){
//        qDebug() << shootTitles.at(i).toString() << "---> " << shootIds.at(i).toString();
//    }


    // set shoot titles, shoot ids.
    //this->setFtpBaseDirectory(QString("VideoLensNAS/Shoots - Shared/Residential"));
    this->setFtpBaseDirectory(QString("HouseLensEnterpriseUploads/Residential"));
    shootTitles.insert(shootTitles.begin(), NoneTitle);
    this->setShootTitles(shootTitles);
    shootIds.insert(shootIds.begin(), NoneTitle);
    this->setShootIds(shootIds);

    QDateTime currentTime = QDateTime::currentDateTime();
    this->showLastQueryDate(currentTime);
	Preference::setLastQueryDate( currentTime );

// - Call ServerList Slot ------------------------------------------

//     QString serverListUrl = QString("https://vluploadent.myhouselens.com/GridExtendedService.svc/GetServerList");
//     QString serverListUrl = QString("https://testuploadservice.myhouselens.com/GridExtendedService.svc/GetServerList");

//     QUrl url(serverListUrl);

//     QNetworkRequest serverListRequest(url);

//     serverListRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");

//     _networkReply = _manager.get(serverListRequest);

//     connect(_networkReply, SIGNAL(finished()), this, SLOT(readServerListHttpData()));
//___________________________________________________________________

	this->on_addTaskButton_clicked();

    qDebug() << "Artist Name: " << this->artistName() << "\n";
    qDebug() << "Shoot Titles: " << this->shootTitles() << "\n";
    qDebug() << "Shoot Ids: " << this->shootIds() << "\n";


}


// - TaskEditWidgetDelegate ----------------------------------------------------------------------------------------------------

void MainWindow::taskEditWidgetDidEndEditing(QString folderURL, SUBMIT_OPTION option, QVariantList &clips, bool autoEditing,
                                             QString shootTItleId, bool mailingCheck, QString noteToEditor)
{
    TaskWidget* taskWidget;

    if( _editingWidget == NULL ) 
	{
        taskWidget = new TaskWidget();
    }
    else
	{
        taskWidget = _editingWidget;
    }

    taskWidget->setFolderURL( folderURL );
    taskWidget->setOption( option );
    taskWidget->setClips( clips );
    taskWidget->setAutoEditing( autoEditing );
    taskWidget->setShootTitleDataSource(this);
    taskWidget->setShootTitleId( shootTItleId );
    taskWidget->setDelegate(this);
    taskWidget->setMailingCheck( mailingCheck );
    taskWidget->setNoteToEditor( noteToEditor );


    if( _editingWidget == NULL )
        this->addTaskWidget(taskWidget, true);

    _editingWidget = NULL;
}

void MainWindow::taskEditWidgetDidCancel(/*TaskEditWidget* controller*/)
{
    //Q_UNUSED(controller);
    //_editingWidget = NULL;
}


//bool MainWindow::taskEditWidgetCanUseShootTitleId(TaskEditWidget* widget, QString& titleId)
//{
//    Q_UNUSED(widget);

//    int count = _taskWidgets.count();
//    for( int i = 0; i < count; ++i )
//	{
//        TaskWidget* taskWidget = _taskWidgets[i];
//        if( taskWidget->shootTitleId() == titleId )
//		{
//            if( _editingWidget == taskWidget )
//                return true;

//            return false;
//        }
//    }

//    return true;
//}

bool MainWindow::taskEditWidgetCanUseSubmitOption(/*TaskEditWidget* widget,*/ SUBMIT_OPTION option)
{
    //Q_UNUSED(widget);
    if( hasUploadOption(option) && this->ftpBaseDirectory().isEmpty() )
        return false;

    return true;
}

void MainWindow::clearLayout(QLayout *layout)
{
    if (layout) {
        QLayoutItem *item;

        //the key point here is that the layout items are stored inside the layout in a stack
        while((item = layout->takeAt(0)) != 0) {
            if (item->widget()) {
                layout->removeWidget(item->widget());
                delete item->widget();
            }

            delete item;
        }
    }

}

void MainWindow::uploadClips(QString fpath)
{
    if ( fpath.isEmpty() ) {

        // Just Clips

//        if( _delegate != NULL )
//            _delegate->taskEditWidgetDidEndEditing(this);

    } else {

        // All Clips

        QVariantList files;
        Q_FOREACH( QVariant aMovieInfo, updateModel.movies() )
        {
            files.push_back( aMovieInfo );
        }

        QVariantMap aMovieInfo;
        aMovieInfo[AppKey_URL] = fpath;
        aMovieInfo[AppKey_RemoveAudio] = true;
        files.push_back(aMovieInfo);

        updateModel.setMovies(files);

    }

    taskEditWidgetDidEndEditing( fURL, opt, updateModel.movies(), autoEdit, shootID, mailCheck, noteEdit );
}

void MainWindow::videoEditWidgetView(QStringList fileList, bool flag, QVariantList updateFiles,
                                     QString folderURL, SUBMIT_OPTION option, bool autoEditing,
                                     QString shootTItleId, bool mailingCheck, QString noteToEditor)
{

    fURL = folderURL;
    opt = option;
    autoEdit = autoEditing;
    shootID = shootTItleId;
    mailCheck = mailingCheck;
    noteEdit = noteToEditor;

    updateModel.setMovies(updateFiles);

    VideoEditWidget* _videoEditWidget = new VideoEditWidget(this);

    clearLayout(vbox);
    _videoEditWidget->setVideoFileList( fileList, flag );
    vbox->addWidget(_videoEditWidget);

    this->ui->scrollAreaMain->setVisible(false);

    _videoEditWidget->setGeometry( 0, 0, this->ui->groupBoxMain->width(), this->ui->groupBoxMain->height() );
    this->ui->groupBoxMain->setLayout(vbox);

    this->activateWindow();
}

void MainWindow::videoEditWidgetCancelView()
{
    this->ui->scrollAreaMain->setVisible(true);
}

void MainWindow::taskEditWidgetView()
{
    //this->on_addTaskButton_clicked();
    TaskEditWidget editWidget(this);
    //editWidget.setDelegate(this);
    editWidget.setShootTitleDataSource(this);
    editWidget.setDataModel(picModel.movies(), videoModel.movies(), audioModel.movies());
    editWidget.setEnvironment(fURL, opt, autoEdit, mailCheck, shootID, noteEdit);
    editWidget.setOption(SUBMIT_COMPRESS_VIDEO_CONVERT_DNG_PHTHO_SOUND);
    editWidget.exec();
}

// - TaskWidgetDelegate ---------------------------------------------------------------------------------------------------

void MainWindow::taskWidgetRemove(TaskWidget* widget)
{
    this->removeTaskWidget(widget, true);
}

void MainWindow::taskWidgetEdit(TaskWidget* widget)
{
    _editingWidget = widget;

    this->editTaskWidget(widget);
}

void MainWindow::taskWidgetDidBegin(TaskWidget* widget)
{
    Q_UNUSED(widget);
    this->decreaseReadyTaskCount();
    this->increaseRunningTaskCount();
}

void MainWindow::taskWidgetDidFinish(TaskWidget* widget)
{
    Q_UNUSED(widget);
    this->decreaseRunningTaskCount();
    this->increaseReadyTaskCount();

    // beep if all task is done.
    this->playFinishSoundIfNoRunningTask();
}

void MainWindow::taskWidgetDidFail(TaskWidget* widget, Error* error)
{
    Q_UNUSED(widget);
    Q_UNUSED(error);
    this->decreaseRunningTaskCount();
    this->increaseReadyTaskCount();

    this->playFinishSoundIfNoRunningTask();
}

void MainWindow::taskWidgetWasCancelled(TaskWidget* widget)
{
    Q_UNUSED(widget);
    this->decreaseUnstableTaskCount();
    this->increaseReadyTaskCount();
}

void MainWindow::taskWidgetWillBeCancelled(TaskWidget* widget)
{
    Q_UNUSED(widget);
    this->decreaseRunningTaskCount();
    this->increaseUnstableTaskCount();
}

QSize MainWindow::hdm4vSize()
{
	return QSize(Preference::hdVideoWidth(), Preference::hdVideoHeight() );
}

QString MainWindow::ftpPassword()
{
	return Preference::ftpPassword();
}

QString MainWindow::ftpBaseDirectory()
{
    return _ftpBaseDirectory;
}

QString MainWindow::artistName()
{
    return _artistName;
}

bool MainWindow::shouldCompressImageForDPI()
{
	return Preference::shouldCompressImageForDPI();
}

int MainWindow::maxDPI()
{
	return Preference::maxImageDPI();
}

bool MainWindow::shouldCompressImageForSize()
{
	return Preference::shouldCompressImageForSize();
}

QSize MainWindow::maxImageSize()
{
	return QSize( Preference::maxImageWidth(), Preference::maxImageHeight() );
}

// - ScrollAreaDelegate --------------------------------------------------------------------------------------------------------

void MainWindow::scrollAreaDidChangeViewportRect(ScrollArea* scrollArea, QRect rect)
{
    qDebug() << "view port rect changed: " << rect << "\n";
    QWidget* scrollWidget = scrollArea->widget();
    if( scrollWidget != NULL ) 
	{
        QRect geometry = scrollWidget->geometry();
        geometry.setWidth(rect.width());
        scrollWidget->setGeometry(geometry);

        QList<TaskWidget*>::iterator it = _taskWidgets.begin();
        for( QList<TaskWidget*>::iterator it = _taskWidgets.begin(); it != _taskWidgets.end(); ++it ) 
		{
            TaskWidget* taskWidget = *it;
            if( taskWidget->x() >= 0 ) 
			{
                taskWidget->setGeometry(rect.x(), taskWidget->y(), rect.width(), taskWidget->height());
            }
        }
    }
}


// - ShootTitleDataSource -------------------------------------------------------------------------------------------------------

int MainWindow::numberOfShootTitles()
{
    return _shootTitles.count();
}

QString MainWindow::shootTitleAtIndex(int index)
{
    if( index >= _shootTitles.count() )
        index = 0;

    return _shootTitles[index].toString();
}

QString MainWindow::shootTitleIdAtIndex(int index)
{
//    qDebug() << _shootTitles.count();
    if( index >= _shootTitles.count() )
        index = 0;

    return _shootIds[index].toString();
}

QString MainWindow::shootTitleIdAtIndex(QString text)
{
    for( int i = 0; i < _shootTitles.count(); i++ ) {
        if( _shootTitles.at(i).toString() == text ) {
            return _shootIds.at(i).toString();
        }
    }
    return "";
}

QString MainWindow::shootTitleForId(QString& Id)
{
    for( int i = 0; i < _shootIds.count(); ++i ) {
        if( _shootIds[i].toString() == Id )
            return _shootTitles[i].toString();
    }

    return _shootTitles[0].toString();
}

int MainWindow::shootTitleIndexForId(QString& Id)
{
    for( int i = 0; i < _shootIds.count(); ++i ) {
        if( _shootIds[i].toString() == Id )
            return i;
    }

    return 0;
}

QString MainWindow::userTokenId()
{
    return _tokenID;
}

QString MainWindow::userId()
{
    return _userID;
}

QString MainWindow::userID()
{
    return _userID;
}

