#include "imageviewer.h"
#include <QtGui>

ImageViewer::ImageViewer(QWidget *parent)
	: QWidget(parent), m_pPixmap( NULL )
{
	m_Rect.setRect(0, 0, 0, 0);
}

ImageViewer::~ImageViewer()
{
	if( m_pPixmap != NULL )
		delete m_pPixmap;
}

void ImageViewer::paintEvent(QPaintEvent * /* event */)
{
	if( m_pPixmap == NULL )
		return;
	
    QPainter painter(this);
    painter.eraseRect(QRect(0, 0, width(), height()));
	painter.drawPixmap( m_Rect, *m_pPixmap );
}

void ImageViewer::setImage( QString & imageFile )
{
	if( imageFile.isNull() || imageFile.isEmpty() )
		return;

	if( m_pPixmap != NULL )
		delete m_pPixmap;

	m_pPixmap = new QPixmap( imageFile );
	if( m_pPixmap == NULL || m_pPixmap->isNull() )
		return;

	QSize newImageSize;
	newImageSize.setWidth( this->width() ); 
	newImageSize.setHeight( (float)m_pPixmap->height() / ((float)m_pPixmap->width() / (float)this->width()) );
	if( newImageSize.height() > this->height() )
	{
		newImageSize.setWidth( (float)newImageSize.width() / ((float)newImageSize.height() / (float)this->height()) );
		newImageSize.setHeight( this->height() );
	}
	
	m_pPixmap->scaled( newImageSize );

	m_Rect.setLeft( (this->width() - newImageSize.width()) / 2.0 );
	m_Rect.setTop( (this->height() - newImageSize.height()) / 2.0 );
	m_Rect.setWidth( newImageSize.width() );
	m_Rect.setHeight( newImageSize.height() );

	update();
}