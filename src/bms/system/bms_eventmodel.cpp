#include "bms_eventmodel.h"

BMS_EventModel::BMS_EventModel(QObject *parent)
{
    _currentPage = 0;
    _nofRecordsPerPage = 10;
}

BMS_EventModel::~BMS_EventModel()
{

}

int BMS_EventModel::rowCount(const QModelIndex &parent) const
{
    return _nofRecordsPerPage;
}

int BMS_EventModel::columnCount(const QModelIndex &parent) const
{
    return ColumnCount;
}

QVariant BMS_EventModel::data(const QModelIndex &index, int role) const
{
    int base_row = _currentPage * _nofRecordsPerPage;
    int r = base_row + index.row();
    int c = index.column();

    //qDebug()<<Q_FUNC_INFO<<"Access Row:"<<r;
    if(r >= _data.count())
        return QVariant();
    BMS_Event *e = _data.at(r);
    switch(role)
    {
    case Qt::DisplayRole:
        switch(c){
        case Date: return QVariant(e->DateString);
        case Time: return QVariant(e->TimeString);
        case EventName: return QVariant(e->EventName);
        case EventLevel: return QVariant(e->Level);
        case State: return QVariant(e->State);
        case Infomration: return QVariant(e->Information);
        }
        break;
    case Qt::DecorationRole:
        break;
    case Qt::TextAlignmentRole:
        break;
    }
    return QVariant();
}

QVariant BMS_EventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal){
        switch(role){
        case Qt::DisplayRole:
            switch(section){
            case 0:return QVariant("Date");break;
            case 1: return QVariant("Time");break;
            case 2: return QVariant("Event Name");break;
            case 3: return QVariant("Level");break;
            case 4: return QVariant("State");break;
            case 5: return QVariant("Information");break;
            }
            break;
        }
    }
    return QVariant();
}

Qt::ItemFlags BMS_EventModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;
    flags.setFlag(Qt::ItemIsEditable,false);
    flags.setFlag(Qt::ItemIsSelectable,true);
    flags.setFlag(Qt::ItemIsEnabled,true);

    return flags;
}

void BMS_EventModel::setEventFile(QString fileName)
{
    //check filename first
    if(!QFile(fileName).exists()) {
        return;
    }
    _eventFileName = fileName;
    // load data
    _data.clear();
    QFile f(_eventFileName);
    f.open(QFile::ReadOnly);
    QTextStream ts(&f);
    ts.setCodec(QTextCodec::codecForName("Big5"));
//    ts.setCodec("UTF-16");
    while(!ts.atEnd()){
        BMS_Event *e = new BMS_Event();
        if(e->parse(ts.readLine())){
            _data.append(e);
        }
        else{
            delete e;
        }
    }
    f.close();
    _currentPage = 0;
    _nofRecords = _data.count();
    _totalPages = _nofRecords/_nofRecordsPerPage + 1;
}

void BMS_EventModel::setRecrodPerPage(int records)
{
    _nofRecordsPerPage = records;
}

void BMS_EventModel::setPage(int page)
{
    if(page < _totalPages){
        _currentPage = page;
        emit dataChanged(QModelIndex(),QModelIndex());
    }
    qDebug()<<Q_FUNC_INFO<<"Change Page to:"<<_currentPage;
}

void BMS_EventModel::nextPage()
{
    _currentPage++;
    if(_currentPage >= _totalPages){
        _currentPage--;
    }
    else{
        // upadte data
        emit dataChanged(QModelIndex(),QModelIndex());
    }
    qDebug()<<Q_FUNC_INFO<<"Change Page to:"<<_currentPage;
}

void BMS_EventModel::prevPage()
{
    _currentPage--;
    if(_currentPage < 0){
        _currentPage++;
    }
    else{
        // upadte data
        emit dataChanged(QModelIndex(),QModelIndex());
    }
    qDebug()<<Q_FUNC_INFO<<"Change Page to:"<<_currentPage;
}

int BMS_EventModel::currentPage() const
{
    return _currentPage;
}

void BMS_EventModel::clearData()
{
    _currentPage = 0;
    _data.clear();
    QFile f(_eventFileName);
    f.resize(0);
}



