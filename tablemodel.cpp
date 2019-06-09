#include "tablemodel.h"
#include <QVariantMap>
#include "ApplicationDefine.h"
#include <QFileInfo>
#include <QDebug>
#include <QMimeData>
#include <QByteArray>
#include <QDataStream>

#define MIMETYPE QLatin1String("application/tableviewdata")

class CompareMovie
{
public:
    CompareMovie(int column, Qt::SortOrder order)
    {
        _column = column;
        _order = order;
    }

    bool operator() (const QVariant& v1, const QVariant& v2) const
    {
        QVariantMap movie1 = v1.toMap();
        QVariantMap movie2 = v2.toMap();

        bool bResult = false;

        switch( _column )
        {
            case 0:
            {
                QString url1 = movie1.value(AppKey_URL).toString();
                QString url2 = movie2.value(AppKey_URL).toString();

                if( _order == Qt::AscendingOrder )
                    bResult = ( url1 < url2 );
                else
                    bResult = ( url2 < url1 );
            }
            break;

            case 1:
            {
                QFileInfo fileInfo1(movie1.value(AppKey_URL).toString());
                QFileInfo fileInfo2(movie2.value(AppKey_URL).toString());

                QString ext1 = fileInfo1.suffix().toLower();
                QString ext2 = fileInfo2.suffix().toLower();

                if( _order == Qt::AscendingOrder )
                    bResult = ( ext1 < ext2 );
             else
                    bResult = ( ext2 < ext1 );

            }
            break;

            case 2:
            {
                QFileInfo fileInfo1(movie1.value(AppKey_URL).toString());
                QFileInfo fileInfo2(movie2.value(AppKey_URL).toString());

                if( _order == Qt::AscendingOrder )
                    bResult = ( fileInfo1.size() < fileInfo2.size() );
                else
                    bResult = ( fileInfo2.size() < fileInfo1.size() );
            }
            break;

            case 3:
            {
                bool removesAudio1 = movie1.value(AppKey_RemoveAudio).toBool();
                bool removesAudio2 = movie2.value(AppKey_RemoveAudio).toBool();

                if( _order == Qt::AscendingOrder )
                    bResult = ( removesAudio1 < removesAudio2 );
                else
                    bResult = ( removesAudio2 < removesAudio1 );
            }
            break;
        }

        return bResult;
    }

protected:
    Qt::SortOrder _order;
    int _column;
};

TableModel::TableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _movies.count();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if( !index.isValid() )
        return QVariant();

    // we consider not only Qt::DisplayRole but also Qt::EditRole.
    // Because Qt::EditRole is necessary when tableview is edited by user.
    if (role == Qt::DisplayRole || role == Qt::EditRole ) {
        if( index.row() >= 0 && index.row() < _movies.count() ) {
            QVariantMap info = _movies.at(index.row()).toMap();

            QFileInfo fileInfo(info.value(AppKey_URL).toString());

            switch( index.column() ) {
                case 0: {// file name
                    return fileInfo.fileName();
                }
                case 1: {// file type
                    return fileInfo.suffix().toUpper();
                }
                case 2: {// file size
                    QString size;
                    size.sprintf("%.2f MB", (double)fileInfo.size()/1024/1024);
                    return size;
                }
                case 3: {// remove audio

//                    if( QString::compare(fileInfo.suffix(), "mov", Qt::CaseInsensitive) != 0 && QString::compare(fileInfo.suffix(), "avi", Qt::CaseInsensitive) != 0 ) {
//                        return QString("N/A");
//                    }

                    if( info.value(AppKey_RemoveAudio).toBool() )
                        return QString("YES");
                    else
                        return QString("NO");
                }
            }
        }
    }

    return QVariant();
}

void TableModel::setMovies(QVariantList& movies)
{
    this->beginResetModel();
    _movies.clear();
    this->endResetModel();

    qDebug() << "movie count = " << _movies.count() << "\n";

    beginInsertRows(QModelIndex(), 0, movies.count()-1);
    _movies = movies;
    endInsertRows();

//    emit dataChanged(index(0, 0), index(_movies.count()-1, 3));
}

QVariantList& TableModel::movies()
{
    return _movies;
}

// we should reimplement this flags for editing field.(Qt::ItemIsEditable)
Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsDropEnabled;

    Qt::ItemFlags flag = QAbstractTableModel::flags(index);
    if( index.column() == 3 )
        flag |= Qt::ItemIsEditable;

    return flag | Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled;

