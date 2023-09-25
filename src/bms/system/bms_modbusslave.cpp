#include "bms_modbusslave.h"
#include <QSerialPort>

BMS_ModbusSlave::BMS_ModbusSlave(QObject *parent)
{
   _registers.insert(QModbusDataUnit::HoldingRegisters,{QModbusDataUnit::HoldingRegisters,0,8000});
    _tcpServer = nullptr;
    _rtuSlave = nullptr;
}

bool BMS_ModbusSlave::startRTUSlave(QString portName, quint32 baudrate)
{
    if(_rtuSlave == nullptr){
        _rtuSlave = new QModbusRtuSerialSlave();
    }
    if(_rtuSlave == nullptr) return false;
    _rtuSlave->setConnectionParameter(QModbusDevice::SerialPortNameParameter,portName);
    _rtuSlave->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,baudrate);
    _rtuSlave->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::EvenParity);
    _rtuSlave->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,8);
    _rtuSlave->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
    return _rtuSlave->connectDevice();
}

void BMS_ModbusSlave::stopRTUSlave()
{
    if(_rtuSlave != nullptr){
        _rtuSlave->disconnectDevice();
        _rtuSlave->deleteLater();
        _rtuSlave = nullptr;
    }
}

bool BMS_ModbusSlave::startTCPServer(QString address, quint32 port)
{
    if(_tcpServer == nullptr){
        _tcpServer = new QModbusTcpServer();
        if(_tcpServer == nullptr) return false;
    }
    _tcpServer->setConnectionParameter(QModbusDevice::NetworkAddressParameter,address);
    _tcpServer->setConnectionParameter(QModbusDevice::NetworkPortParameter,port);
    _tcpServer->setMap(_registers);
    return _tcpServer->connectDevice();

}

void BMS_ModbusSlave::stopTCPServer()
{
    if(_tcpServer != nullptr){
        _tcpServer->disconnectDevice();
        _tcpServer->deleteLater();
        _tcpServer = nullptr;
    }

}

void BMS_ModbusSlave::updateBcuData(BCU *bcu)
{

}