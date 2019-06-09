#include "videoeditwidget.h"
#include "ui_videoeditwidget.h"
#include "taskeditwidget.h"
#include "preference.h"
#include "qmediainfo.h"
#include <QFileInfo>
#include <QDebug>
#include <imageviewer.h>
#include <QVideoFrame>
#include <QLayout>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <QScrollArea>
#include <QLabel>
#include <QImage>
#include <QListWidget>
#include <QGridLayout>
#include <QSize>
#include <QThread>
#include <QFileDialog>
#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>

VideoEditWidget::VideoEditWidget(MainWindow *arg1) :
    window(arg1),
//VideoEditWidget::VideoEditWidget(QWidget *parent) :
//    QWidget(parent),
    ui(new Ui::VideoEditWidget)
{
    ui->setupUi(this);
    cutTime = 1;
    nPSteps = 0;

    this->ui->listWidget->setGeometry( 0, 0, 960, 395 );
    flw = new FileListWidget(this->ui->listWidget);
    flw->setGeometry( 0, 0, 960, 395 );

    instance = new VlcInstance(VlcCommon::args(), this);
    vplayer = new VlcMediaPlayer(instance);

    this->setWindowModality(Qt::NonModal);

    this->ui->ldtCutSecond->setText( "1" );

    mergeVideoProcess = new QProcess(this);
    mergeAudioProcess = new QProcess(this);
    cutProcess = new QProcess(this);

    ffmpegPath = qApp->applicationDirPath() + "/ffmpeg/ffmpeg.exe";
    temporaryPath = new QTemporaryDir;
    temporaryPath->autoRemove();
    tempPath = temporaryPath->path() + "/";
    qDebug() << tempPath;
    setMusicCombo();


    connect(cutProcess, SIGNAL(started()), this, SLOT(processCutVideoStarted()) );
    connect(cutProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processCutVideoFinished()));
    connect(cutProcess, SIGNAL(readyReadStandardOutput()),this,SLOT(readOutput()));

    connect(mergeVideoProcess, SIGNAL(started()), this, SLOT(processMergeVideoStarted()) );
    connect(mergeVideoProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processMergeVideoFinished()));
    connect(mergeVideoProcess, SIGNAL(readyReadStandardOutput()),this,SLOT(readOutput()));

    connect(mergeAudioProcess, SIGNAL(started()), this, SLOT(processMergeAudioStarted()) );
    connect(mergeAudioProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processMergeAudioFinished()));
    connect(mergeAudioProcess, SIGNAL(readyReadStandardOutput()),this,SLOT(readOutput()));

    mergeVideoProcess->setProcessChannelMode(QProcess::MergedChannels);
    mergeVideoProcess->setReadChannel (QProcess :: StandardOutput);

    cutProcess->setProcessChannelMode(QProcess::MergedChannels);
    cutProcess->setReadChannel (QProcess :: StandardOutput);

    mergeAudioProcess->setProcessChannelMode(QProcess::MergedChannels);
    mergeAudioProcess->setReadChannel (QProcess :: StandardOutput);
}

VideoEditWidget::~VideoEditWidget()
{
    cutProcess->deleteLater();
    mergeVideoProcess->deleteLater();
    mergeAudioProcess->deleteLater();
//    pd->deleteLater();

    delete cutProcess;
    delete mergeVideoProcess;
    delete mergeAudioProcess;
//    delete pd;

    delete ui;
}

int VideoEditWidget::duration_from_timeString(QString strTime)
{
    int duration;
    int hh, mm, ss;

    hh = strTime.mid(0, 2).toInt();
    mm = strTime.mid(3, 2).toInt();
    ss = strTime.mid(6, 2).toInt();

    duration = hh * 3600 + mm * 60 + ss;
    return duration;
}

QString VideoEditWidget::timeString_from_duration(int dTime)
{
    QString timeString;

    QString hh, mm, ss;

    if ( (dTime/3600) < 10 ) {
        hh = "0" + QString::number( (int)(dTime/3600) );
    } else {
        hh = QString::number( (int)(dTime/3600) );
    }

    if ( ((dTime-hh.toInt()*3600)/60) < 10 ) {
        mm = "0" + QString::number( (int)((dTime-hh.toInt()*3600)/60) );
    } else {
        mm = QString::number( (int)((dTime-hh.toInt()*3600)/60) );
    }

    if ( (dTime-hh.toInt()*3600-mm.toInt()*60) < 10 ) {
        ss = "0" + QString::number( dTime-hh.toInt()*3600-mm.toInt()*60 );
    } else {
        ss = QString::number( dTime-hh.toInt()*3600-mm.toInt()*60 );
    }
    timeString = hh + ":" + mm + ":" + ss;
    return timeString;
}

