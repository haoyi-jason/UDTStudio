#include "bmsstack.h"
#include <QDataStream>
#include <QDebug>
#include "canopenbus.h"

BCU::BCU(QObject *parent):
    BCU(nullptr,false,parent){

}

BCU::BCU(Node *node, bool autoStart, QObject *parent)
    :QObject(parent)
    ,_autoStart(autoStart)
{
    setNode(node);

    _maxPollId = _cellsPerPack + _ntcsPerPack + 1; // first element is pack voltage

    //_pollThread = new BCUPollThread(this);

    _voltage = 0;
    _current = 0;
    _soc = _soh = 0;

    _configReady = 0x0;
    _nofPacks = 0;
    _cellsPerPack = 0;
    _ntcsPerPack = 0;
    _startMode = 0;
    _currentGap = 2; // operation current between -_currentGap to _currentGap is idle
    _identifyReady = 0x0;
    _simulate = false;
    _alarmManager = nullptr;

    _pollTimer = new QTimer();
    connect(_pollTimer,&QTimer::timeout,this,&BCU::accessData);
    _lastSeen = QDateTime::currentDateTime();
    _pollTimes = 0;
    _configFail = false;
    _nameDefined = false;
    _configDone = false;
    _dataReady = false;
    _scannedDevice = 0;
}

BCU::~BCU()
{
    if(_node != nullptr){
        disconnect(_node,&Node::nameChanged,this,&BCU::nodeNameChanged);
    }
}

Node *BCU::node() const
{
    return _node;
}

void BCU::setNode(Node *node)
{
    if(node == nullptr) return;

    _node = node;
    connect(_node,&Node::nameChanged,this,&BCU::nodeNameChanged);

    if(_autoStart){
        _node->sendStart();
    }

    setNodeInterrest(_node);
    registerObjId(NodeObjectId(0x2001,0x01));
    registerObjId(NodeObjectId(0x2001,0x02));
    registerObjId(NodeObjectId(0x2001,0x03));
    registerObjId(NodeObjectId(0x2001,0x04));
    registerObjId(NodeObjectId(0x2003,0x01));
    registerObjId(NodeObjectId(0x2011,0x01));
    registerObjId(NodeObjectId(0x2012,0x01));
    registerObjId(NodeObjectId(0x2013,0x01));

}

void BCU::reset()
{
    _node->reset();
}

//void BCU::identify()
//{
//    _pollConfig = true;
//    startPoll(200);
//}

void BCU::setSimulate(bool state)
{
    _simulate = state;
    emit modeChanged(_simulate);
}

bool BCU::isSimulate() const
{
    return _simulate;
}

int BCU::pollRetry() const
{
    return _pollTimes;
}

QDateTime BCU::lastSeen() const
{
    return _lastSeen;
}

void BCU::notifyUpdate()
{
    if(_dataReady){
        //emit dataAccessed();
        _dataReady = false;
    }
}

int BCU::scannedDevices() const
{
    return _scannedDevice;
}

double BCU::voltage() const{

    return _voltage;
}

double BCU::current() const
{
    return _current;
}

double BCU::soc() const
{
    return _soc;
}

double BCU::soh() const
{
    return _soh;
}


QString BCU::statusStr()
{
    return QString("%1 V, %2 A").arg(voltage()).arg(current());
}

QString BCU::chargeStr()
{
    QString msg;
    if(alarmManager()->current() >= _currentGap){
        msg = "充電中";
    }
    else if(alarmManager()->current() < -_currentGap){
        msg = "放電中";
    }
    else{
        msg = "靜置中";
    }
    return msg;
}

//void BCU::accessVoltage(quint8 pack, quint8 cell)
//{
////    if((pack < _nofPacks) && (cell < _cellsPerPack)){
////        this->_sdoClients.at(0)->uploadData(0x2100+pack,cell+1,QMetaType::UInt);
////    }
//    if(_pollThread->isRunning()){
//        _pollThread->stop();
//    }
//    else{
//        _pollThread->start();
//    }
//}

//void BCU::startPollThread(int interval)
//{
//    if(_pollThread->isRunning()){
//        _pollThread->stop();
//        emit threadActive(false);
//    }
//    else{
//        _pollThread->start();
//        emit threadActive(true);
//    }
//}

quint8 BCU::nofPacks() const
{
    return _nofPacks;
}

quint8 BCU::nofNtcsPerPack() const
{
    return _ntcsPerPack;
}

quint8 BCU::nofCellsPerPack() const
{
    return _cellsPerPack;
}

