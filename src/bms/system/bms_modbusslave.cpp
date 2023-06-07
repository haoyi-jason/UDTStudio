#include "bms_modbusslave.h"

BMS_ModbusSlave::BMS_ModbusSlave(QObject *parent)
{
   _registers.insert(QModbusDataUnit::HoldingRegisters,{QModbusDataUnit::HoldingRegisters,0,8000});
    _tcpServer = nullptr;
    _rtuSlave = nullptr;
}

bool BMS_ModbusSlave::startRTUSlave(QString portName, quint32 baudrate)
{
    _tcpServer = new QModbusTcpServer();
    _tcpServer->setConnectionParameter(QModbusDevice::NetworkAddressParameter,"localhost");
    _tcpServer->setConnectionParameter(QModbusDevice::NetworkPortParameter,502);
    _tcpServer->setMap(_registers);
    _tcpServer->connectDevice();
}

void BMS_ModbusSlave::stopRTUSlave()
{
    if(_tcpServer != nullptr){
        _tcpServer->disconnectDevice();
        _tcpServer->deleteLater();
        _tcpServer = nullptr;
    }
}

bool BMS_ModbusSlave::startTCPServer(QString address, quint32 port)
{

}

void BMS_ModbusSlave::stopTCPServer()
{

}

void BMS_ModbusSlave::updateBcuData(BCU *bcu)
{

}
