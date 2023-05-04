#include "bms_busnodemanagerview.h"

#include <QLayout>
#include <QScrollArea>
#include <QEventLoop>
#include <QDebug>
#include <QVBoxLayout>

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

    connect(_busNodeTreeView,&BusNodesTreeView::busSelected,this,&BMS_BusNodesManagerView::busSelected);
    connect(_busNodeTreeView,&BusNodesTreeView::nodeSelected,this,&BMS_BusNodesManagerView::setNode);
    connect(_busNodeTreeView,&BusNodesTreeView::busSelected,_busManagerWidget,&BMS_BusManagerWidget::setBus);
    connect(this,&BMS_BusNodesManagerView::bcuSelected,_nodeManagerWidget,&BMS_NodeManagerWidget::setBCU);
    //connect(_bcuManagerWidget,&BMS_BCUManagerwidget::nodeSelected,this,&BMS_BusNodesManagerView::nodeSelected);

    //connect(_bcuManagerWidget,&BMS_BCUManagerwidget::nodeSelected,this,&BMS_BusNodesManagerView::nodeSelected);

    _poller = nullptr;
    //connect(_pollTimer,&QTimer::timeout,this,&BMS_BusNodesManagerView::pollProc);
    _logger = new BMS_Logger();
    _logger->startLog(10);

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


    bool found = false;
    QMap<Node*,BCU*>::const_iterator it = _bcusMap.constFind(node);
    if(it != _bcusMap.constEnd()){
        return;
    }
    BCU *bcu = new BCU(node,false);
    _bcusMap.insert(node,bcu);
    connect(bcu,&BCU::stateChanged,this,&BMS_BusNodesManagerView::bcuStateChanged);
    connect(node,&Node::nameChanged,this,&BMS_BusNodesManagerView::nodeNameChanged);

    _logger->addBCU(bcu);

    //    QVBoxLayout *bcuLayout = (QVBoxLayout*)_bcuGroup->layout();
//    QString title = QString("BCU ID: %1").arg(node->nodeId());
//    bcu->readConfig();
}

void BMS_BusNodesManagerView::nodeNameChanged(QString name)
{
    qDebug()<<Q_FUNC_INFO;
    //UNUSED(name);
    Node *node = static_cast<Node*>(sender());
    QMap<Node*, BCU*>::const_iterator it = _bcusMap.constFind(node);
    if(it != _bcusMap.constEnd()){
        it.value()->readConfig();
        it.value()->node()->sendStart();
    }
}

void BMS_BusNodesManagerView::removeBcu(CanOpenBus *bus, quint8 id)
{
    QMap<Node*,BCU*>::ConstIterator it = _bcusMap.constBegin();
    foreach(Node *n, _bcusMap.keys()){
        if(n->nodeId() == id && n->bus() == bus){
            _logger->removeBCU(_bcusMap.value(n));
            _bcusMap.value(n)->deleteLater();
            _bcusMap.remove(n);
        }
    }
//    foreach(Node *n,_nodes){
//        if(n->nodeId() == id && n->bus() == bus){
//            _nodes.removeOne(n);
//        }
//    }
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
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(10);
    //layout->setContentsMargins(2, 2, 2, 2);


    _busNodeTreeView = new BusNodesTreeView();
    _busManagerWidget = new BMS_BusManagerWidget();

    _busNodeTreeView->addBusAction(_busManagerWidget->actionExplore());
    _busNodeTreeView->addBusAction(_busManagerWidget->actionSyncOne());
    _busNodeTreeView->addBusAction(_busManagerWidget->actionSyncStart());
    _busNodeTreeView->addBusAction(_busManagerWidget->actionTogleConnect());

    _nodeManagerWidget = new BMS_NodeManagerWidget();
//    _nodeManagerWidget = new NodeManagerWidget();
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionRemoveNode());
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionUpdateFirmware());
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionLoadEds());
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionReLoadEds());

    //_bcuManagerWidget = new BMS_BCUManagerwidget();

    //layout->addWidget(_bcuManagerWidget);

//    QVBoxLayout *vlayout = new QVBoxLayout();
//    vlayout->setContentsMargins(0,0,0,0);
//    vlayout->setSpacing(2);
//    QToolBar *toolbar = new QToolBar("BCU Control");

