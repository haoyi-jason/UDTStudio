#include "bms_stackmanager.h"
#include <QDebug>

BMS_StackManager::BMS_StackManager()
{
    _logger = new BMS_Logger();
    _currentBcuId = -1;
    _maxCV =0;
    _minCV = 0;
    _maxCVID =0;
    _minCVID = 0;
    _maxCT =0;
    _minCT = 0;
    _maxCTID =0;
    _minCTID = 0;
    _cvDiff = 0;
    _packVoltage = 0;
    _totalCurrent = 0;
    _maxCVPID = 0;
    _minCVPID = 0;
    _maxCTPID = 0;
    _minCTPID = 0;

    _tmr_statemachine = new QTimer();
    connect(_tmr_statemachine,&QTimer::timeout,this,&BMS_StackManager::pollState);
    _tmr_statemachine->start(50);

    _bcuIterator = _bcusMap.end();
}

void BMS_StackManager::setCanOpen(CanOpen *canoopen)
{
    _canopen = canoopen;
    if(_canopen != nullptr){
        connect(_canopen,&CanOpen::busAdded,this,&BMS_StackManager::addBus);
    }
}

void BMS_StackManager::pollState()
{
    if(_bcusMap.size() > 0){
        if(_bcuIterator == _bcusMap.end()){
            _bcuIterator = _bcusMap.begin();
        }
        //BCU *b = _bcuIterator.value();
        BCU *b = _bcusMap.first();
        if(!b->isConfigReady() && !b->isConfigFail()){
            b->accessConfig();
        }

        _bcuIterator++;
    }
}

void BMS_StackManager::scanBus()
{
    foreach (CanOpenBus *b, CanOpen::buses()) {
        b->exploreBus();
    }
}

void BMS_StackManager::startActivity()
{

}

void BMS_StackManager::addBus(quint8 busId)
{
    CanOpenBus *bus = CanOpen::bus(busId);
    if(!_canopenbus.contains(bus) && bus != nullptr){
        _canopenbus.append(bus);
        connect(bus,&CanOpenBus::nodeAdded,this,&BMS_StackManager::addBcu);
        connect(bus,&CanOpenBus::nodeAboutToBeRemoved,this,&BMS_StackManager::removeBcu);
    }
}

void BMS_StackManager::addBcu(quint8 nodeId)
{
    qDebug()<<Q_FUNC_INFO;
    CanOpenBus *bus = static_cast<CanOpenBus*>(sender());

    Node *node = bus->node(nodeId);
    if(node == nullptr){
        return;
    }

    bool found = false;
    QMap<Node*,BCU*>::const_iterator it = _bcusMap.constFind(node);
    if(it != _bcusMap.constEnd()){
        return;
    }

    BCU *b = new BCU(node,false);
    _bcusMap.insert(node,b);
    //connect(node,&Node::nameChanged,this,&BMS_StackManager::nodeNameChanged);
    connect(b,&BCU::configReady,this,&BMS_StackManager::bcuConfigReady);

    if(_logger != nullptr){
        _logger->addBCU(b);
    }
    if(_currentBcuId == -1){
        _currentBcuId = 0;
        emit activeBcuChannged(b);
    }
    //qDebug()<<Q_FUNC_INFO<<_currentBcuId;

}

void BMS_StackManager::removeBcu(quint8 nodeId)
{
    CanOpenBus *bus = static_cast<CanOpenBus*>(sender());
    Node *node = bus->node(nodeId);
//    QMap<Node*,BCU*>::ConstIterator it = _bcusMap.constBegin();
    if(node == nullptr) return;
    for(auto m = _bcusMap.cbegin(),e = _bcusMap.cend();m != e;++m){
        Node *n = m.key();
        BCU *b = m.value();
        if((n == node) && (n->bus() == bus)){
            if(_logger != nullptr){
                _logger->removeBCU(m.value());
            }
            b->deleteLater();
            _bcusMap.remove(n);
            break;
        }
    }

//    foreach(Node *n, _bcusMap.keys()){
//        if((n == node) && (n->bus() == bus)){
//            if(_logger != nullptr){
//                _logger->removeBCU(_bcusMap.value(n));
//            }
//            _bcusMap.value(n)->deleteLater();
////            _bcusMap.remove(n);
//            break;
//        }
//    }
    if(_bcusMap.size() == 0){
        _currentBcuId = -1;
    }
    else{
        prevBcu();
    }
}

