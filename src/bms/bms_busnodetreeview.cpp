#include "bms_busnodetreeview.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QSettings>

#include "headerview.h"
BMS_BusNodeTreeView::BMS_BusNodeTreeView(QWidget *parent)
    :BMS_BusNodeTreeView(nullptr,parent)
{

}

BMS_BusNodeTreeView::BMS_BusNodeTreeView(CanOpen *canOpen, QWidget *parent)
    :QTreeView(parent)
{
    _stackModel = new BMSStackModel(this);
    _sortFilterProxyModel = new QSortFilterProxyModel(this);
    _sortFilterProxyModel->setSourceModel(_stackModel);
    setModel(_sortFilterProxyModel);

    setCanOpen(canOpen);
    setAnimated(true);
    connect(selectionModel(),&QItemSelectionModel::selectionChanged,this,&BMS_BusNodeTreeView::updateSelection);
    connect(this,&QAbstractItemView::doubleClicked,this,&BMS_BusNodeTreeView::indexDbClick);

    createHeader();
}

BMS_BusNodeTreeView::~BMS_BusNodeTreeView()
{

}

CanOpen *BMS_BusNodeTreeView::canOpen() const
{
    return _stackModel->canOpen();
}

void BMS_BusNodeTreeView::setCanOpen(CanOpen *canOpen)
{
    if(_stackModel->canOpen() != nullptr){
        disconnect(_stackModel->canOpen(),nullptr,this,nullptr);
    }

    _stackModel->setCanOpen(canOpen);
    if(canOpen != nullptr){
        connect(canOpen,&CanOpen::busAdded,this,&BMS_BusNodeTreeView::addBus);
    }
}

CanOpenBus *BMS_BusNodeTreeView::currentBus() const
{
    return _stackModel->bus(_sortFilterProxyModel->mapToSource(selectionModel()->currentIndex()));
}

Node *BMS_BusNodeTreeView::currentNode() const{
    return _stackModel->node(_sortFilterProxyModel->mapToSource(selectionModel()->currentIndex()));
}

void BMS_BusNodeTreeView::addBusAction(QAction *action)
{
    _busActions.append(action);
}

void BMS_BusNodeTreeView::addNodeAction(QAction *action)
{
    _nodeActions.append(action);
}

void BMS_BusNodeTreeView::saveState(QSettings &settings)
{
    settings.setValue("header",header()->saveState());
}

void BMS_BusNodeTreeView::restoreState(QSettings &settings)
{
    header()->restoreState(settings.value("header").toByteArray());
}

void BMS_BusNodeTreeView::updateSelection()
{
    emit busSelected(currentBus());
    emit nodeSelected(currentNode());
}

void BMS_BusNodeTreeView::indexDbClick(const QModelIndex &index)
{
    QModelIndex indexStack = _sortFilterProxyModel->mapToSource(index);

    CanOpenBus *bus = _stackModel->bus(indexStack);
    if(bus != nullptr){
        if(!bus->isConnected()){
            bus->canBusDriver()->connectDevice();
        }
        if(bus->isConnected() && bus->nodes().isEmpty()){
            bus->exploreBus();
        }
    }
}

void BMS_BusNodeTreeView::addBus(quint8 id)
{
    CanOpenBus *bus = CanOpen::bus(id);

    if(bus == nullptr){
        return;
    }

    connect(bus, &CanOpenBus::nodeAdded,[=](int nodeId){addNode(bus,nodeId);});
}

void BMS_BusNodeTreeView::addNode(CanOpenBus *bus, quint8 id)
{
    Q_UNUSED(id);

    if(bus->nodes().count() == 1){
        for(int row = 0; row < _sortFilterProxyModel->rowCount();row++){
            QModelIndex index = _sortFilterProxyModel->index(row,BMSStackModel::StackId);
            if(_sortFilterProxyModel->data(index) == QVariant(bus->busId())){
                expand(index);
            }
        }
    }
}

void BMS_BusNodeTreeView::createHeader()
{
    HeaderView *headerView = new HeaderView(Qt::Horizontal,this);
    setHeader(headerView);

    headerView->addMandatorySection(BMSStackModel::StackId);
    headerView->addMandatorySection(BMSStackModel::Name);
    headerView->setStretchLastSection(true);

    int w0 = QFontMetrics(font()).horizontalAdvance("0");

    headerView->resizeSection(BMSStackModel::StackId,12*w0);
    headerView->resizeSection(BMSStackModel::Name,14*w0);
    headerView->resizeSection(BMSStackModel::Status, 8*w0);

    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    sortByColumn(0,Qt::AscendingOrder);

}

void BMS_BusNodeTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    QList<CanOpenBus*> selectedBuses;
    QList<Node*> selectedNodes;

    QModelIndexList selectedRows = selectionModel()->selectedRows();
    for(QModelIndex row:qAsConst(selectedRows)){
        const QModelIndex &curentIndex = _sortFilterProxyModel->mapToSource((row));

        Node *node = _stackModel->node(curentIndex);
        if(node != nullptr){
            selectedNodes.append(node);
            continue;
        }

        CanOpenBus *bus = _stackModel->bus(curentIndex);
        if(bus != nullptr){
            selectedBuses.append(bus);
            continue;
        }
    }

    QMenu menu;

    if(!selectedBuses.isEmpty()){
        menu.addActions(_busActions);
        if(!selectedNodes.isEmpty()){
            menu.addSeparator();
        }
    }

    if(!selectedNodes.isEmpty()){
        menu.addActions(_nodeActions);
    }

    menu.exec(event->globalPos());

}






















