bool VideoEditWidget::_cutVideo(int cTime)
{
    processStatus = false;
    QMediaInfo mi;

    cutVideoForVideoEdit.append( videoForVideoEdit.at(0) );


    for( cutCount = 1; cutCount < flw->count() - 1; cutCount++ ){

        QStringList arg;
        QFileInfo fi( /*videoForVideoEdit.at(cutCount)*/ flw->item(cutCount)->whatsThis() );

        mi.Open( /*videoForVideoEdit.at(cutCount)*/ flw->item(cutCount)->whatsThis() );

        cTime = flw->item(cutCount)->statusTip().toInt();
        arg << "-ss" << (QString)timeString_from_duration( cTime );
        qDebug() << (QString)timeString_from_duration( cTime );
        int cutEndTime = duration_from_timeString( mi.Duration() ) - cTime -1;
        arg << "-to" << (QString)timeString_from_duration( cutEndTime );
        qDebug() << (QString)timeString_from_duration( cutEndTime );

        QString fpath = /*videoForVideoEdit.at(cutCount)*/flw->item(cutCount)->whatsThis();
        arg << "-i" << fpath;

//        arg1 << "-async" << QString::number(1) << "-strict" << "-2";
        QString fPath = tempPath +fi.fileName();
        arg << (QString)fPath;

        cutProcess->start( ffmpegPath, arg );

        processStatus = cutProcess->waitForFinished(-1);
        cutProcess->close();

        cutVideoForVideoEdit.append( tempPath + fi.fileName() );
    }
//    this->ui->progressRender->setVisible(false);
    cutVideoForVideoEdit.append( videoForVideoEdit.at( videoForVideoEdit.count()-1));

    for(int i = 0; i < cutVideoForVideoEdit.count(); i++ )
        qDebug() << "+++++++++ " << cutVideoForVideoEdit.at(i);

    return processStatus;

}

bool VideoEditWidget::_mergeVideo()
{
    qDebug() << "merge video start!";
    processStatus = false;
    QStringList arg;
    QString renderSize = "1280x720";
    QString color_size = "color=black:";
    int fadeTime = 4;
    QMediaInfo mi;
    QString endParm;

    arg << "-y";

    for( int i = 0; i < cutVideoForVideoEdit.count(); i++ ) {

        arg << "-i";
        QString fpath = cutVideoForVideoEdit.at(i);
        qDebug() << "filePath-- :" << fpath;
//        QString fname = fi.fileName();

        arg << fpath;
    }
    arg << "-f" << "lavfi" << "-i" << (QString)color_size << "-filter_complex";

    mi.Open( cutVideoForVideoEdit.at(0) );

    pTime = 0;
    pTime = duration_from_timeString( mi.Duration() ) -2;
    endParm =  "[0:v]scale=" + renderSize + ",format=pix_fmts=yuva420p,fade=t=out:st=" + QString::number(pTime + 1) + ":d=1:alpha=1,setpts=PTS-STARTPTS[v0];";
//    arg << (QString)endParm;

    for( int i = 1; i < cutVideoForVideoEdit.count(); i++ ) {

        int vTime;

        mi.Open( cutVideoForVideoEdit.at(i) );
        vTime = duration_from_timeString( mi.Duration() ) - 2;

        QString pram = "[" + QString::number(i) + ":v]";
        pram += "scale=" + renderSize;
        pram += ",format=pix_fmts=yuva420p,fade=t=in:st=0:d=" + QString::number(fadeTime) + ":";
        pram += "alpha=1,fade=t=out:st=" + QString::number( vTime + 1 ) + ":";
        pram += "d=1:alpha=0,setpts=PTS-STARTPTS+" + QString::number(pTime) + "/TB[v" + QString::number(i) + "];";
        pTime += vTime;

        endParm += pram;
//        arg << (QString)pram;
    }

    endParm += "[" + QString::number(cutVideoForVideoEdit.count()) + ":v]scale=" + renderSize + ",trim=duration=1[over]; ";
//    arg << (QString)endParm;

    for( int i = 1; i < cutVideoForVideoEdit.count(); i++ ) {

        if ( i == 1 ) {

            endParm += "[over]";
        } else {

            endParm += "[over" + QString::number(i-1) + "]";
        }
        endParm += "[v" + QString::number(i-1) + "]overlay[over" + QString::number(i) + "];";

//        arg << (QString)endParm;
    }

    endParm += "[over" + QString::number(cutVideoForVideoEdit.count()-1) + "]";
    endParm += "[v" + QString::number(cutVideoForVideoEdit.count()-1) + "]overlay=format=yuv420[outv]";

    qDebug() << "-- endParm -- " << endParm;
    arg << (QString)endParm;
    arg << "-vcodec" << "libx264" << "-map" << "[outv]" << "-b:v" << "1500k" << "-aspect" << "16:9";

    QFileInfo fi(outCompletFilePath);
    outMergeVideoPath = tempPath + fi.baseName() + fi.completeSuffix() + "." + fi.completeSuffix();

    arg << outMergeVideoPath;

    mergeVideoProcess->start( ffmpegPath, arg );

    processStatus = mergeVideoProcess->waitForFinished(-1);
    mergeVideoProcess->close();

    for( int i = 1; i < cutVideoForVideoEdit.count() - 1; i++ ) {

        QFile::remove( cutVideoForVideoEdit.at(i) );
    }

    qDebug() << "close merge video!";
    return processStatus;
}

