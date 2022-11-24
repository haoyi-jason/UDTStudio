#include "bms_bcumanagerwidget.h"
#include <QFormLayout>
#include <QDebug>

BMS_BCUManagerwidget::BMS_BCUManagerwidget(QWidget *parent)
    :BMS_BCUManagerwidget(nullptr,parent)
{

}

BMS_BCUManagerwidget::BMS_BCUManagerwidget(BCU *bcu, QWidget *parent)
    :QWidget(parent)
{
    createWidgets();
    _canOpen = nullptr;
    //setCanOpen(bcu);
}

//CanOpen *BMS_BCUManagerwidget::canOpen() const
//{
//    return _canOpen;
//}

//void BMS_BCUManagerwidget::setCanOpen(CanOpen *canOpen)
//{
//    if(_canOpen != nullptr){
//        disconnect(_canOpen,nullptr,this, nullptr);
//    }

//    _canOpen = canOpen;
//    if(_canOpen != nullptr){
////        connect(_canOpen,&CanOpen::busAboutToBeAdded,this,&BMS_BCUManagerwidget::prepareAddBus);
//        connect(_canOpen,&CanOpen::busAdded,this,&BMS_BCUManagerwidget::addBus);
////        connect(_canOpen,&CanOpen::busAboutToBeRemoved,this,&BMS_BCUManagerwidget::removeBus);
////        connect(_canOpen,&CanOpen::busRemoved,this,[=](){endRemoveRows();});
//    }
//}

//CanOpenBus *BMS_BCUManagerwidget::currentBus() const
//{
//    if(_currentBcu != nullptr){
//        return _currentBcu->bus();
//    }

//    return nullptr;
//}

//void BMS_BCUManagerwidget::setBus(CanOpenBus *bus)
//{
//    if(_bus != nullptr){
//        disconnect(_bus,nullptr,this,nullptr);
//    }

//    _bus = bus;

//    if(_bus != nullptr){
//        connect(_bus,&CanOpenBus::connectedChanged,this,&BMS_BCUManagerwidget::updateBusData);
//        connect(_bus,&CanOpenBus::busNameChanged,this,&BMS_BCUManagerwidget::updateBusData);
//    }

//    _groupBox->setEnabled(_bus != nullptr);
//    updateBusData();
//}

BCU *BMS_BCUManagerwidget::bcu(uint8_t id) const
{
    foreach (BCU *b, _bcus) {
        if(b->busId() == id){
            return b;
        }
    }

    return nullptr;
}

void BMS_BCUManagerwidget::addBus(quint8 busId)
{
    CanOpenBus *bus = CanOpen::bus(busId);

    if(bus == nullptr){
        return;
    }

//    connect(bus,&CanOpenBus::nodeAdded,this,[=](int nodeId){addBcu(bus,nodeId);});
//    connect(bus,&CanOpenBus::nodeAboutToBeRemoved,this,[=](int nodeId){removeBcu(bus,nodeId);});
}

//void BMS_BCUManagerwidget::addBcu(BCU *bcu)
//{
//    if(bcu == nullptr){
//        return;
//    }

//    bool found = false;
//    foreach (BCU *b, _bcus) {
//        if(b == bcu){
//            found = true;
//        }
//    }
//    if(!found){
//        _bcus.append(bcu);
//        //updateBusData();
//        QVBoxLayout *bcuLayout = (QVBoxLayout*)_bcuGroup->layout();
//        QString title = QString("BCU ID: %1").arg(bcu->nodeId());
//        QPushButton *btn = new QPushButton(title);
//        btn->setProperty("ID",bcu->nodeId());
//        _bcuButtons.append(btn);
//        bcuLayout->addWidget(btn);
//        connect(btn,&QPushButton::clicked,this,&BMS_BCUManagerwidget::setCurrentBcu);
//    }
//}

void BMS_BCUManagerwidget::setCurrentBcu()
{
    QPushButton *btn = (QPushButton*)sender();
    int id = (btn->property("ID").toInt());

    foreach (BCU *bcu, _bcus) {
        if(bcu->nodeId() == id){
            emit nodeSelected((Node*)bcu);
        }
    }
}

//void BMS_BCUManagerwidget::addBcu(CanOpenBus *bus, quint8 id)
//{
//    qDebug()<<Q_FUNC_INFO;
//    Node *node = bus->node(id);

//    if(node != nullptr){
//        BCU *b = static_cast<BCU*>(node);
//        addBcu(b);
//    }
//}

//void BMS_BCUManagerwidget::removeBcu(BCU *bcu)
//{
//    if(bcu == nullptr){
//        return;
//    }

