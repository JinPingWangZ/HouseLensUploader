#ifndef TASKWIDGET_H
#define TASKWIDGET_H
#include <Error.h>
#include <QWidget>
#include "ApplicationDefine.h"
#include <QVariantList>
#include <QTemporaryDir>
#include "movieconverter.h"
#include "filecopier.h"
#include "zipper.h"
#include "ftpuploader.h"
#include "ftpuploadermanager.h"
#include "ImageCompressor.h"
#include "webservicenotifier.h"
#include "dngconverter.h"
#include "Error.h"


namespace Ui {
class TaskWidget;
}

class TaskWidget;

class TaskWidgetDelegate;
class ShootTitleDataSource;
class TaskWidget : public QWidget, public MovieConverterDelegate, public FileCopierDelegate, public ZipperDelegate, public FTPUploaderDelegate, public WebServiceNotifierDelegate, public ImageCompressorDelegate, public DNGConverterDelegate
{
    Q_OBJECT
public:
    enum TaskState
    {
        READY = 0,
        RUNNING,
        DONE,
        FAIL,
    };

// Property {
    Q_PROPERTY(TaskWidgetDelegate* delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(QString shootTitleId READ shootTitleId WRITE setShootTitleId)

    Q_PROPERTY(bool running READ running WRITE setRunning)
    Q_PROPERTY(bool shouldRun READ shouldRun WRITE setShouldRun)
    Q_PROPERTY(SUBMIT_OPTION option READ option WRITE setOption)
    Q_PROPERTY(bool autoEditing READ autoEditing WRITE setAutoEditing)
    Q_PROPERTY(ShootTitleDataSource* shootTitleDataSource READ shootTitleDataSource WRITE setShootTitleDataSource)
    Q_PROPERTY(QString folderURL READ folderURL WRITE setFolderURL)
    Q_PROPERTY(QVariantList clips READ clips WRITE setClips)

	Q_PROPERTY(bool mailingCheck READ mailingCheck WRITE setMailingCheck)
	Q_PROPERTY(QString noteToEditor READ noteToEditor WRITE setNoteToEditor)

public:
    void setDelegate(TaskWidgetDelegate* delegate) { _delegate = delegate; }
    TaskWidgetDelegate* delegate() { return _delegate; }
    void setOption(SUBMIT_OPTION option);
    SUBMIT_OPTION option();
    void setFolderURL(QString& url);
    QString folderURL();
    void setClips(QVariantList& list);
    QVariantList clips();
    void setShootTitleId(QString& Id);
    QString shootTitleId();
    QString shootTitle();
    void setAutoEditing(bool flag);
    bool autoEditing();
    void setShootTitleDataSource(ShootTitleDataSource* value);
    ShootTitleDataSource* shootTitleDataSource();
	
	bool mailingCheck();
	void setMailingCheck(bool flag);
	QString noteToEditor();
	void setNoteToEditor(QString & note);

private:
    void setRunning(bool flag);
    bool running();
    void setShouldRun(bool flag);
    bool shouldRun();
private:
    TaskWidgetDelegate* _delegate;
    QString _shootTitleId;
    bool _shouldRun;
    bool _running;
    SUBMIT_OPTION _option;
    ShootTitleDataSource* _shootTitleDataSource;
    QString _folderURL;
    QVariantList _clips;
	bool _autoEditing;

	//Added by Myongsok Kim on July 2013
	//Hold file-paths uploading in
	QStringList		myUploadingFiles;
    QTemporaryDir * pMyTempoaryDir;

    bool			_mailingCheck;
	QString			_noteToEditor;

// } Property


public:
    explicit TaskWidget( QWidget *parent = 0 );
    ~TaskWidget();
    bool isRunningState();
    bool isReadyState();
    QString zipName();
    void start_stop();
private:
    void setState( TaskWidget::TaskState state );
    void processMedias();
    void orderMedias();
    void orderMovies();
    void orderImages();
    void orderSounds();
    void orderDNGs();
    bool isMovie( const QString& url );
    bool isImage( const QString& url );
    bool isSound( const QString& url );
    bool isDNG( const QString& url );
    bool hasMovies();
    bool hasImages();
    bool hasSounds();
    bool hasDNGs();
    void compressImages();
    void convertMovies();
    void convertDNGs();

    void createAutoeditTextFileAtDirectory();
    QString autoeditTextFileURL();
    void deleteAutoeditTextFileAtDirectory();
    QString ftpDirectory();

	//Added by Myongsok Kim on July 2013
	int numberOfPicsFromFiles( QStringList & files );
	int numberOfVideosFromFiles( QStringList & files );

private:
    // MovieConverterDelegate
    virtual bool movieConverterShouldBeginConverting(MovieConverter* converter);
    virtual void movieConverterDidBeginConverting(MovieConverter* converter);
    virtual void movieConverterWasCancelled(MovieConverter* converter);
    virtual void movieConverterDidProgressToPercent(MovieConverter* converter, float percent);    // percent = [1, 100]
    virtual void movieConverterDidFailWithError(MovieConverter* converter, Error* error);
    virtual void movieConverterDidFinishConvert(MovieConverter* converter);
    // FileCopierDelegate
    virtual bool fileCopierShouldBeginCopy(FileCopier* fileCopier);
    virtual void fileCopierDidBeginCopy(FileCopier* fileCopier);
    virtual void fileCopierWasCancelled(FileCopier* fileCopier);
    virtual void fileCopierDidFailWithError(FileCopier* fileCopier, Error* error);
    virtual void fileCopierDidFinishCopy(FileCopier* fileCopier);
    // ZipperDelegate
    virtual bool zipperShouldBeginZip(Zipper* zipper);
    virtual void zipperDidBeginZip(Zipper* zipper);
    virtual void zipperWasCancelled(Zipper* zipper);
    virtual void zipperDidFinishZip(Zipper* zipper);
    virtual void zipperDidFailWithError(Zipper* zipper, Error* error);
    // FTPUploaderDelegate
    virtual void FTPUploaderDidConnect(FTPUploader* uploader);
    virtual bool FTPUploaderShouldBeginUploading(FTPUploader* uploader);
    virtual void FTPUploaderDidBeginUploading(FTPUploader* uploader);
    virtual void FTPUploaderWasCancelled(FTPUploader* uploader);
    virtual void FTPUploaderDidFinishUpload(FTPUploader* uploader);
    virtual void FTPUploaderDidFailWithError(FTPUploader* uploader, Error* error);
    virtual void FTPUploaderDidProgressToPercent(FTPUploader* uploader, int percent);    // percent = [1, 100]
	virtual void FTPUploaderIsWaitingNetworkAvailable(FTPUploader* uploader);
    // WebServiceNotifierDelegate
    virtual bool webServiceNotifierShouldBeginNotify(WebServiceNotifier* notifier);
    virtual void webServiceNotifierDidBeginNotify(WebServiceNotifier* notifier);
    virtual void webServiceNotifierWasCancelled(WebServiceNotifier* notifier);
    virtual void webServiceNotifierDidFailWithError(WebServiceNotifier* notifier, Error* error);
    virtual void webServiceNotifierDidProgressToPercent(WebServiceNotifier* notifier, int percent);    // percent = [1, 100]
    virtual void webServiceNotifierDidFinishNotificationBeforeUploading(WebServiceNotifier *notifier);
    virtual void webServiceNotifierDidFinishNotificationAfterUploading(WebServiceNotifier *notifier);
	// ImageCompressorDelegate
    virtual bool imageCompressorShouldBegin(ImageCompressor* imageCompressor);
    virtual void imageCompressorDidBegin(ImageCompressor* imageCompressor);
    virtual void imageCompressorWasCancelled(ImageCompressor* imageCompressor);
    virtual void imageCompressorDidFailWithError(ImageCompressor* imageCompressor, Error* error);
    virtual void imageCompressorDidFinish(ImageCompressor* imageCompressor);
    // DNGConverterDelegate
    virtual bool dngConverterShouldBeginConverting(DNGConverter* converter);
    virtual void dngConverterDidBeginConverting(DNGConverter* converter);
    virtual void dngConverterWasCancelled(DNGConverter* converter);
    virtual bool dngConverterShouldContinueProcessing(DNGConverter* converter);
    virtual void dngConverterDidProgressToPercent(DNGConverter* converter, int percent);
    virtual void dngConverterDidFailWithError(DNGConverter* converter, Error* error);
    virtual void dngConverterDidFinishConverting(DNGConverter* converter);

private:
    MovieConverter			_movieConverter;
    FileCopier				_fileCopier;
    Zipper					_zipper;
    FTPUploader				_uploader;
    FTPUploaderManager      _uploaderManager;
    WebServiceNotifier		_webServiceNotifier;
	ImageCompressor			_imageCompressor;
    DNGConverter            _dngConverter;

    int                     _processingLevel;

private slots:
    void on_removeButton_clicked();
    void on_editButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::TaskWidget *ui;
    virtual void paintEvent(QPaintEvent *pe);
};

class TaskWidgetDelegate
{
public:
    virtual void taskWidgetRemove(TaskWidget* widget) { Q_UNUSED(widget) }
    virtual void taskWidgetEdit(TaskWidget* widget) { Q_UNUSED(widget) }
    virtual void taskWidgetDidBegin(TaskWidget* widget) { Q_UNUSED(widget) }
    virtual void taskWidgetDidFinish(TaskWidget* widget) { Q_UNUSED(widget) }
    virtual void taskWidgetDidFail(TaskWidget* widget, Error* error) { Q_UNUSED(widget); Q_UNUSED(error) }
    virtual void taskWidgetWasCancelled(TaskWidget* widget) { Q_UNUSED(widget) }
    virtual void taskWidgetWillBeCancelled(TaskWidget* widget) { Q_UNUSED(widget) }
	//virtual bool taskWidgetCanUseShootTitleId(TaskWidget* widget, int shootTitleId) { Q_UNUSED(widget);Q_UNUSED(shootTitleId); return true; }
	//virtual bool taskWidgetCanUseSubmitOption(TaskWidget* widget, SUBMIT_OPTION option) { Q_UNUSED(widget); Q_UNUSED(option); return true; }
    virtual QSize hdm4vSize() { return QSize(); }
    virtual QString ftpPassword() { return QString(); }
    virtual QString ftpBaseDirectory() { return QString(); }
    virtual QString artistName() { return QString(); }
	virtual bool shouldCompressImageForDPI() { return false; }
	virtual int maxDPI() { return 0; }
	virtual bool shouldCompressImageForSize() { return false; }
	virtual QSize maxImageSize() { return QSize(0,0); }
};

#endif // TASKWIDGET_H
