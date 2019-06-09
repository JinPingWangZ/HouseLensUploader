#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QObject>
#include <QtGui>
#include <QtCore>
#include <QListWidget>
#include <QLineEdit>

class FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    FileListWidget(QWidget* parent);
    QListWidgetItem *activedItem;
    void setRemoveTime( QString ti );
    void setLineEdit( QLineEdit* ldt );


private:
    QLineEdit* lineEdit;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void enterEvent(QEnterEvent* event);

private slots:

    void mousePressEvent(QMouseEvent* event);
//    void mouseReleaseEvent(QMouseEvent* event);

};

#endif // FILELISTWIDGET_H