bool VideoEditWidget::_mergeAudioToVideo()
{
    qDebug() << "merge audio start!";

    QString cutAudioPath;
    int audioDuration;
    int videoDuration;


    if ( QFile::exists( outCompletFilePath ) )
        QFile::remove( outCompletFilePath );

    if ( this->ui->cmboMusic->currentText() == "" ) {

        musicFilename = "";
        processStatus = false;
        processStatus = QFile::copy( outMergeVideoPath, outCompletFilePath );


    } else {

        musicFilename = qApp->applicationDirPath() + "/Music/" + this->ui->cmboMusic->currentText();

        QMediaInfo mi;
        mi.Open( musicFilename );
        audioDuration = duration_from_timeString( mi.Duration() );

        mi.Open( outMergeVideoPath );
        videoDuration = duration_from_timeString( mi.Duration() );

        qDebug() << "audioDuration: -- " << audioDuration;
        qDebug() << "videoDuration: -- " << videoDuration;

        if ( audioDuration > videoDuration ) {

            QStringList arg1;

            arg1 << "-ss" << QString::number(0);
            arg1 << "-to" << QString::number( videoDuration );

            arg1 << "-i" << musicFilename;

    //        arg1 << "-async" << QString::number(1) << "-strict" << "-2";
            QFileInfo fi( musicFilename );

            cutAudioPath = tempPath + fi.baseName() + fi.completeSuffix() + "." + fi.completeSuffix();

            arg1 << (QString)cutAudioPath;

            renderStatus = VideoEditWidget::cutAudio;
            cutProcess->start( ffmpegPath, arg1 );

            processStatus = cutProcess->waitForFinished(-1);
            qDebug() << "cut Audio: " << processStatus;
            cutProcess->close();

        } else {
            cutAudioPath = musicFilename;
        }
        QStringList arg;

        arg << "-i" << outMergeVideoPath;
        arg << "-i" << cutAudioPath;
        arg << "-c:v" << "copy" << "-c:a" << "aac" << "-strict" << "experimental";
        arg << "-map" << "0:v:0" << "-map" << "1:a:0";
        arg << outCompletFilePath;

        processStatus = false;

        renderStatus = VideoEditWidget::mergeAudioToVideo;

        mergeAudioProcess->start( ffmpegPath, arg );
        processStatus = mergeAudioProcess->waitForFinished(-1);
        qDebug() << "@@@ " << processStatus;
        mergeAudioProcess->close();

        if ( audioDuration > videoDuration )
            QFile::remove( cutAudioPath );
    }
    QFile::remove( outMergeVideoPath );

    qDebug() << "close merge audio!";

    return processStatus;
}

void VideoEditWidget::setParentTaskEditWidget(TaskEditWidget *par)
{
    parentTaskEditWidget = par;
}

