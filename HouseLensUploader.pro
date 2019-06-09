#-------------------------------------------------
#
# Project created by QtCreator 2017-04-20T17:55:37
#
#-------------------------------------------------

QT       += core
QT       += gui
QT       += network
QT       += multimedia
QT       += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HouseLensUploader
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    ApplicationDefine.cpp \
    comboboxitemdelegate.cpp \
    Error.cpp \
    filecopier.cpp \
    ftpuploader.cpp \
    imagecompressor.cpp \
    imageviewer.cpp \
    mailsender.cpp \
    movieconverter.cpp \
    mysettings.cpp \
    parallelanimationgroup.cpp \
    preference.cpp \
    preferencedialog.cpp \
    propertyanimation.cpp \
    QProgressIndicator.cpp \
    scrollarea.cpp \
    scrollareaviewportwidget.cpp \
    sequentialanimationgroup.cpp \
    settingfactory.cpp \
    shelltask.cpp \
    shoottitledatasource.cpp \
    tablemodel.cpp \
    taskeditwidget.cpp \
    tasklistcontainer.cpp \
    taskwidget.cpp \
    webservicenotifier.cpp \
    zipper.cpp \
    exttablemodel.cpp \
    servertablemodel.cpp \
    dngconverter.cpp \
    ftpuploadermanager.cpp \
    videoeditwidget.cpp \
    qmediainfo.cpp \
    videowidgetsurface.cpp \
    taskeditwidgetdelegate.cpp \
    filelistwidget.cpp

HEADERS  += mainwindow.h \
    ApplicationDefine.h \
    comboboxitemdelegate.h \
    Error.h \
    filecopier.h \
    ftpuploader.h \
    imagecompressor.h \
    imageviewer.h \
    mailsender.h \
    movieconverter.h \
    mysettings.h \
    parallelanimationgroup.h \
    preference.h \
    preferencedialog.h \
    propertyanimation.h \
    QProgressIndicator.h \
    resource.h \
    scrollarea.h \
    scrollareaviewportwidget.h \
    sequentialanimationgroup.h \
    settingfactory.h \
    shelltask.h \
    shoottitledatasource.h \
    tablemodel.h \
    taskeditwidget.h \
    tasklistcontainer.h \
    taskwidget.h \
    webservicenotifier.h \
    zipper.h \
    exttablemodel.h \
    servertablemodel.h \
    dngconverter.h \
    ftpuploadermanager.h \
    videoeditwidget.h \
    qmediainfo.h \
    videowidgetsurface.h \
    taskeditwidgetdelegate.h \
    filelistwidget.h

FORMS    += mainwindow.ui \
    preferencedialog.ui \
    taskeditwidget.ui \
    taskwidget.ui \
    videoeditwidget.ui

RESOURCES += \
    resource.qrc

# Edit below for custom library location

#debug
LIBS       += -L"$$PWD\vlc-qt\x64\debug\lib" -lVLCQtCore -lVLCQtWidgets
#LIBS       += -L"$$PWD\mediainfo\" -lMediaInfo

#release
#LIBS       += -L"$$PWD\vlc-qt\x64\release\lib" -lVLCQtCore -lVLCQtWidgets

INCLUDEPATH += "$$PWD\vlc-qt\x64\include"


RC_FILE = HouseLensUploader.rc
