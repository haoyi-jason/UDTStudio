#include "bmsstack.h"
#include <QDataStream>
#include <QDebug>
#include "canopenbus.h"

BCU::BCU(quint8 nodeId, const QString &name, const QString &edsFileName)
    :Node(nodeId,name,edsFileName)
{

    //qDebug()<<QString("Packs:%1, Cells:%2, Ntcs:%3").arg(_nofPacks).arg(_cellsPerPack).arg(_ntcsPerPack);

    //qDebug()<<QString("Node OD count: %1").arg(_nodeOd->indexes().count());

//    for(quint8 i=0;i<_nofPacks;i++){
//        quint16 index_v = 0x2100 + i;

//        NodeIndex *index = new NodeIndex(index_v);
//        index->setObjectType(NodeIndex::ObjectType::ARRAY);
//        for(quint8 i=0;i<(_cellsPerPack + _ntcsPerPack);i++){
//            index->addSubIndex(new NodeSubIndex(i));
//        }
//        _nodeOd->addIndex(index);
//    }

    _maxPollId = _cellsPerPack + _ntcsPerPack + 1; // first element is pack voltage

    _pollThread = new BCUPollThread(this);

    _voltage = 0;
    _current = 0;

    _alarmManager = new AlarmManager(_nofPacks,_cellsPerPack,_ntcsPerPack);

    _configReady = 0x0;
    _nofPacks = 0;
    _cellsPerPack = 0;
    _ntcsPerPack = 0;
    _startMode = 0;
    _currentGap = 2; // operation current between -_currentGap to _currentGap is idle
    _identifyReady = 0x0;

    _pollTimer = new QTimer();
    connect(_pollTimer,&QTimer::timeout,this,&BCU::accessData);


}

BCU::~BCU()
{
}

void BCU::reset()
{
    qDebug()<<Q_FUNC_INFO;
    Node::reset();
    identify();

}

void BCU::identify()
{
//    readObject(0x1018,0x01);
//    readObject(0x1018,0x02);
//    readObject(0x1018,0x03);
//    readObject(0x1018,0x04);
    QCanBusFrame frameEmcy;
    frameEmcy.setFrameId(_emergency->cobIds().at(0));
    QByteArray payload(8,'0');
    frameEmcy.setPayload(payload);
    qDebug()<<Q_FUNC_INFO<<frameEmcy.payload().count();
    qDebug()<<bus()->writeFrame(frameEmcy);

}

double BCU::voltage() const{

    return _voltage;
}

double BCU::current() const
{
    return _current;
}


QString BCU::statusStr()
{
    return QString("%1 V, %2 A").arg(voltage()).arg(current());
}

QString BCU::chargeStr()
{
    QString msg;
    if(_errorCode != 0x0000){
        qDebug()<<Q_FUNC_INFO << " Error!";
        quint8 v1;
        quint32 v2;
        QDataStream ds(_errorDest);
        ds >> v1;
        ds >> v2;

        msg = QString("系統錯誤![0x%1]").arg(v2,8,16);

    }

    else if(current() >= _currentGap){
        msg = "充電中";
    }
    else if(current() < -_currentGap){
        msg = "放電中";
    }
    else{
        msg = "靜置中";
    }
    return msg;
}

QString BCU::cvStr()
{
    return QString("電芯電壓: 最高[%1][%2] %3 mv 最低[%4][%5] %6 mv 壓差:%7 mv")
            .arg(maxCellPos()/nofCellsPerPack()+1)
            .arg(maxCellPos()%nofCellsPerPack()+1)
            .arg(maxCell())
            .arg(minCellPos()/nofCellsPerPack()+1)
            .arg(minCellPos()%nofCellsPerPack()+1)
            .arg(minCell())
            .arg(cellDifference());

}

QString BCU::ctStr()
{
    return QString("電芯溫度: 最高[%1][%2] %3 %7 最低[%4][%5] %6 %7")
            .arg(maxTempPos()/nofNtcsPerPack()+1)
            .arg(maxTempPos()%nofNtcsPerPack()+1)
            .arg(maxTemperature())
            .arg(minTempPos()/nofNtcsPerPack()+1)
            .arg(minTempPos()%nofNtcsPerPack()+1)
            .arg(minTemperature())
            .arg(QChar(0x2103));
}

