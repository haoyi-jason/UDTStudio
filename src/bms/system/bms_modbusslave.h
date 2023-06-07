#ifndef BMS_MODBUSSLAVE_H
#define BMS_MODBUSSLAVE_H

#include <QObject>
#include <QtSerialBus/QModbusRtuSerialSlave>
#include <QtSerialBus/QModbusTcpServer>
#include "bmsstack.h"

class BMS_ModbusSlave:public QObject
{
    Q_OBJECT
public:
    explicit BMS_ModbusSlave(QObject *parent = nullptr);
    bool startRTUSlave(QString portName, quint32 baudrate);
    void stopRTUSlave();
    bool startTCPServer(QString address, quint32 port);
    void stopTCPServer();
    void updateBcuData(BCU *bcu);

private:
    QModbusRtuSerialSlave *_rtuSlave;
    QModbusTcpServer *_tcpServer;
    QModbusDataUnitMap _registers;
};

#endif // BMS_MODBUSSLAVE_H
