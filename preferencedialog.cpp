/*

	preferencedialog.cpp

	Subclass QDialog for Preference

*/

#include "preferencedialog.h"
#include "ui_preferencedialog.h"
#include "preference.h"
#include <QFileDialog>
#include "qnetworkrequest.h"


PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
	
    serverTableModel.startEditServers();
    extTableModel.startEditExtensions();

    getServerList();

    this->ui->serverTableView->setModel(&serverTableModel);
    this->ui->serverTableView->setColumnWidth(0, 20);
    this->ui->serverTableView->setColumnWidth(1, 200);
    this->ui->serverTableView->setColumnWidth(2, 170-8);
    this->ui->extTableView->setModel(&extTableModel);
    this->ui->extTableView->setColumnWidth(0, 390-2);
	this->ui->HDWidthTextEdit->setText( QString( "%1" ).arg( Preference::hdVideoWidth() ) );
	this->ui->HDHeightTextEdit->setText( QString( "%1" ).arg( Preference::hdVideoHeight() ) );
	this->ui->MaxDPITextEdit->setText( QString( "%1" ).arg( Preference::maxImageDPI() ) );
	this->ui->MaxPixelWidthTextEdit->setText( QString( "%1" ).arg( Preference::maxImageWidth() ) );
	this->ui->MaxPixelHeightTextEdit->setText( QString( "%1" ).arg( Preference::maxImageHeight() ) );
	this->ui->DPICompressCheckBox->setChecked( Preference::shouldCompressImageForDPI() );
	this->ui->SizeCompressCheckBox->setChecked( Preference::shouldCompressImageForSize() );

    // DNG
    this->ui->dngConverterPathLabel->setText(Preference::dngConverterLocation());
    this->ui->jpegPreviewComboBox->setCurrentIndex(Preference::dngPreviewIndex());
    this->ui->embedFastLoadDataCheckBox->setChecked(Preference::dngEmbedFastLoadData());
    this->ui->useLossyCompressionCheckBox->setChecked(Preference::dngUseLossyCompression());
    this->ui->lossyCompressionComboBox->setCurrentIndex(Preference::dngLossyCompressionIndex());
    this->ui->lossyCompressionComboBox->setEnabled(Preference::dngUseLossyCompression());
    this->dngConverterLocation = Preference::dngConverterLocation();

    // invisible addServerButton and removeServerButton
    this->ui->addServerButton->setVisible(false);
    this->ui->removeServerButton->setVisible(false);
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

void PreferenceDialog::closeEvent(QCloseEvent * closeEvent)
{
    extTableModel.endEditExtensions();
    serverTableModel.endEditServer();
	QDialog::closeEvent(closeEvent);
}

// - Button Events -----------------------------------------------------------
void PreferenceDialog::on_addServerButton_clicked()
{
    addServer(tr("ServerDomain"), tr("ServerName"), false);
//    serverTableModel.addServer(tr("ServerDomain"), tr("ServerName"), false);
//    this->ui->serverTableView->selectRow(0);
}

void PreferenceDialog::addServer(QString &server, QString &description, bool enabled)
{
    serverTableModel.addServer( server, description, enabled );
    this->ui->serverTableView->selectRow(0);
}

void PreferenceDialog::on_removeServerButton_clicked()
{
    QModelIndexList selectedIndexes = this->ui->serverTableView->selectionModel()->selectedIndexes();

    //Sort Index List
    int nCnt = selectedIndexes.count();
    for( int i = 0 ; i < nCnt - 1 ; i++ )
    {
        for( int j = i + 1 ; j < nCnt ; j++ )
        {
            QModelIndex iIndex = selectedIndexes.at(i);
            QModelIndex jIndex = selectedIndexes.at(j);

            if( iIndex.row() > jIndex.row() )
                selectedIndexes.swap(i,j);
        }
    }

    for( int i = selectedIndexes.count() - 1 ; i >= 0  ; i-- )
    {
        QModelIndex selectedIndex = selectedIndexes.at( i );
        serverTableModel.removeServerAtIndex( selectedIndex.row() );
    }
}


void PreferenceDialog::on_addExtensionButton_clicked()
{
    /*
	QString ext = this->ui->ExtTextEdit->text();
	if( ext.isEmpty() )
		return;
    extTableModel.addExtension( ext );
    */

    extTableModel.addExtension(tr(""));
    //Make the first item selected
    this->ui->extTableView->selectRow(0);
}