void BMS_StackManager::bcuConfigReady()
{
    BCU *bcu = static_cast<BCU*>(sender());
    Node *node = nullptr;

    QMap<Node*,BCU*>::ConstIterator it = _bcusMap.constBegin();

    foreach(BCU *b, _bcusMap.values()){
        if(b == bcu){
            node = _bcusMap.key(bcu);
        }
    }
    if(node != nullptr){
        AlarmManager *alm = new AlarmManager(bcu->nofPacks(),bcu->nofCellsPerPack(),bcu->nofNtcsPerPack());
        bcu->setAlarmManager(alm);
        //_alarmManager.append(alm);
        node->sendStart();
        foreach (PDO *p, node->tpdos()) {
            p->setEnabled(true);
        }
        connect(bcu,&BCU::dataAccessed,this,&BMS_StackManager::bcuDataAccessed);
        // read control state
        node->readObject(0x2003,0x01);
    }

}

void BMS_StackManager::nodeNameChanged(QString name)
{
//    qDebug()<<Q_FUNC_INFO;
//    Node *node = static_cast<Node*>(sender());
//    QMap<Node*, BCU*>::const_iterator it = _bcusMap.constFind(node);
//    if(it != _bcusMap.constEnd()){
//        it.value()->readConfig();
//    }
}

void BMS_StackManager::bcuDataAccessed()
{
    BCU *bcu = static_cast<BCU*>(sender());
    validateState(bcu);
}

BCU *BMS_StackManager::bcu()
{
    //qDebug()<<Q_FUNC_INFO;
    BCU *b = nullptr;
    if(_currentBcuId >= 0){
        b = _bcusMap.values().at(_currentBcuId);
    }
    return b;
}

BCU *BMS_StackManager::nextBcu()
{
    _currentBcuId++;
    if(_currentBcuId >= _bcusMap.size()){
        _currentBcuId--;
    }
    BCU *b = bcu();
    emit activeBcuChannged(b);
    return b;
}

BCU *BMS_StackManager::prevBcu()
{
    _currentBcuId--;
    if(_currentBcuId < 0){
        _currentBcuId++;
    }
    BCU *b = bcu();
    emit activeBcuChannged(b);
    return b;
}

int BMS_StackManager::currentBcuId() const
{
    return _currentBcuId;
}

int BMS_StackManager::totalBcus()
{
    return _bcusMap.size();
}
/*
 *  validate system alarm status
 */