Node::Status BCU::status()
{
    return _status;
}
void BCU::accessVoltage(quint8 pack, quint8 cell)
{
//    if((pack < _nofPacks) && (cell < _cellsPerPack)){
//        this->_sdoClients.at(0)->uploadData(0x2100+pack,cell+1,QMetaType::UInt);
//    }
    if(_pollThread->isRunning()){
        _pollThread->stop();
    }
    else{
        _pollThread->start();
    }
}

void BCU::startPollThread(int interval)
{
    if(_pollThread->isRunning()){
        _pollThread->stop();
        emit threadActive(false);
    }
    else{
        _pollThread->start();
        emit threadActive(true);
    }
}

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

BCU::BCU_CHARGE_STATE BCU::state() const
{
    return _state;
}

double BCU::maxCell() const
{
    return _alarmManager->maxCv();
}

double BCU::minCell() const
{
    return _alarmManager->minCv();
}

qint16 BCU::cellDifference() const
{
    return (qint16)((_alarmManager->maxCv() - _alarmManager->minCv()));
}

quint8 BCU::maxCellPos() const
{
    return _alarmManager->maxCvPos();
}

quint8 BCU::minCellPos() const
{
    return _alarmManager->minCvPos();
}

double BCU::maxTemperature() const
{
    return _alarmManager->maxCt();
}

double BCU::minTemperature() const
{
    return _alarmManager->minCt();
}

quint8 BCU::maxTempPos() const
{
    return _alarmManager->maxCtPos();
}

quint8 BCU::minTempPos() const
{
    return _alarmManager->minCtPos();
}

bool BCU::validate()
{
    bool valid = true;
    // check if emergency received

    if(_errorClass != 0x00){
        qDebug()<<Q_FUNC_INFO<<" Error";
        valid = false;
    }

    // loop through pack/cell/ntcs to validate
    NodeObjectId obj = NodeObjectId();

    _voltage = nodeOd()->value(0x2002,0x03).toDouble()/10.;
    _current = nodeOd()->value(0x2002,0x04).toDouble()/10.;


    if(_alarmManager == nullptr){
        valid = false;
    }

    else{
        for(int i=0;i<_nofPacks;i++){
            for(int j=0;j<_cellsPerPack;j++){
                _alarmManager->set_cell_voltage(i * _cellsPerPack + j,_nodeOd->value(0x2100 + i,j + 0x0a).toDouble());
            }
        }

        for(int i=0;i<_nofPacks;i++){
            for(int j=0;j<_ntcsPerPack;j++){
                _alarmManager->set_cell_temperature(i*_ntcsPerPack + j,_nodeOd->value(0x2100 + i,j + 0x18).toDouble()/10.);
            }
        }

        _alarmManager->set_soc(_nodeOd->value(0x2102,0x01).toDouble());

        // todo : add alarm output function to fire output
        quint32 org = nodeOd()->value(0x2004,0x02).toInt();
        quint32 out = 0x0;
        if(_alarmManager->isAlarm()){
            out = 0x02;
        }
        else if(_alarmManager->isWarning()){
            out = 0x01;
        }

        if(out != org){
            QVariant valueToWrite;
            valueToWrite.setValue(out);
            qDebug()<< Q_FUNC_INFO << " :Issue digital output";
            writeObject(0x2004,0x02, valueToWrite);
        }

        emit updateState();
    }

    if(alarmManager()->isEvent()){
        qDebug()<<Q_FUNC_INFO<<alarmManager()->eventString();
        emit sendEvent(alarmManager()->eventString());
    }

    return valid;
}

