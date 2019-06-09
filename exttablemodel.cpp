#include "exttablemodel.h"
#include <QSettings>
#include <QVariant>

ExtTableModel::ExtTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    extensions = NULL;
}

ExtTableModel::~ExtTableModel()
{
    if( extensions != NULL )
        delete extensions;
}


// - Edit Mode Implementation ---------------------------------------------------------------------------------

void ExtTableModel::startEditExtensions()
{
    if( extensions != NULL )
        delete extensions;
    extensions = new QStringList();

    QSettings preference;
    QVariant customExtensionsForPhotos = preference.value( "ExtensionForPhotos" );
    QStringList exts = customExtensionsForPhotos.toStringList();
    extensions->append(exts);
}

void ExtTableModel::removeExtensionAtIndex( int nIndex )
{
    if( extensions == NULL )
        return;

    if( extensions->count() > nIndex )
    {
        this->beginRemoveRows( QModelIndex(), nIndex, nIndex );
        extensions->removeAt( nIndex );
        this->endRemoveRows();
    }
}

void ExtTableModel::addExtension( QString & ext )
{
    if( extensions == NULL )
        return;

    int lastIndex = extensions->count();
    this->beginInsertRows( QModelIndex(), lastIndex, lastIndex );
    extensions->push_front(ext);
    this->endInsertRows();
}

void ExtTableModel::endEditExtensions( bool shouldSave )
{
    if( shouldSave )
    {
        QSettings preference;
        QStringList newExts;
        for( int i = 0 ; i < extensions->count() ; i++ )
        {
            QString ext = extensions->at(i);
            if( ext.length() > 0 )
                newExts.push_back(ext);
        }
        preference.setValue( "ExtensionForPhotos", newExts );
    }

    if( extensions != NULL )
    {
        delete extensions;
        extensions = NULL;
    }
}

// - QAbstractTableModel Implementation -------------------------------------------------------------------------

int	ExtTableModel::rowCount ( const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return extensions->count();
}

int ExtTableModel::columnCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return 1;
}

Qt::ItemFlags ExtTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant ExtTableModel::data ( const QModelIndex & index, int role ) const
{
    if( !index.isValid() )
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole )
    {
        if( (index.column() == 0) && (index.row() >= 0) && (index.row() < extensions->count()) )
        {
            QString ext = extensions->at( index.row() );
            return ext;
        }
    }

    return QVariant();
}

QVariant ExtTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if( section == 0 )
        return QString( "Extensions" );
    }

    return QVariant();
}

bool ExtTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    qDebug() << "setData:" << index << " value:" << value << " role:" << role << "\n";
    if (role != Qt::EditRole)
        return false;

    if( !value.isValid() ) {
        //qDebug() << "invalid value" << "\n";
        return false;
    }

    if( !index.isValid() )
        return false;

    extensions->replace(index.row(), value.toString());
    emit dataChanged(index, index);
    return true;
}
