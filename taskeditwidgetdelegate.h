#ifndef TASKEDITWIDGETDELEGATE_H
#define TASKEDITWIDGETDELEGATE_H

#include "taskeditwidget.h"
#include "videoeditwidget.h"
#include <QWidget>

class TaskEditWidget;
class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;
class VideoEditWidget;

class TaskEditWidgetDelegate
{
public:
    TaskEditWidgetDelegate();

public:
//    virtual void taskEditWidgetDidEndEditing(TaskEditWidget* widget) {(void)widget;}
//    virtual void videoEditWidgetView(TaskEditWidget* widget) {(void)widget;}
//    virtual void videoEditWidgetCancelView(TaskEditWidget* widget) {(void)widget;}
//    virtual void videoPlayWidgetView(VideoEditWidget* widget) {(void)widget;}
//    virtual void taskEditWidgetDidCancel(TaskEditWidget* widget) {(void)widget;}
//    virtual bool taskEditWidgetCanUseShootTitleId(TaskEditWidget* widget, QString& titleId) { (void)widget;(void)titleId;return true; }
//    virtual bool taskEditWidgetCanUseSubmitOption(TaskEditWidget* widget, SUBMIT_OPTION option) { (void)widget;(void)option;return true; }
//    virtual void taskEditWidgetView(){ return; }
};

#endif // TASKEDITWIDGETDELEGATE_H