TaskEditWidget *VideoEditWidget::getParentTaskEditWidget()
{
    return parentTaskEditWidget;
}

void VideoEditWidget::setMusicCombo()
{
    QStringList lstMusic;

    QString appPath = qApp->applicationDirPath() + "/Music";

    QStringList extensions = TaskEditWidget::fileExtensionsForOption( SUBMIT_COMPRESS_AUDIO );
    lstMusic = TaskEditWidget::fileURLsAtDirectoryPath(appPath, extensions);

    this->ui->cmboMusic->clear();
    this->ui->cmboMusic->addItem( "" );

    if ( lstMusic.isEmpty() )
        return;

    for ( int i = 0; i < lstMusic.length(); i++ ) {
        QFileInfo fi( lstMusic.at(i) );
        this->ui->cmboMusic->addItem( fi.fileName() );
    }
}

void VideoEditWidget::setVideoFileList(QStringList lst, bool flag)
{
    videoForVideoEdit.clear();
    videoForVideoEdit.append( qApp->applicationDirPath() + "/BumperVideo/START BUMPER.mov");

    for ( int i = 0; i < lst.count(); i++ ) {
        videoForVideoEdit.append( lst.at(i) );
    }
    videoForVideoEdit.append( qApp->applicationDirPath() + "/BumperVideo/END BUMPER.mov");

    if ( !flag )
        this->ui->cmboMusic->setEnabled(false);

    viewVideoList();
}

void VideoEditWidget::viewVideoList()
{
    QString videoFilename;
//    layout = new QGridLayout;
//    layout->setHorizontalSpacing( 15 );
//    layout->setVerticalSpacing(15);
//    layout->setColumnStretch( 4,  10);
//    layout->setAlignment(Qt::AlignTop & Qt::AlignLeft);

//    this->ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

//    this->ui->scrollArea->setLayout(layout);

    QProgressDialog* ppd = new QProgressDialog("Loading Video...", "Cancel", 0,  videoForVideoEdit.count(), this);
    ppd->setModal(true);

    for( int i = 0; i < videoForVideoEdit.count(); i++ ){

        videoFilename = videoForVideoEdit.at(i);
        qDebug() << "videoFilename: " << videoFilename;

        ppd->setValue(i);

        ppd->show();

        if ( ppd->wasCanceled() )
            break;

        QFileInfo fi( videoFilename );

        QString thumbPath = tempPath + fi.baseName() + fi.completeSuffix() + ".jpg";

        QStringList arg;
        arg << "-i";
        arg << videoFilename;
        arg << "-an" << "-ss" << "2" << "-qscale:v" << "2";
        arg << thumbPath;

        QProcess* thumbProcess = new QProcess(this);
        thumbProcess->start( ffmpegPath, arg );
        thumbProcess->waitForFinished(-1);
        thumbProcess->close();

        QListWidgetItem *element = new QListWidgetItem();
        element->setIcon(QIcon(thumbPath));
        element->setWhatsThis(videoFilename);
        element->setStatusTip("1");

        flw->setIconSize(QSize( 150, 100 ));

        flw->addItem( element );

        QFile::remove( thumbPath );

    }
    flw->setLineEdit( this->ui->ldtCutSecond );
    ppd->deleteLater();
    delete ppd;
}

void VideoEditWidget::UploadClips(bool b_allVideo)
{
    if ( b_allVideo ) {

        // Upload All Video Clips
        window->uploadClips( outCompletFilePath );
        this->setVisible(false);

    } else {

        // Upload Just Video Clips
        window->uploadClips( "" );
        this->setVisible(false);
    }
}

void VideoEditWidget::processClose()
{
//    if ( pd->wasCanceled() ) {
        cutProcess->close();
        mergeVideoProcess->close();
        mergeAudioProcess->close();
        qDebug() << " processclose Click Cancel------";

//        pdCancel = true;
        //    }
}

int VideoEditWidget::isValue()
{
    return b_value;
}

void VideoEditWidget::setValue(int flag)
{
    b_value = flag;
}

void VideoEditWidget::setTaskEditWidgetDelegate(TaskEditWidgetDelegate *delegate)
{
    _taskEditDelegate = delegate;
}

QString VideoEditWidget::getPlayFilePath()
{
    return outCompletFilePath;
}

