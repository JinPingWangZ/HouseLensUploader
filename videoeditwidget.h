#ifndef VIDEOEDITWIDGET_H
#define VIDEOEDITWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QImage>
#include <QStringList>
#include <VLCQtWidgets/WidgetSeek.h>
#include <VLCQtWidgets/WidgetVideo.h>
#include <VLCQtCore/Enums.h>
#include <VLCQtCore/VideoDelegate.h>
#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <QProcess>
#include <QTimer>
#include <QProgressDialog>
#include <QMediaPlayer>
#include <QVideoWidget>
#include "videowidgetsurface.h"
#include <QGridLayout>
#include "taskeditwidget.h"
#include <QTemporaryDir>
#include <QDate>
#include <QTime>
#include <QWidget>
#include "taskeditwidgetdelegate.h"
#include "qmediainfo.h"
#include <QMediaPlayer>
#include <QVideoWidget>
#include "filelistwidget.h"

#include "mainwindow.h"

namespace Ui {
class VideoEditWidget;
}

class TaskEditWidget;
class TaskEditWidgetDelegate;

class VideoEditWidget : public QWidget
{
    Q_OBJECT

public:
//    explicit VideoEditWidget(QWidget *parent = 0);
    explicit VideoEditWidget(MainWindow *arg1);
    ~VideoEditWidget();
    static int duration_from_timeString( QString strTime );
    static QString timeString_from_duration( int dTime );
    bool _cutVideo( int cTime );
    bool _mergeVideo();
    bool _mergeAudioToVideo();

    void setParentTaskEditWidget( TaskEditWidget* par );
    TaskEditWidget* getParentTaskEditWidget();

    enum ProcessStatus{
        none,
        cutVideo,
        mergeVideo,
        cutAudio,
        mergeAudioToVideo
    };

Q_SIGNALS:
    void fnClearPixmap();

private slots:
    void on_btnProceed_clicked();

    void on_btnCancel_clicked();

    void on_cmboMusic_currentTextChanged(const QString &arg1);

    void on_ldtCutSecond_textChanged(const QString &arg1);

    void processCutVideoStarted();
    void processCutVideoFinished();

    void processMergeVideoStarted();
    void processMergeVideoFinished();

    void processMergeAudioStarted();
    void processMergeAudioFinished();

    void readOutput();
    void perform();
    void processError();

    void onMediaPlayerStop(QPixmap pix);

//    void on_btnSave_clicked();

    void on_btnPlayWidgetCancel_clicked();

    void on_btnReEdit_clicked();

    void on_btnPlay_clicked();

    void on_btnStop_clicked();

    void on_btnUploadOnlyVideo_clicked();

    void on_btnUploadAllVideo_clicked();

    void on_listWidget_itemActivated(QListWidgetItem *item);

    void on_listWidget_clicked(const QModelIndex &index);

public:
    void setMusicCombo();
    void setVideoFileList(QStringList lst, bool flag );
    void viewVideoList();
    void UploadClips( bool b_allVideo );
    void processClose();

    int isValue();
    void setValue( int flag );
    void setTaskEditWidgetDelegate( TaskEditWidgetDelegate* delegate );
    QString getPlayFilePath();

    void cancelVideoEditWidget();

    void setPlayFilePath( QString fPath );

    QLineEdit* getCutSecondLabel();

private:
    QString musicForVideoEdit;
    QString ffmpegPath;
    QString tempPath;
    QString outCompletFilePath;
    QString outMergeVideoPath;
//    QString callPerform;

    QStringList videoForVideoEdit;
    QStringList cutVideoForVideoEdit;
    int cutTime;
    QProcess* mergeVideoProcess;
    QProcess* mergeAudioProcess;
    QProcess* cutProcess;

    QProgressDialog* pdCut;
    QProgressDialog* pdMergeVideo;
    QProgressDialog* pdMergeAudio;

    QProgressDialog* pd;

    QTimer* cutTimer;
    QTimer* mergeVideoTimer;
    QTimer* mergeAudioTimer;

    int nPSteps;
    QString musicFilename;
    QString mOutputString;

    bool processStatus;
    QTemporaryDir* temporaryPath;

//    QMediaPlayer* mplayer;
    QVideoWidget* videoView;
    myQAbstractVideoSurface* vsurface;
    QPixmap imgCapture;
    QGridLayout* layout;

    TaskEditWidget* parentTaskEditWidget;

    int pTime;
    bool pdCancel;
    int cutCount;
    int renderStatus;

    bool b_value;

    TaskEditWidgetDelegate* _taskEditDelegate;

    QString playFilePath;

    VlcMediaPlayer* vplayer;
    VlcWidgetVideo* vlcWidet;
    VlcInstance* instance;
    VlcWidgetSeek* vlcSeek;

    QMediaPlayer* player;
    QVideoWidget* vw;

    FileListWidget* flw;

private:
    Ui::VideoEditWidget *ui;
    MainWindow *window;
};



#endif // VIDEOEDITWIDGET_H