void BMS_StackManager::validateState(BCU *b)
{
    Node *n = _bcusMap.key(b);
    if(n == nullptr) return;

//    for(auto m = _bcusMap.cbegin(), end = _bcusMap.cend(); m != end;++m){
        //Node *n = m.key();
        //BCU *b = m.value();
        AlarmManager *a = b->alarmManager();

        for(int i=0;i<b->nofPacks();i++){
            for(int j=0;j<b->nofCellsPerPack();j++){
                a->set_cell_voltage(i * b->nofCellsPerPack() + j,n->nodeOd()->value(0x2100 + i,j + 0x0a).toDouble());
            }
        }

        for(int i=0;i<b->nofPacks();i++){
            for(int j=0;j<b->nofNtcsPerPack();j++){
                a->set_cell_temperature(i*b->nofNtcsPerPack() + j,n->nodeOd()->value(0x2100 + i,j + 0x1a).toDouble()/10.);
            }
        }
        a->set_soc(n->nodeOd()->value(0x2102,0x01).toDouble());
        a->set_pack_voltage(n->nodeOd()->value(0x2002,0x03).toDouble()/10.);
        a->set_pack_current(n->nodeOd()->value(0x2002,0x04).toDouble()/10.);

        // todo : add alarm output function to fire output
        quint32 org = n->nodeOd()->value(0x6300,0x01).toInt();
        quint32 out = 0x0;
        if(a->isAlarm()){
            out = 0x02;
        }
        else if(a->isWarning()){
            out = 0x01;
        }

        if(out != org){
            QVariant valueToWrite;
            valueToWrite.setValue(out);
            //qDebug()<< Q_FUNC_INFO << " :Issue digital output";
            n->writeObject(0x6300,0x01, valueToWrite);
        }
//    }


    // loop through each alarm manager to find max/min
    double maxcv = 0;
    double mincv = 10000;
    double maxct = -100;
    double minct = 1000;
    int maxcv_p = -1;
    int maxcv_c = -1;
    int mincv_p = -1;
    int mincv_c = -1;
    int maxct_p = -1;
    int maxct_n = -1;
    int minct_p = -1;
    int minct_n = -1;
    double cvdiff = 100;

    int packId = 1;
    foreach(BCU *b,_bcusMap.values()){
        AlarmManager *m = b->alarmManager();
        if(m->maxCv() > maxcv){
            maxcv = m->maxCv();
            maxcv_p = m->maxCvPos()/b->nofCellsPerPack();
            maxcv_c = m->maxCvPos()%b->nofCellsPerPack();
        }
        if(m->minCv() < mincv){
            mincv = m->minCv();
            mincv_p = m->minCvPos()/b->nofCellsPerPack();
            mincv_c = m->minCvPos()%b->nofCellsPerPack();
        }
        if(m->maxCt() > maxct){
            maxct = m->maxCt();
            maxct_p = m->maxCtPos()/b->nofNtcsPerPack();
            maxct_n = m->maxCtPos()%b->nofNtcsPerPack();
        }
        if(m->minCt() < minct){
            minct = m->minCt();
            minct_p = m->minCtPos()/b->nofNtcsPerPack();
            minct_n = m->minCtPos()%b->nofNtcsPerPack();
        }
        packId++;
    }
    _maxCV = maxcv;
    _maxCVID = maxcv_c+1;
    _minCV = mincv;
    _minCVID = mincv_c+1;
    _maxCT = maxct;
    _maxCTID = maxct_n+1;
    _minCT = minct;
    _minCTID = minct_n+1;
    _cvDiff = _maxCV - _minCV;

    _maxCVPID = maxcv_p+1 ;
    _minCVPID = mincv_p+1 ;
    _maxCTPID = maxct_p+1 ;
    _minCTPID = minct_p+1 ;

    emit statusUpdated();

}

void BMS_StackManager::clearAlarm()
{
    foreach(BCU *b,_bcusMap.values()){
        AlarmManager *m = b->alarmManager();
        m->resetState();
    }
}

double BMS_StackManager::maxCV() const
{
    return _maxCV;
}
double BMS_StackManager::minCV() const
{
    return _minCV;
}
double BMS_StackManager::maxCT() const
{
    return _maxCT;
}
double BMS_StackManager::minCT() const
{
    return _minCT;
}
double BMS_StackManager::cvDiff() const
{
    return _cvDiff;
}
int BMS_StackManager::maxCvPos() const
{
    return _maxCVID;
}
int BMS_StackManager::minCvPos() const
{
    return _minCVID;
}
int BMS_StackManager::maxCtPos() const
{
    return _maxCTID;
}
int BMS_StackManager::minCtPos() const
{
    return _minCTID;
}

int BMS_StackManager::maxCvPID() const
{
    return _maxCVPID;
}
int BMS_StackManager::minCvPID() const
{
    return _minCVPID;
}
int BMS_StackManager::maxCtPID() const
{
    return _maxCTPID;
}
int BMS_StackManager::minCtPID() const
{
    return _minCTPID;
}
double BMS_StackManager::packVoltage() const
{
    return _packVoltage;
}
double BMS_StackManager::packCurrent() const
{
    return _totalCurrent;
}

bool BMS_StackManager::setCurrent(Node *node)
{
    QMap<Node*, BCU*>::const_iterator it = _bcusMap.constFind(node);
    if(it != _bcusMap.constEnd()){
        _currentBcuId = _bcusMap.keys().indexOf(node);
        emit activeBcuChannged(bcu());
        return true;
    }
    return false;
}


//void BMS_StackManager::validate()
//{

//}