//    foreach (BCU *b, _bcus) {
//        if(b == bcu){
//            _bcus.removeOne(bcu);
//        }
//    }
//}

//void BMS_BCUManagerwidget::removeBcu(CanOpenBus *bus, quint8 id)
//{
//    Node *node = bus->node(id);

//    if(node == nullptr){
//        return;
//    }

//    BCU *bcu = static_cast<BCU*>(node);
//    removeBcu(bcu);
//}

void BMS_BCUManagerwidget::updateBusData()
{
    qDebug()<<Q_FUNC_INFO;
//    if(_bus != nullptr){
//        _actionTogleConnect->blockSignals(true);
//        _actionTogleConnect->setEnabled(true);
//        _actionTogleConnect->setCheckable(currentBus()->isConnected());
//        _actionTogleConnect->setText(currentBus()->isConnected()?tr("Disconnect"):tr("Connect"));
//        _actionTogleConnect->blockSignals(false);

//        _actionTogleSync->blockSignals(true);
//        actionSyncStart()->setChecked(_bus->sync()->status() == Sync::STARTED);
//        _actionTogleSync->blockSignals(false);

//        _actionTogleSync->setEnabled(_bus->isConnected());
//        _bcuIDEdit->setText(_bus->driverAddress());

//        QVBoxLayout *bcuLayout = (QVBoxLayout*)_bcuGroup->layout();
//        bcuLayout->setContentsMargins(2,2,2,2);
//        bcuLayout->setSpacing(2);

//        foreach (QPushButton *btn, _bcuButtons) {
//            btn->deleteLater();
//        }


//        foreach (BCU *b, _bcus) {
//            QString title = QString("ID: %1").arg(b->busId());
//            QPushButton *btn = new QPushButton(title);

//            _bcuButtons.append(btn);
//            bcuLayout->addWidget(btn);
//        }
//        _bcuGroup->updateGeometry();


//    }

//    else if(_bus == nullptr){
//        _actionTogleConnect->setChecked(false);
//        _actionTogleConnect->setEnabled(false);

//        _actionTogleSync->setEnabled(false);
//    }
}

//QAction *BMS_BCUManagerwidget::actionSyncStart() const
//{
//    return _actionTogleSync;
//}

//QAction *BMS_BCUManagerwidget::actionTogleConnect() const
//{
//    return _actionTogleConnect;
//}

//void BMS_BCUManagerwidget::toggleConnect()
//{
//    if(_bus != nullptr){
//        if(_bus->canBusDriver() != nullptr){
//            if(_bus->isConnected()){
//                _bus->canBusDriver()->disconnectDevice();
//            }
//            else{
//                _bus->canBusDriver()->connectDevice();
//            }
//        }
//    }
//}

//void BMS_BCUManagerwidget::toggleSync(bool start)
//{
//    if(_bus != nullptr){
//        if(start){
//            _bus->sync()->startSync(100);
//        }
//        else{
//            _bus->sync()->stopSync();
//        }
//    }
//}

void BMS_BCUManagerwidget::setBusName()
{
    if(_bus != nullptr){
        _bus->setBusName(tr("BUS Name"));
    }
}

void BMS_BCUManagerwidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    _groupBox = new QGroupBox(tr("BCU"));
    QHBoxLayout *layoutGroupbox = new QHBoxLayout();
    layoutGroupbox->setSpacing(2);
    layoutGroupbox->setContentsMargins(2,2,2,2);

    _actionTogleConnect = new QAction();
    _actionTogleSync = new QAction();

//    QPushButton *button;

//    button = new QPushButton();
//    button->setText("BUTTON-1");
//    layoutGroupbox->addWidget(button);

    _groupBox->setLayout(layoutGroupbox);

    _bcuGroup = new QGroupBox();
    QVBoxLayout *bcuLayout = new QVBoxLayout();
    bcuLayout->setContentsMargins(2,2,2,2);
    bcuLayout->setSpacing(2);
    _bcuButtons.clear();
    foreach (BCU *b, _bcus) {
        QString title = QString("ID: %1").arg(b->busId());
        QPushButton *btn = new QPushButton(title);
        _bcuButtons.append(btn);
        bcuLayout->addWidget(btn);
    }

//    for(int i=0;i<5;i++){
//        QString title = QString("ID: %1").arg(i+1);
//        QPushButton *btn = new QPushButton(title);
//        _bcuButtons.append(btn);
//        bcuLayout->addWidget(btn);
//    }

    _bcuGroup->setLayout(bcuLayout);
    layout->addWidget(_bcuGroup);

    setLayout(layout);

}

//void BMS_BCUManagerwidget::setBCUId()
//{

//}

void BMS_BCUManagerwidget::setSyncTimer(int i)
{

}