void BCU::configReceived(quint16 index, quint8 subindex)
{
    qDebug()<<Q_FUNC_INFO;
    if(index == 0x2001){
        _configReady |= (1 << (subindex -1));
        switch(subindex){
        case 1:
            _nofPacks = static_cast<quint8>(nodeOd()->value(0x2001,1).toInt()) - 1;
            break;
        case 2:
            _cellsPerPack = static_cast<quint8>(nodeOd()->value(0x2001,2).toInt());
            break;
        case 3:
            _ntcsPerPack = static_cast<quint8>(nodeOd()->value(0x2001,3).toInt());
            break;
        default:break;
        }

        if(_configReady == 0x07){
            // initial access list
            for(int i=0;i<_nofPacks;i++){
                for(int j=0;j<0x09;j++){
                    _accessIds.append(NodeObjectId(0x2100+i,j+0x01));
                }
                for(int j=0;j<_cellsPerPack;j++){
                    _accessIds.append(NodeObjectId(0x2100+i,j+0x0A));
                }
                for(int j=0;j<_ntcsPerPack;j++){
                    _accessIds.append(NodeObjectId(0x2100+i,j+0x18));
                }
            }
            _accessIds.append(NodeObjectId(0x1001,0x0));
            for(int i=1;i<5;i++){
                _accessIds.append(NodeObjectId(0x2002,i));
            }

            emit configReady();
        }
    }
    else if(index == 0x1018){
        _identifyReady |= (1 <<(subindex -1));
        if(_identifyReady == 0x0F){
            emit identified();
        }
    }
}

void BCU::readConfig()
{
    qDebug()<<Q_FUNC_INFO << _configReady;
    if((_configReady & 0x01) == 0x00){
        readObject(0x2001,1);
    }
    if((_configReady & 0x02) == 0x00){
        readObject(0x2001,2);
    }
    if((_configReady & 0x04) == 0x00){
        readObject(0x2001,3);
    }
}

void BCU::resetError()
{
    _errorClass = 0x0;
    _errorCode = 0x0;
}

void BCU::startPoll(int interval)
{
    qDebug()<<Q_FUNC_INFO;
    if(!canPoll()) return;

    _accessIdIterator = _accessIds.begin();
    ++_accessIdIterator;
    if(interval == 0){
        _accessOnece = true;
    }
    else{
        _accessOnece = false;
    }
    _pollTimer->start(150);
}

void BCU::stopPoll()
{
    qDebug()<<Q_FUNC_INFO;
    _pollTimer->stop();
}

void BCU::accessData()
{
    if(nodeOd()->errorObject(*_accessIdIterator) == 0){
        readObject(*_accessIdIterator);
    }
    ++_accessIdIterator;
    if(_accessIdIterator == _accessIds.end()){
        if(_accessOnece){
            _pollTimer->stop();
        }
        _accessIdIterator = _accessIds.begin();
    }
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
    return (_configReady == 0x07);
}

void BCU::reConfig()
{
    _configReady = 0x0;
    readConfig();
}

bool BCU::canPoll()
{
    return ((status() == Node::Status::STARTED) && isConfigReady());
}

/****
 *
 *
 *
 *
 */

AlarmManager *BCU::alarmManager() const
{
    return _alarmManager;
}

/*  batterypack */

BatteryPack::BatteryPack(QObject *parent):QObject(parent)
{

}

QString BatteryPack::name() const
{
    return _name;
}

void BatteryPack::updateState()
{
    uint32_t sum_mv = 0;
    foreach (uint16_t v, _cellVoltage) {
        sum_mv += v;
    }
    _voltage = sum_mv/1000.;

}

double BatteryPack::voltage() const{
    //updateState();
    return _voltage;
}


QString BatteryPack::status()
{
    return QString("%1 V").arg(voltage());
}

QVariant BatteryPack::cellVoltage(int cell) const
{
    return _cellVoltage.at(cell);
}

QVariant BatteryPack::temperature(int ntc) const
{
    return _ntcTemperature.at(ntc);
}


/* Poll thread */

BCUPollThread::BCUPollThread(BCU *bcu)
    :QThread(bcu)
{
    _bcu = bcu;
    _stop = false;
    _interval = 1000;
}

void BCUPollThread::run()
{
    bool running = true;
    quint8 packs = _bcu->nofPacks();
    quint8 ids = _bcu->nofCellsPerPack() + _bcu->nofNtcsPerPack() + 1;
    _stop = false;
    while(!_stop){
        for(quint8 i=0;i< packs;i++){
            for(quint8 j=1;j< ids; j++){
                //_bcu->_sdoClients.at(0)->uploadData(0x2100+i,j,QMetaType::UInt);
                _bcu->readObject(0x2100+i,j);
                //QThread::msleep(100);
                if(_stop){
                    j = ids;
                    i = packs;
                }
            }
        }
    }

}

void BCUPollThread::stop()
{
    _stop = true;
}

void BCUPollThread::setInterval(int value)
{
    _interval = value;
}
