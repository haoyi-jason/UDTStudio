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
    _rtuSlave->setConnectionParameter(QModbusDevice::SerialPortNameParameter,portName);
    _rtuSlave->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,baudrate);
    _rtuSlave->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::EvenParity);
    _rtuSlave->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,8);
    _rtuSlave->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);
    _rtuSlave->connectDevice();
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
    _tcpServer = new QModbusTcpServer();
    _tcpServer->setConnectionParameter(QModbusDevice::NetworkAddressParameter,"localhost");
    _tcpServer->setConnectionParameter(QModbusDevice::NetworkPortParameter,502);
    _tcpServer->setMap(_registers);
    _tcpServer->connectDevice();

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
