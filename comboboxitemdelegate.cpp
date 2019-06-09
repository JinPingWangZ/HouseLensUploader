#include "comboboxitemdelegate.h"
#include <QComboBox>
#include <QtDebug>
#include "tablemodel.h"
#include <QTableView>

ComboBoxItemDelegate::ComboBoxItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

ComboBoxItemDelegate::~ComboBoxItemDelegate()
{
}

QWidget* ComboBoxItemDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    Q_UNUSED(option);

    if( index.column() == 3 ) {
        QVariant data = index.data();
        if( data.toString() == QString("N/A") )
            return NULL;

        QComboBox *cb = new QComboBox(parent);
        cb->addItem(QString("NO"));
        cb->addItem(QString("YES"));
        return cb;
    }
    else {
        return NULL;
    }
}

void ComboBoxItemDelegate::setEditorData ( QWidget *editor, const QModelIndex &index ) const
{
    if(QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
        bool removesAudio = index.data(Qt::EditRole).toBool();
        QString currentText = index.data(Qt::EditRole).toString();
        qDebug() << "removeAudio = " << removesAudio << "\n";
        qDebug() << "currentText = " << currentText << "\n";

        int cbIndex = cb->findText(currentText);
        // if it is valid, adjust the combobox
        if(cbIndex >= 0)
            cb->setCurrentIndex(cbIndex);
    } else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ComboBoxItemDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    if(QComboBox *cb = qobject_cast<QComboBox *>(editor)) {
        model->setData(index, cb->currentIndex(), Qt::DisplayRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

