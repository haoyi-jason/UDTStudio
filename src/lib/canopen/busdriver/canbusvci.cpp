#include "canbusvci.h"
#include "../../vci_can/VCI_CAN.h"

#include <QDebug>

CanBusVCI::CanBusVCI(const QString &address)
    :CanBusDriver(address)
{

    _can_port = -1;
    _readNotifier = nullptr;

    QString portStr = address.mid(3,address.size()-3);
    int portNum = portStr.toInt();
    if(portNum != 0){
        _can_port = portNum;
    }

    _vciLibrary.setFileName("VCI_CAN.dll");
    _vciLibrary.load();

    QLibrary libTest("VCI_CAN.dll");
    libTest.load();
    if(libTest.isLoaded()){
        libTest.unload();
    }

    if(!_vciLibrary.isLoaded()){
        qDebug()<<"Load VCI Dll failed";
        _vciLibrary.unload();
    }
}

CanBusVCI::~CanBusVCI()
{
    disconnectDevice();
    if(_vciLibrary.isLoaded()){
        _vciLibrary.unload();
    }
}

bool CanBusVCI::connectDevice()
{
    _VCI_CAN_PARAM pCANPARAM;
    uint8_t Mod_CfgData[512];

    Mod_CfgData[0] = 1;
    Mod_CfgData[1] = 1;

    pCANPARAM.DevPort = _can_port;
    pCANPARAM.DevType = 2;
    pCANPARAM.CAN1_Baud = 500000;
    pCANPARAM.CAN2_Baud = 500000;

    typedef int32_t (*vci_open)(_VCI_CAN_PARAM *);
    vci_open VCI_OPEN = (vci_open)_vciLibrary.resolve("VCI_OpenCAN");

    int ret = VCI_OPEN(&pCANPARAM);

    if(ret != No_Err){
        return false;
    }

    QMutexLocker locker(&_mutex);

    _readNotifier = new CanBusVCINotifierThread(this);
    _readNotifier->start();

    setState(CONNECTED);

    return true;
}

void CanBusVCI::disconnectDevice()
{
    QMutexLocker locker(&_mutex);
    if(_can_port == 0){
        return;
    }
    _readNotifier->terminate();

    typedef int32_t (*vci_close)(int);
    vci_close VCI_Close = (vci_close)_vciLibrary.resolve("VCI_CloseCAN");
    VCI_Close(_can_port);

    setState(DISCONNECTED);
    _readNotifier->deleteLater();

}

QCanBusFrame CanBusVCI::readFrame()
{
    QCanBusFrame qtFrame;

    typedef int32_t (*VCI_RecvCANMsgEx)(BYTE,_VCI_CAN_MSG*);

    DWORD nofPacket = 0;
    int32_t ret;
    VCI_RecvCANMsgEx vci_recv_msg = (VCI_RecvCANMsgEx)_vciLibrary.resolve("VCI_RecvCANMsg");

    _VCI_CAN_MSG canMsg;

    nofPacket = message_count(1);
    qtFrame.setFrameType(QCanBusFrame::InvalidFrame);
    if(nofPacket > 0){
        ret = vci_recv_msg(1,&canMsg);
        if(ret == No_Err){
            qtFrame.setFrameId(canMsg.ID);
            qtFrame.setFrameType(canMsg.RTR == RTR_0? QCanBusFrame::DataFrame:QCanBusFrame::RemoteRequestFrame );

            QByteArray payload;
            for(int i=0;i<canMsg.DLC;i++){
                payload.append(canMsg.Data[i]);
            }
            qtFrame.setPayload(payload);
        }
    }

    return qtFrame;
}

bool CanBusVCI::writeFrame(const QCanBusFrame &qtframe)
{
    QMutexLocker locker(&_mutex);
    _VCI_CAN_MSG canMsg;
    typedef int32_t (*VCI_SendCANMsgEx)(BYTE,_VCI_CAN_MSG*);

    DWORD nofPacket = 0;
    int32_t ret;
    VCI_SendCANMsgEx vci_sendmsg = (VCI_SendCANMsgEx)(_vciLibrary.resolve("VCI_SendCANMsg"));

    canMsg.ID = qtframe.frameId();
    if(qtframe.frameType() == QCanBusFrame::RemoteRequestFrame){
        canMsg.RTR = RTR_1;
    }
    else{
        canMsg.RTR = RTR_0;
    }
    canMsg.DLC = qtframe.payload().size();
    for(int i=0;i<qtframe.payload().size();i++){
        canMsg.Data[i] = qtframe.payload()[i];
    }

    ret = vci_sendmsg(1,&canMsg);


    return (ret == No_Err);
}

void CanBusVCI::notifyRead()
{
    emit framesReceived();
}

void CanBusVCI::handleError()
{
    disconnectDevice();
}

int CanBusVCI::message_count(int id)
{
    QMutexLocker locker(&_mutex);
    typedef int32_t (*VCI_Get_RxMsgCntEx)(BYTE,DWORD*);

    DWORD nofPacket = 0;
    int32_t ret;
    VCI_Get_RxMsgCntEx vci_get_rxcnt = (VCI_Get_RxMsgCntEx)(_vciLibrary.resolve("VCI_Get_RxMsgCnt"));
    vci_get_rxcnt(id,&nofPacket);

    return (int)nofPacket;
}

void CanBusVCI::setAddress(const QString &address)
{
    // disconnect bus

    disconnectDevice();

    _adress = address;
    QString portStr = address.mid(3,_adress.size()-3);
    int portNum = portStr.toInt();
    if(portNum != 0){
        _can_port = portNum;
    }

    connectDevice();
}



CanBusVCINotifierThread::CanBusVCINotifierThread(CanBusVCI *driver)
    :QThread(driver)
{
    _driver = driver;
}

void CanBusVCINotifierThread::run()
{

    bool running = true;
    int nofPacket = 0;
    while(running){
        nofPacket = _driver->message_count(1);
        if(nofPacket > 0){
            _driver->notifyRead();
        }
        nofPacket = _driver->message_count(2);
        if(nofPacket > 0){
            _driver->notifyRead();
        }
        QThread::msleep(100);
    }

}

