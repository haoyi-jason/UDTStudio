#include "bmsstack.h"

#include <QDebug>

BCU::BCU(quint8 nodeId, const QString &name, const QString &edsFileName)
    :Node(nodeId,name,edsFileName)
{
    _nofPacks = static_cast<quint8>(nodeOd()->value(0x2001,1).toInt());
    _cellsPerPack = static_cast<quint8>(nodeOd()->value(0x2001,2).toInt());
    _ntcsPerPack = static_cast<quint8>(nodeOd()->value(0x2001,3).toInt());

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
}

BCU::~BCU()
{
}

double BCU::voltage() const{

    return _voltage;
}

double BCU::current() const
{
    return _current;
}

QList<BatteryPack*> BCU::packs() const
{
    return _packs;
}

QString BCU::statusStr()
{
    return QString("%1 V, %2 A").arg(voltage()).arg(current());
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
    return _maxCellVoltage;
}

double BCU::minCell() const
{
    return _minCellVoltage;
}

qint16 BCU::cellDifference() const
{
    return _maxCellDifferenceMv;
}

quint8 BCU::maxCellPos() const
{
    return _maxCVid;
}

quint8 BCU::minCellPos() const
{
    return _minCVid;
}

double BCU::maxTemperature() const
{
    return _maxTemperature;
}

double BCU::minTemperature() const
{
    return _minTemperature;
}

quint8 BCU::maxTempPos() const
{
    return _maxTid;
}

quint8 BCU::minTempPos() const
{
    return _minTid;
}

void BCU::validate()
{
    // loop through pack/cell/ntcs to validate
    NodeObjectId obj = NodeObjectId();


    if(_alarmManager == nullptr) return;


    for(int i=0;i<_nofPacks;i++){
        for(int j=0;j<_cellsPerPack;j++){
            _alarmManager->set_cell_voltage(i,_nodeOd->value(0x2100 + i,j + 0x0a).toDouble());
        }
    }

    for(int i=0;i<_nofPacks;i++){
        for(int j=0;j<_ntcsPerPack;j++){
            _alarmManager->set_cell_temperature(i,_nodeOd->value(0x2100 + i,j + 0x18).toDouble());
        }
    }

    _alarmManager->set_soc(_nodeOd->value(0x2102,0x01).toDouble());

    // todo : add alarm output function to fire output

}

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
                QThread::msleep(2000);
                if(_stop){
                    break;
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
