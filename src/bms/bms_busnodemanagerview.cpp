#include "bms_busnodemanagerview.h"

#include <QLayout>
#include <QScrollArea>
#include <QEventLoop>
#include <QDebug>

BMS_BusNodesManagerView::BMS_BusNodesManagerView(QWidget *parent)
    :BMS_BusNodesManagerView(nullptr,parent)
{

}

BMS_BusNodesManagerView::BMS_BusNodesManagerView(CanOpen *canOpen, QWidget *parent)
    : QWidget(parent)
    ,_canOpen(canOpen)
{
    createWidgets();
    setCanOpen(canOpen);

    connect(_busNodeTreeView,&BMS_BusNodeTreeView::busSelected,this,&BMS_BusNodesManagerView::busSelected);
    connect(_busNodeTreeView,&BMS_BusNodeTreeView::nodeSelected,this,&BMS_BusNodesManagerView::nodeSelected);
    connect(_busNodeTreeView,&BMS_BusNodeTreeView::busSelected,_busManagerWidget,&BMS_BusManagerWidget::setBus);
    connect(_busNodeTreeView,&BMS_BusNodeTreeView::nodeSelected,_nodeManagerWidget,&BMS_NodeManagerWidget::setNode);

    //connect(_bcuManagerWidget,&BMS_BCUManagerwidget::nodeSelected,this,&BMS_BusNodesManagerView::nodeSelected);

    _poller = nullptr;
    //connect(_pollTimer,&QTimer::timeout,this,&BMS_BusNodesManagerView::pollProc);
}

CanOpen *BMS_BusNodesManagerView::canOpen() const
{
    return _canOpen;
}

void BMS_BusNodesManagerView::setCanOpen(CanOpen *canOpen)
{
    _canOpen = canOpen;

    _busNodeTreeView->setCanOpen(_canOpen);
    _busNodeTreeView->expandAll();

    //_bcuManagerWidget->setCanOpen(_canOpen);
    if(_canOpen != nullptr){
//        connect(_canOpen,&CanOpen::busAboutToBeAdded,this,&BMS_BCUManagerwidget::prepareAddBus);
        connect(_canOpen,&CanOpen::busAdded,this,&BMS_BusNodesManagerView::addBus);
//        connect(_canOpen,&CanOpen::busAboutToBeRemoved,this,&BMS_BCUManagerwidget::removeBus);
//        connect(_canOpen,&CanOpen::busRemoved,this,[=](){endRemoveRows();});
//        foreach (auto v, _bcus) {
//            v->deleteLater();
//        }
    }

}

void BMS_BusNodesManagerView::addBus(quint8 busId)
{
    CanOpenBus *bus = CanOpen::bus(busId);

    if(bus == nullptr){
        return;
    }
    connect(bus,&CanOpenBus::nodeAdded,this,[=](int nodeId){addBcu(bus,nodeId);});
    connect(bus,&CanOpenBus::nodeAboutToBeRemoved,this,[=](int nodeId){removeBcu(bus,nodeId);});
}

void BMS_BusNodesManagerView::addBcu(CanOpenBus *bus, quint8 id)
{
    Node *node = bus->node(id);

    if(node == nullptr){
        return;
    }

    BCU *bcu = static_cast<BCU*>(node);

    if(bcu == nullptr){
        return;
    }

    bool found = false;
    foreach (BCU *b, _bcus) {
        if(b == bcu){
            found = true;
        }
    }
    if(!found){
        _bcus.append(bcu);
        QVBoxLayout *bcuLayout = (QVBoxLayout*)_bcuGroup->layout();
        QString title = QString("BCU ID: %1").arg(bcu->nodeId());
        BMS_NodeManagerWidget *nm = new BMS_NodeManagerWidget();
        nm->setNode(bcu);
        _bcuWidgets.append(nm);
        bcuLayout->addWidget(nm);
        //connect(btn,&QPushButton::clicked,this,&BMS_BCUManagerwidget::setCurrentBcu);
        connect(nm,&BMS_NodeManagerWidget::nodeSelected,this,&BMS_BusNodesManagerView::nodeSelected);
    }
}

void BMS_BusNodesManagerView::removeBcu(CanOpenBus *bus, quint8 id)
{

}

void BMS_BusNodesManagerView::setFunction(int func)
{

}

CanOpenBus *BMS_BusNodesManagerView::currentBus() const
{
    return _busNodeTreeView->currentBus();
}

