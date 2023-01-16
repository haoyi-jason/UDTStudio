#ifndef BMS_EVENTVIEWER_H
#define BMS_EVENTVIEWER_H

#include <QTableView>
#include <QAction>
#include <QList>
#include <QSettings>
#include <QSortFilterProxyModel>

#include "system/bmsstack.h"
#include "system/bms_eventmodel.h"

class BMS_EventViewer : public QTableView
{
    Q_OBJECT
public:
    BMS_EventViewer(QWidget *parent = nullptr);
    ~BMS_EventViewer();

private:
    void createHeader();
private:
    BMS_EventModel *_eventModel;

};

#endif // BMS_EVENTVIEWER_H
