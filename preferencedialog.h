/*

	preferencedialog.h

	Subclass QDialog for Preference

*/


#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>
#include "exttablemodel.h"
#include "servertablemodel.h"

#include <QNetworkConfigurationManager>
#include <qnetworkreply.h>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Ui {
class PreferenceDialog;
}


class PreferenceDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PreferenceDialog(QWidget *parent = 0);
    ~PreferenceDialog();
    
public:

	void closeEvent(QCloseEvent * closeEvent);
    void addServer(QString & server, QString & description, bool enabled);
    void getServerList();

private slots:
    void on_addServerButton_clicked();
    void on_removeServerButton_clicked();
    void on_addExtensionButton_clicked();
    void on_removeExtensionButton_clicked();
    void on_OKButton_clicked();
    void on_CancelButton_clicked();

    void on_chooseDNGConverterButton_clicked();

    void on_useLossyCompressionCheckBox_stateChanged(int arg1);

    void readServerListHttpData();

private:
    ExtTableModel extTableModel;
    ServerTableModel serverTableModel;
    QString dngConverterLocation;

    QNetworkReply* _networkReply;
    QNetworkAccessManager _manager;

private:
    Ui::PreferenceDialog *ui;
};



#endif // PREFERENCEDIALOG_H
