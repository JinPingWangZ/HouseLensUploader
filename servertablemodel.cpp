#include "servertablemodel.h"
#include "preference.h"
#include <QDebug>
#include <QSettings>

ServerTableModel::ServerTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    servers = NULL;
}

ServerTableModel::~ServerTableModel()
{
    if( servers != NULL )
    {
        qDeleteAll(servers->begin(), servers->end());
        delete servers;
    }
}

// - Edit Mode Implementation ---------------------------------------------------------------------------------
void ServerTableModel::startEditServers()
{
    if( servers != NULL )
    {
        qDeleteAll(servers->begin(), servers->end());
        delete servers;
    }
    servers = new QList<QVariantMap *>();

//    QVariantList existingServers = Preference::servers();
    QVariantList existingServers;

    for( int i = 0 ; i < existingServers.count() ; i++ )
    {
        QVariantMap * serverInfo = new QVariantMap(existingServers[i].toMap());
        servers->push_back(serverInfo);
    }
}

void ServerTableModel::removeServerAtIndex( int nIndex )
{
    if( servers == NULL )
        return;

    if( servers->count() > nIndex )
    {
        this->beginRemoveRows( QModelIndex(), nIndex, nIndex );
        QVariantMap * serverInfo = servers->at(nIndex);
        delete serverInfo;
        servers->removeAt( nIndex );
        this->endRemoveRows();
    }
}

void ServerTableModel::addServer( QString & server, QString & description, bool enabled )
{
    if( servers == NULL )
        return;

    int lastIndex = servers->count();
    this->beginInsertRows( QModelIndex(), lastIndex, lastIndex );
    QVariantMap * serverInfo = new QVariantMap;
    (*serverInfo)["enabled"] = enabled;
    (*serverInfo)["host"] = server;
    (*serverInfo)["description"] = description;
//    servers->push_front(serverInfo);
    servers->push_back(serverInfo);
    this->endInsertRows();
}

void ServerTableModel::endEditServer( bool shouldSave )
{
    if( shouldSave )
    {
        QVariantList newServers;
        for( int i = 0 ; i < servers->count() ; i++ )
        {
            QVariantMap * serverInfo = servers->at(i);
            if( (*serverInfo)["host"].toString().length() > 0 )
                newServers.push_back(*serverInfo);
        }
        Preference::setServers(newServers);
    }

    if( servers != NULL )
    {
        qDeleteAll(servers->begin(), servers->end());
        delete servers;
        servers = NULL;
    }
}

// - QAbstractTableModel Implementation -------------------------------------------------------------------------

int	ServerTableModel::rowCount ( const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return servers->count();
}

int ServerTableModel::columnCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return 3;
}

Qt::ItemFlags ServerTableModel::flags(const QModelIndex &index) const
{
    if( !index.isValid() )
        return 0;
    Qt::ItemFlags flag;
    if( index.column() == 0 )
        flag = Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    else
        flag = Qt::ItemIsEditable;

    return flag | QAbstractItemModel::flags(index);
}

QVariant ServerTableModel::data ( const QModelIndex & index, int role ) const
{
    if( !index.isValid() )
        return QVariant();

    if( index.row() < servers->count() )
    {
        QVariantMap * serverInfo = servers->at(index.row());
        if( index.column() == 0 )
        {
            if( role == Qt::CheckStateRole )
            {
                if( (*serverInfo)["enabled"].toBool() )
                    return Qt::Checked;
                else
                    return Qt::Unchecked;
            }
            else
                return QVariant();
        }
        else if( index.column() == 1 )
        {
            if( role == Qt::DisplayRole || role == Qt::EditRole )
                return (*serverInfo)["host"];
            else
                return QVariant();
        }
        else if( index.column() == 2 )
        {
            if( role == Qt::DisplayRole || role == Qt::EditRole )
                return (*serverInfo)["description"];
            else
                return QVariant();
        }
    }

    return QVariant();
}

QVariant ServerTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        if( section == 0 )
            return QString( "" );
        else if( section == 1 )
            return QString( "Host" );
        else if( section == 2 )
            return QString( "Description" );
        else
            return QVariant();
    }

    return QVariant();
}

bool ServerTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    qDebug() << "setData:" << index << " value:" << value << " role:" << role << "\n";

    if( !value.isValid() ) {
        //qDebug() << "invalid value" << "\n";
        return false;
    }

    if( !index.isValid() )
        return false;

//---------- You can not change the Server list information in the view.

//    if( index.row() < servers->count( ) )
//    {
//        QVariantMap * serverInfo = servers->at(index.row());
//        if( index.column() == 0 )
//        {
//            if( role == Qt::CheckStateRole )
//            {
//                if( value.toInt() == Qt::Checked )
//                    (*serverInfo)["enabled"] = true;
//                else
//                    (*serverInfo)["enabled"] = false;
//            }
//        }
//        else if( index.column() == 1 )
//        {
//            if( role == Qt::EditRole )
//                (*serverInfo)["host"] = value;
//        }
//        else if( index.column() == 2 )
//        {
//            if( role == Qt::EditRole )
//                (*serverInfo)["description"] = value;
//        }

//        emit dataChanged(index, index);
//        return true;
//    }

    return false;
}
