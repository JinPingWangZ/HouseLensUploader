#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVariantList>
#include <QList>
#include <QStringList>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(QObject *parent = 0);
private:
    // QAbstractTableModel Override
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual void sort( int column, Qt::SortOrder order = Qt::AscendingOrder);
    // To support drag and drop.
    virtual QStringList mimeTypes() const;
    virtual QMimeData* mimeData(const QModelIndexList &indexes) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    virtual Qt::DropActions supportedDropActions () const;
public:
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    void setMovies(QVariantList& movies);
    QVariantList& movies();
    void setRemovesAudio(int index, bool flag);
    void removeMoviesAtRows(QList<int>& rows);
public:
    QVariantList _movies;
};

#endif // TABLEMODEL_H