void VideoEditWidget::cancelVideoEditWidget()
{
    this->on_btnCancel_clicked();
}

void VideoEditWidget::setPlayFilePath(QString fPath)
{
    playFilePath = fPath;

    vplayer->setVideoWidget(ui->vlcPlayer);
    ui->vlcPlayer->setMediaPlayer( vplayer );
    ui->vlcSeek->setMediaPlayer( vplayer );

    VlcMedia* _media = new VlcMedia(playFilePath, true, instance);
    vplayer->open( _media );
}

QLineEdit *VideoEditWidget::getCutSecondLabel()
{
    qDebug() << "getCutSecondLabel";
    return this->ui->ldtCutSecond;
}

void VideoEditWidget::on_btnProceed_clicked()
{
    bool status;

    outCompletFilePath = tempPath + " integrated_video_clip_" +
            QDate::currentDate().toString( "yyyyMMdd" ) + "_" +
            QTime::currentTime().toString( "hhmmss" ) + ".mov";

    pd = new QProgressDialog("Render in progress.", "Cancel", 0, 100, this);
    connect(pd, SIGNAL(canceled()), this, SLOT(processClose()));

    pd->setModal(true);
    pd->show();

    cutVideoForVideoEdit.clear();

    if ( cutTime != 0 ) {
        renderStatus = VideoEditWidget::cutVideo;
        status = _cutVideo( cutTime );

        if ( !status ) {
            QMessageBox::information(this, tr("Warring!"),tr("Cut Video Fail!") );
            return;
        }
    } else {

        cutVideoForVideoEdit.clear();
        for( int i = 0; i < videoForVideoEdit.count(); i++ )
            cutVideoForVideoEdit.append( videoForVideoEdit.at(i) );
    }

    renderStatus = VideoEditWidget::mergeVideo;
    status = _mergeVideo();
    if ( !status ) {

        QMessageBox::information(this, tr("Warring!"),tr("Merge Video Fail!") );
        return;
    }

    status = _mergeAudioToVideo();
    if ( !status ) {

        QMessageBox::information(this, tr("Warring!"),tr("Merge Audio to Video") );
        return;
    }

    delete pd;

    if ( pdCancel )
        return;

    this->ui->widgetEidt->setGeometry( 0, 1080, 960, 470);
    this->ui->widgetPlay->setGeometry( 0, 0, 960, 470 );
    this->setPlayFilePath( outCompletFilePath );

}

void VideoEditWidget::on_btnCancel_clicked()
{
    this->close();
    window->videoEditWidgetCancelView();
}

void VideoEditWidget::on_cmboMusic_currentTextChanged(const QString &arg1)
{

    if ( arg1.isEmpty() ) {

        musicForVideoEdit = "";
    } else {

        musicForVideoEdit = qApp->applicationDirPath() + "/Music/" + arg1;
    }
}


void VideoEditWidget::on_ldtCutSecond_textChanged(const QString &arg1)
{
    cutTime = arg1.toInt();
    if ( cutTime == 0 ) {
        this->ui->ldtCutSecond->setText("0");
    }
    flw->setRemoveTime( this->ui->ldtCutSecond->text() );
}

void VideoEditWidget::processCutVideoFinished()
{
    renderStatus = VideoEditWidget::none;
    qDebug() << "process Cut Finished";
}

void VideoEditWidget::processMergeVideoStarted()
{
    qDebug() << "process Merge Video Started";
}

void VideoEditWidget::processMergeVideoFinished()
{
    renderStatus = VideoEditWidget::none;
    qDebug() << "process Merge Video Finished";
}

void VideoEditWidget::processMergeAudioStarted()
{
    qDebug() << "process Merge Audio Started";
}

void VideoEditWidget::processMergeAudioFinished()
{
    renderStatus = VideoEditWidget::none;
    qDebug() << "process Merge Audio Finished";
}

void VideoEditWidget::readOutput()
{
        pdCancel = false;
        if ( pd->wasCanceled() ) {
            cutProcess->close();
            cutProcess->kill();
            mergeVideoProcess->close();
            mergeVideoProcess->kill();
            mergeAudioProcess->close();
            mergeAudioProcess->kill();
            qDebug() << " Click Cancel------";
            nPSteps = 0;

            pdCancel = true;
        }
    qDebug()<<"-- Read --";
    QByteArray outData;

    if( renderStatus == VideoEditWidget::cutVideo )
        outData = cutProcess->readAllStandardOutput();

    if( renderStatus == VideoEditWidget::mergeVideo )
        outData = mergeVideoProcess->readAllStandardOutput();

    if( renderStatus == VideoEditWidget::mergeAudioToVideo )
        outData = mergeAudioProcess->readAllStandardOutput();

    qDebug()<< outData;

    perform();
}

