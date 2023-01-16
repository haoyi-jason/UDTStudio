#ifndef BMS_EVENTMODEL_H
#define BMS_EVENTMODEL_H
#include <QtCore>

class BMS_Event;

class BMS_EventModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    BMS_EventModel(QObject *parent = nullptr);
    ~BMS_EventModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

public:
    QList<BMS_Event*> events();
    void setEvents(QList<BMS_Event*>);
    void addEvent(BMS_Event *event);
    enum Column{
        Date,
        Time,
        EventName,
        EventLevel,
        State,
        Infomration,
        ColumnCount
    };
    int currentPage() const;

public slots:
    void setEventFile(QString fileName);
    void setRecrodPerPage(int records);
    void setPage(int page);
    void nextPage();
    void prevPage();
    void clearData();

private:
    QList<BMS_Event*> _data;
    QString _eventFileName;
    int _nofRecords;
    int _currentPage;
    int _nofRecordsPerPage;
    int _totalPages;
};

class BMS_Event:public QObject
{
    Q_OBJECT
public:
    explicit BMS_Event(QObject *parent = nullptr){}
    friend QTextStream &operator << (QTextStream &out,const BMS_Event *event){
        out << QString("%1;").arg(event->DateString);
        out << QString("%1;").arg(event->TimeString);
        out << QString("%1;").arg(event->EventName);
        out << QString("%1;").arg(event->Level);
        out << QString("%1;").arg(event->State);
        out << QString("%1\n").arg(event->Information);
    }

    bool parse(QString msg){
        QStringList sl = msg.split(";");
        if(sl.size() == 6){
            DateString = sl[0];
            TimeString = sl[1];
            EventName = sl[2];
            Level = sl[3];
            State = (sl[4]);
            Information = sl[5];
        }
        else{
            return false;
        }
    }

public:
    QString _bcuID;
    QString DateString, TimeString;
    QString EventName;
    QString Level;
    QString State;
    QString Information;
};

#endif // BMS_EVENTMODEL_H