//    return (Qt::ItemIsDragEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable);
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Name");
            case 1:
                return tr("Type");
            case 2:
                return tr("Size");
            case 3:
                return tr("Remove Audio");
            default:
                return QVariant();
        }
    }
    else {
        return "  ";
    }

    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    qDebug() << "setData:" << index << " value:" << value << " role:" << role << "\n";

    if( !value.isValid() ) {
        qDebug() << "invalid value" << "\n";
        return false;
    }

    if( !index.isValid() )
        return false;

    if( index.column() != 3 )
        return false;

    if( role == Qt::DisplayRole ) {
        this->setRemovesAudio(index.row(), value.toBool());
        return true;
    }

    return false;
}

void TableModel::setRemovesAudio(int index, bool flag)
{
    qDebug() << "remove movie at " << index << "\n";
    qDebug() << "movie count = " << _movies.count() << "\n";
    qDebug() << "movie infos = " << _movies << "\n";
    QVariantMap dict = _movies.at(index).toMap();
    qDebug() << "removed video info: " << dict << "\n";
    dict[AppKey_RemoveAudio] = flag;
    _movies[index] = dict;
    emit dataChanged(this->index(index, 3), this->index(index, 3));
}

void TableModel::removeMoviesAtRows(QList<int>& rows)
{
    qSort(rows.begin(), rows.end(), qGreater<int>());
    Q_FOREACH(int aRow, rows) {
        beginRemoveRows(QModelIndex(), aRow, aRow);
        _movies.removeAt(aRow);
        endRemoveRows();
    }
}

void TableModel::sort( int column, Qt::SortOrder order)
{
    qDebug() << "sort column:" << column << " order: " << order << "\n";

    qSort(_movies.begin(), _movies.end(), CompareMovie(column, order));
    emit dataChanged(this->index(0, 0), this->index(_movies.count()-1, 3));
}

// Drag and Drop

// we should reimplement this function for indicate dragable data types.
QStringList TableModel::mimeTypes() const
{
//    qDebug() << "mimeTypes" << "\n";
    QStringList types;
    types << MIMETYPE;
    return types;
}

// when user start drag, this method will be called to supply dragging data.
QMimeData* TableModel::mimeData(const QModelIndexList &indexes) const
{
    qDebug() << "mimeData: " << indexes << "\n";

    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    qDebug() << "selected indexes:" << indexes << "\n";

    QList<int> selectedRows;

    Q_FOREACH(QModelIndex aIndex, indexes) {

        if( !selectedRows.contains(aIndex.row()) ) {
            selectedRows.push_back(aIndex.row());
        }
    }

    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    stream << selectedRows;
    mimeData->setData(MIMETYPE, encodedData);
    return mimeData;
}

// when user drop items, this method will be called.
bool TableModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    qDebug() << "dropMimeData action: "<< action << " row: "<< row << " column: "<< column << " parent:" << parent << "\n";

    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat(MIMETYPE))
        return false;

    if (column > 0)
        return false;

    int insertionRow = parent.row();
    if( insertionRow < 0 )
        insertionRow = _movies.count();

    QByteArray encodedData = data->data(MIMETYPE);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    QList<int> selectedRows;
    stream >> selectedRows;

    qSort(selectedRows.begin(), selectedRows.end(), qLess<int>());
    int count = 0;
    Q_FOREACH(int aRow, selectedRows) {
        if( aRow >= insertionRow )
            break;

        ++count;
    }

    insertionRow -= count;
    qDebug() << "insertion row = " << insertionRow << "\n";
    qDebug() << "count = " << count << "\n";
    qDebug() << "parent = " << parent.row() << "\n";

    qSort(selectedRows.begin(), selectedRows.end(), qGreater<int>());

    QVariantList selectedMovies;
    Q_FOREACH(int aRow, selectedRows) {
        selectedMovies.push_back(_movies[aRow]);
    }

    qDebug() << "deleted movies: " << selectedMovies << "\n";
    Q_FOREACH(int aRow, selectedRows) {
        _movies.removeAt(aRow);
    }

    qDebug() << "inserted movies: " << selectedMovies << "\n";
    Q_FOREACH(QVariant aMovie, selectedMovies) {
        _movies.insert(insertionRow, aMovie);
    }

    qDebug() << "selected rows = " << selectedRows << "\n";

    beginResetModel();
    endResetModel();
//    emit dataChanged(index(0, 0), index(_movies.count()-1, 3));

    return true;
}

Qt::DropActions TableModel::supportedDropActions () const
{
    return Qt::MoveAction|Qt::CopyAction;
}