Node *BMS_BusNodesManagerView::currentNode() const
{
    return _busNodeTreeView->currentNode();
}

void BMS_BusNodesManagerView::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(2, 2, 2, 2);


    _busNodeTreeView = new BMS_BusNodeTreeView();
    _busManagerWidget = new BMS_BusManagerWidget();

    _busNodeTreeView->addBusAction(_busManagerWidget->actionExplore());
    _busNodeTreeView->addBusAction(_busManagerWidget->actionSyncOne());
    _busNodeTreeView->addBusAction(_busManagerWidget->actionSyncStart());
    _busNodeTreeView->addBusAction(_busManagerWidget->actionTogleConnect());

    _nodeManagerWidget = new BMS_NodeManagerWidget();
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionRemoveNode());
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionUpdateFirmware());
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionLoadEds());
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionReLoadEds());

    //_bcuManagerWidget = new BMS_BCUManagerwidget();

//    layout->addWidget(_nodeManagerWidget);
    //layout->addWidget(_bcuManagerWidget);

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(2);
    QToolBar *toolbar = new QToolBar("BMU Control");

    toolbar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _groupControl = new QActionGroup(this);
    _groupControl->setExclusive(true);
    _actionSingle = toolbar->addAction(tr("Single"));
    _actionSingle->setCheckable(false);
    _actionSingle->setIcon(QIcon(":/icons/img/icons8-update.png"));
    _actionSingle->setStatusTip(tr("Acquire BCU data onece"));
    connect(_actionSingle, &QAction::triggered, this, &BMS_BusNodesManagerView::single);

    _actionStart = _groupControl->addAction(tr("Start"));
    _actionStart->setCheckable(false);
    _actionStart->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _actionStart->setStatusTip(tr("Start Acquire BCU data"));
    connect(_actionStart, &QAction::triggered, this, &BMS_BusNodesManagerView::start);

    _actionStop = _groupControl->addAction(tr("Stop"));
    _actionStop->setCheckable(false);
    _actionStop->setIcon(QIcon(":/icons/img/icons8-stop.png"));
    _actionStop->setStatusTip(tr("Stop Acquire BCU data"));
    connect(_actionStop, &QAction::triggered, this, &BMS_BusNodesManagerView::stop);

    toolbar->addActions(_groupControl->actions());

    vlayout->addWidget(toolbar);

    BMS_NodeManagerWidget *nm;
    _bcuGroup = new QGroupBox();
//    for(int i=0;i<5;i++){
//        nm = new BMS_NodeManagerWidget();
//        vlayout->addWidget(nm);
//        _bcuWidgets.append(nm);
//    }
    _bcuGroup->setLayout(vlayout);
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setWidget(_bcuGroup);
    layout->addWidget(scroll);

    _sysManager = new BMS_SystemManagerWidget();
    connect(_sysManager,&BMS_SystemManagerWidget::validFunction,this,&BMS_BusNodesManagerView::functionSelected);
    layout->addWidget(_sysManager);


    layout->addWidget(_busNodeTreeView);
    layout->addWidget(_busManagerWidget);

    setLayout(layout);

}

BMS_NodeManagerWidget *BMS_BusNodesManagerView::nodeManagerWidget() const
{
    return _nodeManagerWidget;
}

void BMS_BusNodesManagerView::saveState(QSettings &settings)
{
    _busNodeTreeView->saveState(settings);
}

void BMS_BusNodesManagerView::restoreState(QSettings &settings)
{
    _busNodeTreeView->restoreState(settings);
}

BMS_BusManagerWidget *BMS_BusNodesManagerView::busManagerWidget() const
{
    return _busManagerWidget;
}

BMS_BusNodeTreeView *BMS_BusNodesManagerView::busNodeTreeView() const
{
    return _busNodeTreeView;
}

void BMS_BusNodesManagerView::pollProc()
{
//    BCU *b = _bcus[_currentBcuId++];
//    if(_currentBcuId == _bcus.count()){
//        _currentBcuId = 0;
//    }

//    for(quint8 i=0;i<0x04;i++){
//        b->readObject(0x2002,i);
//        QThread::msleep(50);
//    }

//    for(quint8 i=0;i<b->nofPacks();i++){
//        for(quint8 j=0;i<0x1F;j++){
//            b->readObject(0x2100+i,j);
//            QThread::msleep(50);
//        }
//    }
}

