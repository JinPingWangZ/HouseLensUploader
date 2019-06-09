#include "mainwindow.h"
#include <QApplication>
#include <VLCQtCore/Common.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VlcCommon::setPluginPath(a.applicationDirPath() + "/plugins");
    a.setOrganizationDomain("HouseLens");
    a.setApplicationName("HouseLens Uploader");
    MainWindow w;
    w.show();

    return a.exec();
}
