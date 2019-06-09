#ifndef SERVERTABLEMODEL_H
#define SERVERTABLEMODEL_H

#include <QAbstractTableModel>

class ServerTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ServerTableModel(QObject *parent = 0);
    ~ServerTableModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    void startEditServers();
    void removeServerAtIndex( int nIndex );
    void addServer( QString & server, QString & description, bool enabled );
    void endEditServer( bool shouldSave = false );

private:
    QList<QVariantMap *> * servers;
};

#endif // SERVERTABLEMODEL_H