quint8 BCU::bmuType() const
{
    return _bmuType;
}

quint8 BCU::cmdState() const
{
    return _cmdState;
}

BCU::BCU_CHARGE_STATE BCU::state() const
{
    return _state;
}

//double BCU::maxCell() const
//{
//    if(_alarmManager != nullptr){
//        return _alarmManager->maxCv();
//    }
//    return 0;
//}

//double BCU::minCell() const
//{
//    if(_alarmManager != nullptr){
//        return _alarmManager->minCv();
//    }
//    return 0;
//}

//qint16 BCU::cellDifference() const
//{
//    if(_alarmManager != nullptr){
//        return (qint16)((_alarmManager->maxCv() - _alarmManager->minCv()));
//    }
//    return 0;
//}

//quint8 BCU::maxCellPos() const
//{
//    if(_alarmManager != nullptr){
//        return _alarmManager->maxCvPos();
//    }
//    return 0;
//}

//quint8 BCU::minCellPos() const
//{
//    if(_alarmManager != nullptr){
//        return _alarmManager->minCvPos();
//    }
//    return 0;
//}

//double BCU::maxTemperature() const
//{
//    if(_alarmManager != nullptr){
//        return _alarmManager->maxCt();
//    }
//    return 0;
//}

//double BCU::minTemperature() const
//{
//    if(_alarmManager != nullptr){
//        return _alarmManager->minCt();
//    }
//    return 0;
//}

//quint8 BCU::maxTempPos() const
//{
//    if(_alarmManager != nullptr){
//        return _alarmManager->maxCtPos();
//    }
//    return 0;
//}

//quint8 BCU::minTempPos() const
//{
//    if(_alarmManager != nullptr){
//        return _alarmManager->minCtPos();
//    }
//    return 0;
//}

bool BCU::validate()
{
    bool valid = true;
    // check if emergency received

//    if(_errorClass != 0x00){
//        qDebug()<<Q_FUNC_INFO<<" Error";
//        valid = false;
//    }

    // loop through pack/cell/ntcs to validate
    NodeObjectId obj = NodeObjectId();

   // double scale_v = node()->nodeOd()->value(0x642F,0x03).toDouble();
   // double scale_a = node()->nodeOd()->value(0x642F,0x04).toDouble();

    _voltage = _node->nodeOd()->value(0x2002,0x03).toDouble()/10;
    _current = _node->nodeOd()->value(0x2002,0x04).toDouble()/10;
    _soc = node()->nodeOd()->value(0x2002,0x01).toDouble()/10;
    _soh = node()->nodeOd()->value(0x2002,0x02).toDouble()/10;
    _dataReady = false;
    emit dataAccessed();

    return valid;
}

//void BCU::configReceived(quint16 index, quint8 subindex)
//{
//    qDebug()<<Q_FUNC_INFO<<QString("%1").arg(_configReady,16);
//    if(index == 0x2001){
//        switch(subindex){
//        case 1:
//            _configReady |= (1 << (subindex -1));
//            _nofPacks = static_cast<quint8>(nodeOd()->value(0x2001,1).toInt()) - 1;
//            //readObject(0x2001,2);
//            _pollRetry = 0;
//            break;
//        case 2:
//            _configReady |= (1 << (subindex -1));
//            _cellsPerPack = static_cast<quint8>(nodeOd()->value(0x2001,2).toInt());
//            _pollRetry = 0;
//            //readObject(0x2001,3);
//            break;
//        case 3:
//            _configReady |= (1 << (subindex -1));
//            _ntcsPerPack = static_cast<quint8>(nodeOd()->value(0x2001,3).toInt());
//            _pollRetry = 0;
//            break;
//        default:break;
//        }

//        if(_configReady == 0x07){
//            // initial access list
////            foreach(NodeObjectId o , this->_accessIds){
////                this->_accessIds.removeOne(o);
////            }
////            for(int i=0;i<_nofPacks;i++){
////                for(int j=0;j<0x09;j++){
////                    _accessIds.append(NodeObjectId(0x2100+i,j+0x01));
////                }
////                for(int j=0;j<_cellsPerPack;j++){
////                    _accessIds.append(NodeObjectId(0x2100+i,j+0x0A));
////                }
////                for(int j=0;j<_ntcsPerPack;j++){
////                    _accessIds.append(NodeObjectId(0x2100+i,j+0x18));
////                }
////            }
////            _accessIds.append(NodeObjectId(0x1001,0x0));
////            for(int i=1;i<5;i++){
////                _accessIds.append(NodeObjectId(0x2002,i));
////            }