//    toolbar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
//    _groupControl = new QActionGroup(this);
//    _groupControl->setExclusive(true);
//    _actionSingle = toolbar->addAction(tr("Single"));
//    _actionSingle->setCheckable(false);
//    _actionSingle->setIcon(QIcon(":/icons/img/icons8-update.png"));
//    _actionSingle->setStatusTip(tr("Acquire BCU data onece"));
//    connect(_actionSingle, &QAction::triggered, this, &BMS_BusNodesManagerView::single);

//    _actionStart = _groupControl->addAction(tr("Start"));
//    _actionStart->setCheckable(false);
//    _actionStart->setIcon(QIcon(":/icons/img/icons8-play.png"));
//    _actionStart->setStatusTip(tr("Start Acquire BCU data"));
//    connect(_actionStart, &QAction::triggered, this, &BMS_BusNodesManagerView::start);

//    _actionStop = _groupControl->addAction(tr("Stop"));
//    _actionStop->setCheckable(false);
//    _actionStop->setIcon(QIcon(":/icons/img/icons8-stop.png"));
//    _actionStop->setStatusTip(tr("Stop Acquire BCU data"));
//    connect(_actionStop, &QAction::triggered, this, &BMS_BusNodesManagerView::stop);

//    _actionScan = _groupControl->addAction(tr("Scan"));
//    _actionScan->setCheckable(false);
//    _actionScan->setIcon(QIcon(":/icons/img/icons8-stop.png"));
//    _actionScan->setStatusTip(tr("Scan BCUs on the bus"));
//    connect(_actionScan, &QAction::triggered, this, &BMS_BusNodesManagerView::scanBus);

//    toolbar->addActions(_groupControl->actions());

//    vlayout->addWidget(toolbar);

//    BMS_NodeManagerWidget *nm;
//    _bcuGroup = new QGroupBox();
//    _bcuGroup->setLayout(vlayout);
    //layout->addWidget(scroll);

    _sysManager = new BMS_SystemManagerWidget();
    connect(_sysManager,&BMS_SystemManagerWidget::validFunction,this,&BMS_BusNodesManagerView::functionSelected);



    QScrollArea *scroll = new QScrollArea();

    scroll->setWidgetResizable(true);
    scroll->setWidget(_busNodeTreeView);


    layout->addWidget(_nodeManagerWidget);
    layout->addWidget(_busManagerWidget);
    layout->addWidget(scroll);
    layout->addWidget(_sysManager);

    setLayout(layout);

}

BMS_NodeManagerWidget *BMS_BusNodesManagerView::nodeManagerWidget() const
{
//    return _nodeManagerWidget;
    return nullptr;
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

BusNodesTreeView *BMS_BusNodesManagerView::busNodeTreeView() const
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
    foreach (BCU *b, _bcusMap.values()) {
        b->startPoll(interval);
    }
//    if(_poller == nullptr && _bcus.count() > 0){
//        _poller = new BMS_Poller(_bcus);
//        _poller->setInterval(interval);
//        _poller->start();
//        connect(_poller,&BMS_Poller::finished,this,&BMS_BusNodesManagerView::handlePollerDone);
//        qDebug()<<"Started";
//    }
}

void BMS_BusNodesManagerView::stopPoll()
{
    foreach (BCU *b, _bcusMap.values()) {
        b->stopPoll();
    }
    //qDebug()<<Q_FUNC_INFO;
//    if(_poller != nullptr){
//        QEventLoop loop;
//        connect(_poller,&BMS_Poller::finished,&loop,&QEventLoop::quit);
        //qDebug()<<"Stop Poller....";
//        _poller->stop();
//        loop.exec();
        //qDebug()<<"Done";
//    }
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
        foreach (Node *n, _bcusMap.keys()) {
            n->sendStart();
        }
    }
    else if(id < _bcusMap.count()){
        _bcusMap.keys().at(id)->sendStart();
    }
}

void BMS_BusNodesManagerView::stopBcu(int id)
{
    if(id == -1){
        foreach (Node *n, _bcusMap.keys()) {
            n->sendStop();
        }
    }
    else if(id < _bcusMap.count()){
        _bcusMap.keys().at(id)->sendStop();
    }
}