void PreferenceDialog::on_removeExtensionButton_clicked()
{
    QModelIndexList selectedIndexes = this->ui->extTableView->selectionModel()->selectedIndexes();

	//Sort Index List
	int nCnt = selectedIndexes.count();
	for( int i = 0 ; i < nCnt - 1 ; i++ )
	{
		for( int j = i + 1 ; j < nCnt ; j++ )
		{
			QModelIndex iIndex = selectedIndexes.at(i);
			QModelIndex jIndex = selectedIndexes.at(j);

			if( iIndex.row() > jIndex.row() )
				selectedIndexes.swap(i,j);
		}
	}

	for( int i = selectedIndexes.count() - 1 ; i >= 0  ; i-- )
	{
		QModelIndex selectedIndex = selectedIndexes.at( i );
        extTableModel.removeExtensionAtIndex( selectedIndex.row() );
	}
}

void PreferenceDialog::on_OKButton_clicked()
{
    extTableModel.endEditExtensions( true );
    serverTableModel.endEditServer( true );
    Preference::setHDVideoWidth( this->ui->HDWidthTextEdit->text().toInt() );
    Preference::setHDVideoHeight( this->ui->HDHeightTextEdit->text().toInt() );
    Preference::setMaxImageDPI( this->ui->MaxDPITextEdit->text().toInt() );
    Preference::setMaxImageWidth( this->ui->MaxPixelWidthTextEdit->text().toInt() );
    Preference::setMaxImageHeight( this->ui->MaxPixelHeightTextEdit->text().toInt() );
    Preference::setShouldCompressImageForDPI( this->ui->DPICompressCheckBox->isChecked() );
    Preference::setShouldCompressImageForSize( this->ui->SizeCompressCheckBox->isChecked() );
    // DNG
    Preference::setDngConverterLocation( this->dngConverterLocation );
    Preference::setDNGPreviewIndex( this->ui->jpegPreviewComboBox->currentIndex() );
    Preference::setEmbedFastLoadData( this->ui->embedFastLoadDataCheckBox->isChecked() );
    Preference::setDNGUseLossyCompression( this->ui->useLossyCompressionCheckBox->isChecked() );
    Preference::setDNGLossyCompressionIndex( this->ui->lossyCompressionComboBox->currentIndex() );

	QDialog::accept();
}

void PreferenceDialog::on_CancelButton_clicked()
{
    extTableModel.endEditExtensions();
    serverTableModel.endEditServer();
	QDialog::reject();
}

void PreferenceDialog::on_chooseDNGConverterButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choose Adobe DNG Converter"), QString(), tr("*.exe"));
    if( path.isEmpty() )
        return;
    this->dngConverterLocation = path;
    this->ui->dngConverterPathLabel->setText(path);
}

void PreferenceDialog::on_useLossyCompressionCheckBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    this->ui->lossyCompressionComboBox->setEnabled(this->ui->useLossyCompressionCheckBox->isChecked());
}

void PreferenceDialog::getServerList()
{
//    QString serverListUrl = QString("https://testuploadservice.myhouselens.com/GridExtendedService.svc/GetServerList");
    QString serverListUrl = QString("https://vluploadent.myhouselens.com/GridExtendedService.svc/GetServerList");

    QUrl url(serverListUrl);

    QNetworkRequest serverListRequest(url);

    serverListRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");

    _networkReply = _manager.get(serverListRequest);

    connect(_networkReply, SIGNAL(finished()), this, SLOT(readServerListHttpData()));
}

void PreferenceDialog::readServerListHttpData()
{
    _networkReply->deleteLater();

    if( _networkReply->error() != QNetworkReply::NoError )
    {
        qDebug() << "network error: " << _networkReply->error() << "\n";
//        qDebug() << jsn["Message"].toString();
        _networkReply = NULL;
        return;
    }

    QByteArray httpData = _networkReply->readAll();
    _networkReply = NULL;

    if( httpData.isEmpty() )
    {
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(httpData);
    if( jsonDoc.isEmpty() )
    {
        return;
    }

    QJsonObject json = jsonDoc.object();

    if( json.isEmpty() )
    {
        return;
    }

    QJsonArray shoots = json["Data"].toArray();

    for( int i = 0; i < shoots.count(); i++ )
    {
        QJsonObject shootJson = shoots[i].toObject();
        QString serverHost = shootJson["ServerDomain"].toString();
        QString serverDescription = shootJson["ServerName"].toString();
        bool serverEnabled = shootJson["IsEnabled"].toInt();

        addServer( serverHost, serverDescription, serverEnabled );
    }


}
