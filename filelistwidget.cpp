#include "filelistwidget.h"
#include <QDebug>

FileListWidget::FileListWidget(QWidget* parent)
    : QListWidget(parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);

    setViewMode(QListView::IconMode);
    setMovement(QListView::Snap);
    setGridSize(QSize(180, 120));
    setSelectionMode(QAbstractItemView::SingleSelection);

    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);

    activedItem = new QListWidgetItem;

    setAlternatingRowColors(true);
}

void FileListWidget::setRemoveTime(QString ti)
{
    activedItem->setStatusTip(ti);
}

void FileListWidget::setLineEdit(QLineEdit *ldt)
{
    lineEdit = ldt;
}

void FileListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QListWidget::dragEnterEvent(event);
}

void FileListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QListWidget::dragMoveEvent(event);
}

void FileListWidget::dropEvent(QDropEvent *event)
{
    qDebug() << "dropEvent";

    QListWidgetItem *target = itemAt(event->pos());
    int temp_row = row(target);
    if ( temp_row <= 0 || temp_row == count()-1 ) {

        temp_row = currentRow();
    }
    //get the drag item
    //remove the drag item
    if ( currentRow() == 0 ) {
        temp_row = -1;
    }

    if ( currentRow() == count()-1 ) {
        temp_row = count();
    }

    QListWidgetItem *a = takeItem(currentRow());

    //add the drag item to position of target item
    insertItem(temp_row, a);
    //set Select item
    setCurrentItem(a);
}


//void FileListWidget::enterEvent(QEnterEvent *event)
//{
//    QListWidget::enterEvent(event);
//}

void FileListWidget::mousePressEvent(QMouseEvent *event){
    QListWidgetItem *target = itemAt(event->pos());
    activedItem = target;
    qDebug() << target->whatsThis() << " " << target->statusTip();
    lineEdit->setText( target->statusTip() );
    QListWidget::mousePressEvent(event);
}

//void FileListWidget::mouseReleaseEvent(QMouseEvent *event)
//{
//    QListWidgetItem *target = itemAt(event->pos());
//    qDebug() << "mousePressEvent: " << target->whatsThis();
//    qDebug() << "mousePressEvent: " << target->statusTip();

//}
