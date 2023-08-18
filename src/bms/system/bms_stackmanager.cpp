#include "bms_stackmanager.h"
#include "gsettings.h"
#include <QDebug>
#include <QAction>
#include <QStorageInfo>
#include <QDateTime>

BMS_StackManager::BMS_StackManager()
{
    _logger = new BMS_Logger();
    // test file move
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

    _pollCounter = 0;
    _tmr_statemachine = new QTimer();
    connect(_tmr_statemachine,&QTimer::timeout,this,&BMS_StackManager::pollState);
    _tmr_statemachine->start(50);

    _tmr_scanbus = new QTimer();
    connect(_tmr_scanbus,&QTimer::timeout,this,&BMS_StackManager::scanDone);

    _bcuIterator = _bcusMap.end();

    // modbus slave
#ifdef Q_OS_UNIX
    GSettings::instance().Info("Start MODBUS");
    _mbSlave = new BMS_ModbusSlave();
    if(GSettings::instance().bcuSection()->mb_rtu_enabled()){
        GSettings::instance().Info("MODBUS RTU Enabled");
        _mbSlave->startRTUSlave(GSettings::instance().bcuSection()->mb_rtu_connection());
    }
    if(GSettings::instance().bcuSection()->mb_tcp_enabled()){
        GSettings::instance().Info("MODBUS TCP Enabled");
        _mbSlave->startTCPServer("localhost",GSettings::instance().bcuSection()->mb_tcp_port());
    }
#endif
    QTimer::singleShot(24*60*60*1000,this,&BMS_StackManager::dailyTimeout);
    _enableBalance = false;
}

BMS_StackManager::~BMS_StackManager()
{
#ifdef Q_OS_UNIX
    if(GSettings::instance().bcuSection()->mb_rtu_enabled()){

    }
    if(GSettings::instance().bcuSection()->mb_tcp_enabled()){

    }
#endif

    // actions
    _actScanBus = new QAction("TEST");
    _actScanBus->setIcon(QIcon(":/icons/img/icons8-search-database.png"));
    _actScanBus->setEnabled(true);
    _actScanBus->setStatusTip("Scan buses");
    connect(_actScanBus,&QAction::triggered,this,&BMS_StackManager::scanBus);
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

        _mtx_poll.lock();
        BCU *b = _bcuIterator.value();
        //BCU *b = _bcusMap.first();

        if(b != nullptr && !b->isConfigReady()){
//        if(b != nullptr && !b->isConfigReady() && !b->isConfigFail()){
            b->accessConfig();
        }


        if(b->dataReady()){
            validateState(b);
            _mbSlave->updateBcuData(b);
            //b->notifyUpdate();
        }
        _mtx_poll.unlock();
        _bcuIterator++;
    }

    _pollCounter++;
    if(_pollCounter == 20){
        _pollCounter = 0;
        updateStackStatus();
    }
}

void BMS_StackManager::scanDone()
{
    _tmr_statemachine->start(50);
    _logger->startLog(GSettings::instance().bcuSection()->log_interval());
    _currentBcuId = -1;
//    if(_bcusMap.count() > 0){
//        nextBcu();
//    }
//    if(_bcusMap.size() > 0){
//        _currentBcuId = 0;
//    }
}

void BMS_StackManager::dailyTimeout()
{
    if(!GSettings::instance().bcuSection()->move_to_path().isEmpty()){
        _logger->movdLogFiles(GSettings::instance().bcuSection()->move_to_path());
    }

    QTimer::singleShot(24*60*60*1000,this,&BMS_StackManager::dailyTimeout);
}

void BMS_StackManager::scanBus()
{
    _tmr_statemachine->stop();
    _logger->stopLog();
    _mtx_poll.lock();
    for(int i=0;i<_bcusMap.size();i++){
        Node *n = _bcusMap.keys().at(i);
        BCU *b = _bcusMap.values().at(i);
        delete b;
    //    _bcusMap.remove(n);
    }
    _bcusMap.clear();
    _bcuIterator = _bcusMap.begin();
    _mtx_poll.unlock();
    bool scan = false;
    foreach (CanOpenBus *b, CanOpen::buses()) {
        b->exploreBus();
        scan = true;
    }
    if(scan){
        QTimer::singleShot(20000,this,&BMS_StackManager::scanDone);
        emit updateStatusText("裝置掃瞄中...",0);
        emit uiControl(false);
    }
    else{
        emit updateStatusText("沒有找到通訊裝置",0);
    }
}

