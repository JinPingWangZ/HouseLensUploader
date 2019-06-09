#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>

class ImageViewer : public QWidget
{
	Q_OBJECT

public:
	ImageViewer(QWidget *parent);
	~ImageViewer();

	void setImage( QString & imageFile );

protected:
    void paintEvent(QPaintEvent *event);

private:
	QPixmap * m_pPixmap;
	QRect	 m_Rect;
};

#endif // IMAGEVIEWER_H
