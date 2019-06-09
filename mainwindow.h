#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPropertyAnimation>
#include <QNetworkConfigurationManager>
#include <QList>
#include <QStringList>
#include <QTimer>
#include "taskwidget.h"
#include "propertyanimation.h"
#include "sequentialanimationgroup.h"
#include "tasklistcontainer.h"
#include "scrollarea.h"
//#include "taskeditwidget.h"
#include "shoottitledatasource.h"
//#include "videoeditwidget.h"
#include <QMdiArea>
//#include "taskeditwidgetdelegate.h"
#include <QSettings>
#include <QVBoxLayout>
#include "tablemodel.h"

namespace Ui {
class MainWindow;
}

class QNetworkReply;
class PropertyAnimation;
//class TaskEditWidget;
//class TaskEditWidgetDelegate;

class MainWindow : public QMainWindow, public PropertyAnimationDelegate, public TaskWidgetDelegate, public SequentialAnimationGroupDelegate, public ScrollAreaDelegate, /*public TaskEditWidgetDelegate*/ public ShootTitleDataSource
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	
	bool isOnline();
    void initBindingControls();

    QString tokenID();
    void setTokenID(QString& tokenId);
    QString userID();
    void setUserID(QString& userID);
    void setArtistName(QString& name);
    void setEmailAddress(QString& email);
    void setFtpBaseDirectory(QString& directory);
	QVariantList shootTitles();
    void setShootTitles(QVariantList& titles);
    QVariantList shootIds();
    void setShootIds(QVariantList& ids);
    QDateTime lastQueryDate();
	void showLastQueryDate(QDateTime& date);

    void addTaskWidget(TaskWidget* widget, bool animate);
    void removeTaskWidget(TaskWidget* widget, bool animate);
    void editTaskWidget(TaskWidget* widget);

    void increaseRunningTaskCount();
    void decreaseRunningTaskCount();

    void increaseReadyTaskCount();
    void decreaseReadyTaskCount();

    void increaseUnstableTaskCount();
    void decreaseUnstableTaskCount();

    void playFinishSoundIfNoRunningTask();
    void setHeaderDescription(QString value);

    void delTempPath();

private:
    QPropertyAnimation* _myAnimation;
    QString _tokenID;
    QString	_userID;
    QString _artistName;
    QString _emailAddress;
    QString _ftpBaseDirectory;
    QVariantList _shootTitles;
    QVariantList _shootIds;

	//Phonon::MediaObject _mediaObject;
	//Phonon::AudioOutput _audioOutput;

    QList<TaskWidget*> _taskWidgets;
    QNetworkAccessManager _manager;
    QNetworkReply* _networkReply;
	QNetworkConfigurationManager _networkConfigurationManager;
    QWidget* _scrollWidget;

    int _runningTaskCount;
    int _readyTaskCount;
    int _unstableTaskCount;

    TaskWidget* _editingWidget;

    //TaskEditWidget* _taskEditWidget;
//    VideoEditWidget* _videoEditWidget;
//    VideoPlayWidget* _videoPlayWidget;
    QVBoxLayout* vbox;

    TableModel updateModel;
    TableModel picModel, videoModel, audioModel;

    QString fURL;
    SUBMIT_OPTION opt;
    bool autoEdit, mailCheck;
    QString shootID;
    QString noteEdit;


//    QNetworkReply* _networkServerListReply;

private slots:
    void readHttpData();
    void readWorkorderHttpData();
    void on_addTaskButton_clicked();
    void on_runTaskButton_clicked();
    void on_stopTaskButton_clicked();
    void on_removeTaskButton_clicked();
    void on_settingButton_clicked();
    void on_queryButton_clicked();
    void on__userNameTextEdit_editingFinished();
    void on__passwordTextEdit_editingFinished();
	void onlineStateChanged( bool isOnline );

//    void readServerListHttpData();
public:
     void taskEditWidgetDidEndEditing(QString folderURL, SUBMIT_OPTION option, QVariantList &clips, bool autoEditing,
                                      QString shootTItleId, bool mailingCheck, QString noteToEditor);
     void videoEditWidgetView(QStringList fileList, bool flag, QVariantList updateFiles,
                              QString folderURL, SUBMIT_OPTION option, bool autoEditing,
                              QString shootTItleId, bool mailingCheck, QString noteToEditor);

     void taskEditWidgetDidCancel();
     void videoEditWidgetCancelView();
     void taskEditWidgetView();
     bool taskEditWidgetCanUseSubmitOption(/*TaskEditWidget* widget,*/ SUBMIT_OPTION option);

     void clearLayout(QLayout* layout);
     void uploadClips(QString fpath);
     void setDataModel(QVariantList _picModel, QVariantList _videoModel, QVariantList _audioModel);

public:
    // TaskEditWidgetDelegate
//    virtual void videoEditWidgetView(TaskEditWidget* widget);
//    virtual void taskEditWidgetDidEndEditing(TaskEditWidget* widget);
//    virtual void taskEditWidgetDidCancel(TaskEditWidget* controller);
//    virtual bool taskEditWidgetCanUseShootTitleId(TaskEditWidget* widget, QString& titleId);
//    virtual bool taskEditWidgetCanUseSubmitOption(TaskEditWidget* widget, SUBMIT_OPTION option);
//    virtual void videoPlayWidgetView(VideoEditWidget *widget);
    // TaskWidgetDelegate
    virtual void taskWidgetRemove(TaskWidget* widget);
    virtual void taskWidgetEdit(TaskWidget* widget);
    virtual void taskWidgetDidBegin(TaskWidget* widget);
    virtual void taskWidgetDidFinish(TaskWidget* widget);
    virtual void taskWidgetDidFail(TaskWidget* widget, Error* error);
    virtual void taskWidgetWasCancelled(TaskWidget* widget);
    virtual void taskWidgetWillBeCancelled(TaskWidget* widget);
    virtual QSize hdm4vSize();
    virtual QString ftpPassword();
    virtual QString ftpBaseDirectory();
    virtual QString artistName();
	virtual bool shouldCompressImageForDPI();
	virtual int maxDPI();
	virtual bool shouldCompressImageForSize();
	virtual QSize maxImageSize();
    // ScrollAreaDelegate
    virtual void scrollAreaDidChangeViewportRect(ScrollArea*, QRect);
    // ShootTitleDataSource
    virtual int numberOfShootTitles();
    virtual QString shootTitleAtIndex(int);
    virtual QString shootTitleIdAtIndex(int);

    virtual QString shootTitleIdAtIndex(QString text);

    virtual QString shootTitleForId(QString& Id);
    virtual int shootTitleIndexForId(QString& Id);
    virtual QString userTokenId();
    virtual QString userId();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
