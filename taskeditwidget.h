#ifndef TASKEDITWIDGET_H
#define TASKEDITWIDGET_H

#include <QWidget>
#include <QVariantList>
#include "ApplicationDefine.h"
#include <QVariantList>
#include <QDialog>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include "tablemodel.h"
#include "comboboxitemdelegate.h"
#include <QLineEdit>
#include "taskeditwidgetdelegate.h"
#include "mainwindow.h"
//#include "videoeditwidget.h"

class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;
class TaskEditWidgetDelegate;

//class TaskEditWidgetDelegate
//{
//public:
//    virtual void taskEditWidgetDidEndEditing(TaskEditWidget* widget) {(void)widget;}
//    virtual void videoEditWidgetView(TaskEditWidget* widget) {(void)widget;}
////    virtual void videoEditWidgetCancelView(TaskEditWidget* widget) {(void)widget;}
////    virtual void videoPlayWidgetView(TaskEditWidget* widget) {(void)widget;}
//    virtual void taskEditWidgetDidCancel(TaskEditWidget* widget) {(void)widget;}
//    virtual bool taskEditWidgetCanUseShootTitleId(TaskEditWidget* widget, QString& titleId) { (void)widget;(void)titleId;return true; }
//    virtual bool taskEditWidgetCanUseSubmitOption(TaskEditWidget* widget, SUBMIT_OPTION option) { (void)widget;(void)option;return true; }
//};

namespace Ui {
class TaskEditWidget;
}

class ShootTitleDataSource;
class TaskEditWidget : public QDialog
{
    Q_OBJECT
    
public:
//    explicit TaskEditWidget(QWidget *parent = 0);
    explicit TaskEditWidget(MainWindow *arg1);
    ~TaskEditWidget();

    Q_PROPERTY(TaskEditWidgetDelegate* delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(QString folderURL READ folderURL WRITE setFolderURL)
    Q_PROPERTY(SUBMIT_OPTION option READ option WRITE setOption)
    Q_PROPERTY(QVariantList clips READ clips WRITE setClips)
    Q_PROPERTY(bool autoEditing READ autoEditing WRITE setAutoEditing)
    Q_PROPERTY(QString shootTitleId READ shootTitleId WRITE setShootTitleId)
    Q_PROPERTY(ShootTitleDataSource* shootTitleDataSource READ shootTitleDataSource WRITE setShootTitleDataSource)
	Q_PROPERTY(bool mailingCheck READ mailingCheck WRITE setMailingCheck)
	Q_PROPERTY(QString noteToEditor READ noteToEditor WRITE setNoteToEditor)

private:
    TaskEditWidgetDelegate* _delegate;
    QString _folderURL;
    SUBMIT_OPTION _option;
    bool _autoEditing;
    int _shootTitleIndex;
    ShootTitleDataSource* _shootTitleDataSource;

    TableModel _pic_model, _old_pic_model;
    TableModel _video_model, _old_video_model;
    TableModel _audio_model, _old_audio_model;
    TableModel _model;
    QSortFilterProxyModel _pic_filterProxy;
    QSortFilterProxyModel _video_filterProxy;
    QSortFilterProxyModel _audio_filterProxy;

    ComboBoxItemDelegate _comboBoxItemDelegate;
	bool			_mailingCheck;
	QString			_noteToEditor;

    QStringList _serverShootTitleList;

    QWidget* pw;
    QWidget* pmw;

    QVariantList updateFiles;

public:

public:
    TaskEditWidgetDelegate* delegate();
    void setDelegate(TaskEditWidgetDelegate* delegate);
    TaskEditWidgetDelegate* getDelegate();
    QString folderURL();
    void setFolderURL(QString value);
    SUBMIT_OPTION option();
    void setOption(SUBMIT_OPTION value);
    QVariantList& clips();
    void setClips(QVariantList& value);
    bool autoEditing();
    void setAutoEditing(bool value);
    QString shootTitleId();
    void setShootTitleId(QString& value);
    ShootTitleDataSource* shootTitleDataSource();
    void setShootTitleDataSource(ShootTitleDataSource* value);
	bool mailingCheck();
	void setMailingCheck(bool flag);
	QString noteToEditor();
	void setNoteToEditor(QString & note);

    void addShootTitleList( QStringList  *sl );
    void searchedShootTitle(QString text);
    void assignTableModel( TableModel& toTm, TableModel& fromTm );
    static QStringList fileURLsAtDirectoryPath( QString directoryPath, QStringList extensions );
    static QStringList fileExtensionsForOption( SUBMIT_OPTION opt );

    void uploadClips( QString fpath );
    void setParentMainwindow( QWidget* pm );

    QStringList getVideoFiles();
    bool getRemoveAudio();

    void _videoEidtWidgetView();
    void setDataModel(QVariantList picModel, QVariantList videoModel, QVariantList audioModel);
    void setEnvironment(QString fURL, SUBMIT_OPTION opt, bool autoEdit, bool mailCheck, QString shootID, QString noteEdit);
//    void _videoEditWidgetCancelView();
//    void _videoPlayWidgetView(VideoEditWidget *editWidget);

//    void _videoEditReEditView();

private:
    void updateClips();
    void validateSubmitOption();
    QStringList videoFileList;
    bool bRemoveAudio;

private slots:
    void on_deleteButton_clicked();
    void on_btnNext_clicked();
    void on_cancelButton_clicked();
    void on_browse_clicked();
    void on_optionCombo_currentIndexChanged(int index);
    void on_autoEditingCheck_clicked();
    void on_mailingCheck_clicked();
    void on_noteTextEdit_textChanged();
    void on_addButton_clicked();
    void on_picTableView_clicked(const QModelIndex &index);

    void on_previewCheckBox_clicked(bool checked);

    void on_playVideo_clicked();

    void on_stopVideo_clicked();


private:
    Ui::TaskEditWidget *ui;
    MainWindow *window;
    friend class TableModel;

    VlcInstance *_instance;
    VlcMedia *_media;
    VlcMediaPlayer *_player;

signals:
//    void textChanged(const QString &text);
//    void textEdited(const QString &text);

private slots:
    void on_searchShootTitleEdit_textChanged(const QString &arg1);
    void on_videoTableView_clicked(const QModelIndex &index);
    void on_audioTableView_clicked(const QModelIndex &index);
    void on_addPicButton_clicked();
    void on_deletePicButton_clicked();
    void on_addVideoButton_clicked();
    void on_deleteVideoButton_clicked();
    void on_addAudioButton_clicked();
    void on_deleteAudioButton_clicked();
    void on_btnDone_clicked();
};

#endif // TASKEDITWIDGET_H
