#include "bms_eventviewer.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QSettings>

BMS_EventViewer::BMS_EventViewer(QWidget *parent)
    :QTableView(parent)
{
    _eventModel = new BMS_EventModel(this);
    setModel(_eventModel);


}

BMS_EventViewer::~BMS_EventViewer()
{

}

void BMS_EventViewer::createHeader()
{

}
