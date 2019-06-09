#ifndef EXTTABLEMODEL_H
#define EXTTABLEMODEL_H

#include <QAbstractTableModel>

class ExtTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ExtTableModel(QObject *parent = 0);
    ~ExtTableModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    void startEditExtensions();
    void removeExtensionAtIndex( int nIndex );
    void addExtension( QString & ext );
    void endEditExtensions( bool shouldSave = false );

private:
    QStringList * extensions;
};

#endif // EXTTABLEMODEL_H