//            emit configReady();
//        }
//    }
//    else if(index == 0x1018){
//        _identifyReady |= (1 <<(subindex -1));
//        if(_identifyReady == 0x0F){
//            emit identified();
//        }
//    }
//}

void BCU::nodeNameChanged(QString name)
{
    qDebug()<<Q_FUNC_INFO;
    (void)name;

    _nameDefined = true;
    _firstPollTime = QDateTime::currentDateTime();
//    readConfig();
}

void BCU::readConfig()
{
    qDebug()<<Q_FUNC_INFO;
    _pollConfig = true;
    _configReady = 0x00;

}

void BCU::resetError()
{
    //_errorClass = 0x0;
    //_errorCode = 0x0;
}

void BCU::startPoll(int interval)
{
//    qDebug()<<Q_FUNC_INFO;
//    if(!canPoll()) return;

    if(_pollConfig){
        _pollTimer->start(interval);
    }
    else if(canPoll()){
        if(_pollTimer->isActive()){
            stopPoll();
        }
        else { // start poll node if not in simulate mode
            _accessIdIterator = _accessIds.begin();
            ++_accessIdIterator;
            if(interval == 0){
                _accessOnece = true;
            }
            else{
                _accessOnece = false;
            }
            _pollTimer->start(interval);
            node()->tpdos().at(0)->setEnabled(true);
        }
    }
}

void BCU::stopPoll()
{
//    qDebug()<<Q_FUNC_INFO;
    node()->tpdos().at(0)->setEnabled(false);
    _pollConfig = false;
    _pollTimer->stop();
}

void BCU::accessConfig()
{
    if(_nameDefined){
        quint16 mask = (~_configReady)&0x1F;
        if(mask & 0x01){
            _node->readObject(0x2001,0x01);
        }
        else if(mask & 0x02){
            _node->readObject(0x2001,0x02);
        }
        else if(mask & 0x04){
            _node->readObject(0x2001,0x03);
        }
        else if(mask & 0x08){
            _node->readObject(0x2001,0x04); //BMU type
        }
        else if(mask & 0x10){
            _node->readObject(0x2003,0x01); // data dump?
        }

        if(_firstPollTime.secsTo(QDateTime::currentDateTime()) > 20){
            _configFail = true;
            emit configFail();

        }
//        _pollRetry++;
//        if(_pollRetry > 100){
//            _configFail = true;
//            emit configFail();
//        }
    }
}

void BCU::accessData()
{
    if(_pollConfig){
        if(_configReady == 0x00){
            _node->readObject(0x2001,0x01);
            _pollRetry++;
        }
        else if(_configReady == 0x01){
            _node->readObject(0x2001,0x02);
            _pollRetry++;
        }
        else if(_configReady == 0x03){
            _node->readObject(0x2001,0x03);
            _pollRetry++;
        }
        else if(_configReady == 0x07){
            _node->readObject(0x1018,0x04);
            _pollRetry++;
            stopPoll();
        }

        if(_pollRetry > 10){
            stopPoll();
            emit configFail();
        }

    }
//    else if(!_simulate){
//        if(_node->nodeOd()->errorObject(*_accessIdIterator) == 0){
//            readObject(*_accessIdIterator);
//        }
//        ++_accessIdIterator;
//        if(_accessIdIterator == _accessIds.end()){
//            if(_accessOnece){
//                _pollTimer->stop();
//            }
//            _accessIdIterator = _accessIds.begin();
//            //validate();
//            emit dataAccessed();
//        }
//    }
}

quint8 BCU::startMode() const
{
    return _startMode;
}

void BCU::setStartMode(quint8 mode)
{
    _startMode = mode;
}

bool BCU::isConfigReady()
{
    return _configDone;
}

bool BCU::isConfigFail() const
{
    return _configFail;
}
void BCU::reConfig()
{
    _configReady = 0x0;
    //readConfig();
}

bool BCU::canPoll()
{
    return (isConfigReady());
}

bool BCU::dataReady() const
{
    return _dataReady;
}

//bool BCU::isPolling()
//{
//    return (_pollTimer->isActive());
//}