void BMS_BusNodesManagerView::startPoll(int interval)
{
    qDebug()<<Q_FUNC_INFO<<" Interval = "<<interval;
    if(_poller == nullptr && _bcus.count() > 0){
        _poller = new BMS_Poller(_bcus);
        _poller->setInterval(interval);
        _poller->start();
        connect(_poller,&BMS_Poller::finished,this,&BMS_BusNodesManagerView::handlePollerDone);
        qDebug()<<"Started";
    }
}

void BMS_BusNodesManagerView::stopPoll()
{
    //qDebug()<<Q_FUNC_INFO;
    if(_poller != nullptr){
//        QEventLoop loop;
//        connect(_poller,&BMS_Poller::finished,&loop,&QEventLoop::quit);
        //qDebug()<<"Stop Poller....";
        _poller->stop();
//        loop.exec();
        //qDebug()<<"Done";
    }
}

void BMS_BusNodesManagerView::handlePollerDone()
{
    if(_poller != nullptr){
        _poller->deleteLater();
        _poller = nullptr;
    }
}

void BMS_BusNodesManagerView::startBcu(int id)
{
    if(id == -1){
        foreach (BCU *b, _bcus) {
            b->sendStart();

        }
    }
    else if(id < _bcus.count()){
        _bcus[id]->sendStart();
    }
}

void BMS_BusNodesManagerView::stopBcu(int id)
{
    if(id == -1){
        foreach (BCU *b, _bcus) {
            b->sendStop();
        }
    }
    else if(id < _bcus.count()){
        _bcus[id]->sendStop();
    }
}

void BMS_BusNodesManagerView::preopBcu(int id)
{
    if(id == -1){
        foreach (BCU *b, _bcus) {
            b->sendPreop();
        }
    }
    else if(id < _bcus.count()){
        _bcus[id]->sendPreop();
    }
}

void BMS_BusNodesManagerView::reloadEds(int id)
{
    if(id == -1){
        foreach (BCU *b, _bcus) {
            b->loadEds(b->edsFileName());
        }
    }
    else if(id < _bcus.count()){
        _bcus[id]->loadEds(_bcus[id]->edsFileName());
    }
}
// poll once
void BMS_BusNodesManagerView::single()
{
    startPoll(0);
}

void BMS_BusNodesManagerView::start()
{
    startPoll(2000);
}

void BMS_BusNodesManagerView::stop()
{
    stopPoll();
}


/************* poll thread ************/
BMS_Poller::BMS_Poller(QObject *parent)
    :BMS_Poller(QList<BCU*>(),parent)
{

}

BMS_Poller::BMS_Poller(QList<BCU *> bcus, QObject *parent)
    :_bcus(bcus)
    ,QThread(parent)
{
    _interval = 50; // ms
}

void BMS_Poller::stop()
{
   _stop = true;
}

void BMS_Poller::setInterval(int interval)
{
    _interval = interval;
}

void BMS_Poller::run()
{
    if(_bcus.count() == 0) {

        return;
    }
    _stop = false;
    quint8 max_bcu = _bcus.count();
    quint8 packs;
    quint8 cells;
    quint8 ntcs;
    /*
     * readObject will send packet into a queue,
     * so no interval is necessary to insert
     * _interval simply indicate polling all bcus duration
     *
     */
    while(!_stop){
        foreach (BCU *b, _bcus) {
            if(b->status() == Node::Status::STARTED){
                // validate data first
                b->validate();

                packs = b->nofPacks();
                cells = b->nofCellsPerPack();
                ntcs = b->nofNtcsPerPack();
                for(int i=0;i<packs;i++){
                    for(int j=0;j<0x09;j++){
                        b->readObject(0x2100 + i,j + 0x01);
                    }
                    //QThread::msleep(_interval);
                    for(int j=0;j<cells;j++){
                        b->readObject(0x2100 + i,j+0x0A);
                        //QThread::msleep(_interval);
                    }
                    for(int j=0;j<ntcs;j++){
                        b->readObject(0x2100 + i,j+0x18);
                        //QThread::msleep(_interval);
                    }
                }

                // poll error
                b->readObject(0x1001,0x00);
                // stack state
                for(int i=1;i<5;i++){
                    b->readObject(0x2002,i);
                }

            }
        }
        if(_interval > 0){
            QThread::msleep(_interval);
        }
        else{
            _stop = true; // poll onece
        }
    }

    emit finished();
}