void VideoEditWidget::perform()
{

    pd->setLabelText( "Rendering video..." );

    qDebug () << renderStatus;
    if ( renderStatus == VideoEditWidget::cutVideo ) {

        if ( (nPSteps * 100 / pd->maximum()) > 20 ) {

        } else {
            pd->setValue(nPSteps);
            nPSteps++;
            pd->setModal(true);
            pd->show();
        }

        qDebug() << "Cut Video per: " << nPSteps * 100 / pd->maximum();
    }

    if ( renderStatus == VideoEditWidget::mergeVideo ) {

//        QFileInfo fi( outMergeVideoPath );
//        qint64 ksize = fi.size() / 1000;
//        qDebug() << "ksize: " << ksize;

//        pd->setValue( ksize * pd->maximum() / ( pTime * 190 ) );
//        pd->setModal(true);

//        qDebug() << "Perform" << callPerform << " " << nPSteps << pd->value()*100/pd->maximum();

        if ( (nPSteps * 100 / pd->maximum()) > 95 ) {
            pd->show();
        } else {
            pd->setValue(nPSteps /*+ pd->maximum()/100 * 20 */);
            nPSteps++;
            pd->setModal(true);
            pd->show();
        }
        qDebug() << "Merge Video per: " << nPSteps * 100 / pd->maximum();
    }

    if ( renderStatus == VideoEditWidget::cutAudio ) {

        if ( (nPSteps * 100 / pd->maximum() ) > 99 ) {
            pd->show();
        } else {
            pd->setValue(nPSteps /*+ pd->maximum()/100 * 95*/);
            nPSteps++;
            pd->setModal(true);
            pd->show();
        }

        qDebug() << "Cut audio per: " << nPSteps * 100 / pd->maximum();
    }
    if ( renderStatus == VideoEditWidget::mergeAudioToVideo ) {
        pd->show();
    }

    pd->setModal(true);
//    pd->show();
}

void VideoEditWidget::processError()
{

}

void VideoEditWidget::onMediaPlayerStop(QPixmap pix)
{
    qDebug() << "onMediaPlayerStop pix: " << pix;
}


void VideoEditWidget::processCutVideoStarted()
{
    qDebug() << "process Cut Started";
}

//void VideoEditWidget::on_btnSave_clicked()
//{

//}

void VideoEditWidget::on_btnPlayWidgetCancel_clicked()
{
    vplayer->stop();

    this->ui->widgetEidt->setGeometry( 0, 0, 960, 470 );
    this->ui->widgetPlay->setGeometry( 0, 1080, 960, 470 );
}

void VideoEditWidget::on_btnReEdit_clicked()
{
    vplayer->stop();

    window->videoEditWidgetCancelView();
    window->taskEditWidgetView();
//    this->close();
}

void VideoEditWidget::on_btnPlay_clicked()
{
    if ( vplayer->state() == Vlc::Ended ) {
        vplayer->stop();
    }

    if ( vplayer->state() != Vlc::Playing ) {
        qDebug() << "PlayerStatus: " << vplayer->state();
        vplayer->play();
    }
}

void VideoEditWidget::on_btnStop_clicked()
{
    vplayer->pause();
}

void VideoEditWidget::on_btnUploadOnlyVideo_clicked()
{
    vplayer->stop();
    window->videoEditWidgetCancelView();
    this->UploadClips( false );
    this->close();
}

void VideoEditWidget::on_btnUploadAllVideo_clicked()
{
    vplayer->stop();
    window->videoEditWidgetCancelView();
    this->UploadClips( true );
    this->close();
}

void VideoEditWidget::on_listWidget_itemActivated(QListWidgetItem *item)
{
}

void VideoEditWidget::on_listWidget_clicked(const QModelIndex &index)
{
    qDebug() << "click: ";
//    qDebug() << "mousePressEvent: " << item->statusTip();
//    qDebug() << "mousePressEvent: " << item->whatsThis();
}