void BCU::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    //qDebug()<<Q_FUNC_INFO<<node()->nodeId();
    if((flags & NodeOd::FlagsRequest::Error) != 0){
        // todo: handle error here
        //_odError = true;
        return;
    }
    _lastSeen = QDateTime::currentDateTime();
    quint16 index = objId.index();
    quint8 subindex = objId.subIndex();
    //qDebug()<<Q_FUNC_INFO<<QString("%1").arg(_configReady,16);
    if(index == 0x2001 && _nameDefined){
        switch(subindex){
        case 1:
            _configReady |= (1 << (subindex -1));
            _pollRetry = 0;
            break;
        case 2:
            _configReady |= (1 << (subindex -1));
            _pollRetry = 0;
            //readObject(0x2001,3);
            break;
        case 3:
            _configReady |= (1 << (subindex -1));
            _pollRetry = 0;
            break;
        case 4:
            _configReady |= (1 << (subindex -1));
            _pollRetry = 0;
            break;
        default:break;
        }

    }
    else if(index == 0x2003 && _nameDefined){
        switch(subindex){
        case 1:
            _configReady |= 0x10;
            _pollRetry = 0;
            _cmdState = static_cast<quint8>(_node->nodeOd()->value(objId).toInt());
            break;
        default:break;
        }

    }
    else if(index == 0x2011 && isConfigReady()){
        // save data to od
        quint8 pack = static_cast<quint8>(_node->nodeOd()->value(index,0x01).toInt());
        quint8 cell = static_cast<quint8>(_node->nodeOd()->value(index,0x02).toInt());
        if((cell == 0xff)){
            //emit dataAccessed();
            _dataReady = true;
        }
        else if(pack < _nofPacks && !_simulate){
            for(int i=0;i<3;i++,cell++){
                if(cell < _cellsPerPack){
                    quint16 value = static_cast<quint16>(_node->nodeOd()->value(index,0x03 + i).toInt());
                    node()->nodeOd()->subIndex(0x2100+pack,cell + 0x0a )->setValue(value);
                }
            }
        }
    }
    else if(index == 0x2012 && isConfigReady()){
        quint8 pack = static_cast<quint8>(_node->nodeOd()->value(index,0x01).toInt());
        quint8 ntc = static_cast<quint8>(_node->nodeOd()->value(index,0x02).toInt());
        if(pack < _nofPacks && !_simulate ){
            for(int i=0;i<3;i++,ntc++){
                if(ntc < _ntcsPerPack){
                    quint16 value = static_cast<quint16>(_node->nodeOd()->value(index,0x03 + i).toInt());
                    node()->nodeOd()->subIndex(0x2100+pack,ntc + 0x1a )->setValue(value);
                }
            }
        }

    }
    else if(index == 0x2013 && isConfigReady()){
        quint8 pack = static_cast<quint8>(_node->nodeOd()->value(index,0x01).toInt());
        quint8 id = static_cast<quint8>(_node->nodeOd()->value(index,0x02).toInt());
        quint16 v1 = static_cast<quint16>(_node->nodeOd()->value(index,0x03).toInt());
        quint16 v2 = static_cast<quint16>(_node->nodeOd()->value(index,0x04).toInt());
        quint16 v3 = static_cast<quint16>(_node->nodeOd()->value(index,0x05).toInt());
        if(pack == 0xf0){ // 0xf0 == state report
            switch(id){
            case 0: // scanned device
                _scannedDevice = v1;
                break;
            case 1:
                _errorCode = v1;
                break;
            }
            emit statusReported();
        }
        else if(pack < _nofPacks && !_simulate){
            if(id < 0x04){
                node()->nodeOd()->subIndex(0x2100+pack,id)->setValue(v1);
            }
        }
    }

    if(!_configDone && (_configReady == 0x1F)){
        _nofPacks = static_cast<quint8>(_node->nodeOd()->value(0x2001,1).toInt());
        _cellsPerPack = static_cast<quint8>(_node->nodeOd()->value(0x2001,2).toInt());
        _ntcsPerPack = static_cast<quint8>(_node->nodeOd()->value(0x2001,3).toInt());
        _bmuType = static_cast<quint8>(_node->nodeOd()->value(0x2001,4).toInt());
        if(_bmuType == 2){
            _nofPacks--;
        }
        _cmdState = static_cast<quint8>(_node->nodeOd()->value(0x2003,1).toInt());
        //qDebug()<<Q_FUNC_INFO<<"config ready" <<node()->nodeId() << _nofPacks << _cellsPerPack << _ntcsPerPack;
        _configDone = true;
        emit configReady();
    }

}
/****
 *
 *
 *
 *
 */

void BCU::setAlarmManager(AlarmManager *alarm)
{
    _alarmManager = alarm;
}

AlarmManager *BCU::alarmManager() const
{
    return _alarmManager;
}