void BMS_StackManager::enableBalance()
{

    foreach (BCU *b, _bcusMap.values()) {
        b->node()->writeObject(0x2003,0x07,_enableBalance?0x0:0x2);
    }
    _enableBalance = !_enableBalance;
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
    qDebug()<<Q_FUNC_INFO<<nodeId;
    CanOpenBus *bus = static_cast<CanOpenBus*>(sender());

    Node *node = bus->node(nodeId);
    if(node == nullptr){
        return;
    }

    //bool found = false;
    QMap<Node*,BCU*>::const_iterator it = _bcusMap.constFind(node);
    if(it != _bcusMap.constEnd()){
        return;
    }

    BCU *b = new BCU(node,true);
    _bcusMap.insert(node,b);
    //connect(node,&Node::nameChanged,this,&BMS_StackManager::nodeNameChanged);
    connect(b,&BCU::configReady,this,&BMS_StackManager::bcuConfigReady);
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
            _mtx_poll.lock();
            if(_logger != nullptr){
                _logger->removeBCU(m.value());
            }
            b->deleteLater();
            //b = nullptr;
            _bcusMap.remove(n);
            _mtx_poll.unlock();
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
    BCU *bu = static_cast<BCU*>(sender());
    Node *node = nullptr;

    QMap<Node*,BCU*>::ConstIterator it = _bcusMap.constBegin();

    foreach(BCU *b, _bcusMap.values()){
        if(b == bu){
            node = _bcusMap.key(bu);
        }
    }
    if(node != nullptr){
        AlarmManager *alm = new AlarmManager(bu->nofPacks(),bu->nofCellsPerPack(),bu->nofNtcsPerPack());
        //alm->setDuration();
        bu->setAlarmManager(alm);
        //_alarmManager.append(alm);
        node->sendStart();
        foreach (PDO *p, node->tpdos()) {
            p->setEnabled(true);
        }
        if(_logger != nullptr){
            _logger->addBCU(bu);
        }
        //connect(bu,&BCU::dataAccessed,this,&BMS_StackManager::bcuDataAccessed);
        // read control state
        //node->readObject(0x2003,0x01);
    }
    if(_currentBcuId == -1){
        nextBcu();
//        _currentBcuId = _bcusMap.values().indexOf(bu);
//        emit activeBcuChannged(bcu());
    }

}

//void BMS_StackManager::nodeNameChanged(QString name)
//{
//    qDebug()<<Q_FUNC_INFO;
//    Node *node = static_cast<Node*>(sender());
//    QMap<Node*, BCU*>::const_iterator it = _bcusMap.constFind(node);
//    if(it != _bcusMap.constEnd()){
//        it.value()->readConfig();
//    }
//}

void BMS_StackManager::bcuDataAccessed()
{
//    BCU *bcu = static_cast<BCU*>(sender());
//    if(bcu->isConfigReady()){
//        validateState(bcu);
//        _mbSlave->updateBcuData(bcu);
//    }
}

BCU *BMS_StackManager::bcu()
{
    //qDebug()<<Q_FUNC_INFO;
    if(_bcusMap.size() == 0) return nullptr;
    BCU *b = nullptr;
    if(_currentBcuId >= 0){
        b = _bcusMap.values().at(_currentBcuId);
    }
    return b;
}

BCU *BMS_StackManager::nextBcu()
{
    if(_bcusMap.size() == 0) return nullptr;
    _currentBcuId++;
    if(_currentBcuId >= _bcusMap.size()){
        _currentBcuId = 0;
    }
    BCU *b = bcu();
    emit activeBcuChannged(b);
    return b;
}

BCU *BMS_StackManager::prevBcu()
{
    if(_bcusMap.size() == 0) return nullptr;
    _currentBcuId--;
    if(_currentBcuId < 0){
        _currentBcuId = _bcusMap.size()-1;
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

BMS_Logger *BMS_StackManager::logger() const
{
    return _logger;
}
/*
 *  validate system alarm status
 */
void BMS_StackManager::validateState(BCU *b)
{
    Node *n = _bcusMap.key(b);
    if(n == nullptr) return;
    b->validate();
//    for(auto m = _bcusMap.cbegin(), end = _bcusMap.cend(); m != end;++m){
        //Node *n = m.key();
        //BCU *b = m.value();
        AlarmManager *a = b->alarmManager();

        for(int i=0;i<b->nofPacks();i++){
            a->setBalMask(i,(quint32)n->nodeOd()->value(0x2010+i,0x02).toInt());
            a->setOpenWire(i,(quint32)n->nodeOd()->value(0x2010+i,0x03).toInt());
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

        if(a->isEvent()){
            // todo: log event string
            _logger->logEvent(a->eventString());
        }

        // check balancing or openwire here
//    }



}

void BMS_StackManager::updateStackStatus()
{
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
    double _current = 0;
    _bcuLost = false;
    foreach(BCU *b,_bcusMap.values()){
        if(!b->isConfigReady()) continue;
        AlarmManager *m = b->alarmManager();
        _packVoltage = b->voltage();
        _current += b->current();
        if(m == nullptr) continue;
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

        // check if bcu lost
        int diff = QDateTime::currentDateTime().secsTo(b->lastSeen());
        if(diff > 10){
            _bcuLost = true;
        }
    }
    _totalCurrent = _current;
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

    QString statustip = "";
    statustip += QString("連線BCU數量:%1,均衡控制:%2").arg(totalBcus()).arg(_enableBalance?"作用":"停用");
    // storage info
#ifdef Q_OS_UNIX
    QStorageInfo sd_info = QStorageInfo("/run/media/mmcblk1p1");
    if(sd_info.isValid()){
        statustip += QString("SD:%1/%2 MB").arg(sd_info.bytesFree()>>20).arg(sd_info.bytesTotal()>>20);
    }
    else{
        statustip += "SD:Not Installed";
    }
#endif
    emit updateStatusText(statustip,0);
    emit uiControl(true);
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

bool BMS_StackManager::bcuLost() const
{
    return _bcuLost;
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

QAction *BMS_StackManager::actionScanBus() const
{
    return _actScanBus;
}

//void BMS_StackManager::validate()
//{

//}