void BMS_BusNodesManagerView::preopBcu(int id)
{
    if(id == -1){
        foreach (Node *n, _bcusMap.keys()) {
            n->sendPreop();
        }
    }
    else if(id < _bcusMap.count()){
        _bcusMap.keys().at(id)->sendPreop();
    }
}

void BMS_BusNodesManagerView::reloadEds(int id)
{
    if(id == -1){
        foreach (Node *n, _bcusMap.keys()) {
            n->loadEds(n->edsFileName());
        }
    }
    else if(id < _bcusMap.count()){
        _bcusMap.keys().at(id)->loadEds(_bcusMap.keys().at(id)->edsFileName());
    }
}
// poll once
void BMS_BusNodesManagerView::single()
{
    startPoll(0);
}

void BMS_BusNodesManagerView::start()
{
    //startPoll(2000);
    foreach (BCU *b, _bcusMap.values()) {
        b->startPoll();
    }
}

void BMS_BusNodesManagerView::stop()
{
    //stopPoll();
    foreach (BCU *b, _bcusMap.values()) {
        b->stopPoll();
    }
}

void BMS_BusNodesManagerView::scanBus()
{
    foreach (CanOpenBus *b, CanOpen::buses()) {
        //b->exploreBus();
//        foreach (Node *n, b->nodes()) {
            //BCU *b = static_cast<BCU*> (n);
            //b->readConfig();
            //b->identify();
//        }
    }
}

void BMS_BusNodesManagerView::bcuStateChanged()
{

}

void BMS_BusNodesManagerView::setNode(Node *node)
{
    QMap<Node*, BCU*>::const_iterator it = _bcusMap.constFind(node);
    if(it == _bcusMap.constEnd()) {
        emit nodeSelected(nullptr);
        emit bcuSelected(nullptr);
        return;
    }
    emit nodeSelected(it.key());
    emit bcuSelected(it.value());
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

    foreach (BCU *b, _bcus) {
        _bcuQueue.enqueue(b);
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
    bool removeBcu = false;
    while(!_stop){
//        foreach (BCU *b, _bcus) {
//            if(b->canPoll()){
////            if(b->status() == Node::Status::STARTED){
//                // validate data first
//                b->validate();


//                packs = b->nofPacks();
//                cells = b->nofCellsPerPack();
//                ntcs = b->nofNtcsPerPack();
//                for(int i=0;i<packs;i++){
//                    for(int j=0;j<0x09;j++){
//                        if( !removeBcu && (b->nodeOd()->errorObject(0x2100 + i,j+0x01) == 0)){
//                            b->readObject(0x2100 + i,j + 0x01);
//                        }
//                        else{
//                            removeBcu = true;
//                            continue;
//                        }
//                        //QThread::msleep(5);
//                    }
//                    for(int j=0;j<cells;j++){
//                        if(!removeBcu && (b->nodeOd()->errorObject(0x2100 + i,j+0x0A) == 0)){
//                            b->readObject(0x2100 + i,j+0x0A);
//                        }
//                        else{
//                            removeBcu = true;
//                            continue;
//                        }
//                        //QThread::msleep(5);
//                    }
//                    for(int j=0;j<ntcs;j++){
//                        if(!removeBcu && (b->nodeOd()->errorObject(0x2100 + i,j+0x18) == 0)){
//                            b->readObject(0x2100 + i,j+0x18);
//                        }
//                        else{
//                            removeBcu = true;
//                            continue;
//                        }
//                        //QThread::msleep(5);
//                    }
//                }

//                if(!removeBcu){
//                    // poll error
//                    b->readObject(0x1001,0x00);
//                    // stack state
//                    for(int i=1;i<5;i++){
//                        b->readObject(0x2002,i);
//                    }
//                }
//                else{
//                    _bcus.removeOne(b);
//                    removeBcu = false;
//                    emit error();
//                }

//            }
//            if(_bcus.count() == 0){
//                _stop = true;
//                break;
//            }
//        }
//        if(_interval > 0){
//            QThread::msleep(_interval);
//        }
//        else{
//            _stop = true; // poll onece
//        }
    }

    emit finished();
}

