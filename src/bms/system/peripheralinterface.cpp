#include "peripheralinterface.h"
#include <QFile>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <QProcess>

PeripheralInterface::PeripheralInterface()
{

}



/*************** IIODevice *****************/

IIODevice::IIODevice(QObject *parent, QString devName, QByteArray *pBuffer, QMutex *pMutex)
    :QThread(parent)
    ,fileName(devName)
    ,buffer(pBuffer)
    ,_mutex(pMutex)
{
    _abort = false;
    _started =false;
    _iop = new QProcess();
}

int IIODevice::openDevice()
{
    QString cmdStr = "echo 1 > /sys/bus/iio/devices/"+fileName+"/scan_elements/in_voltage0_en";
    QByteArray cmdBuffer = cmdStr.toLocal8Bit();
    char *cmd = cmdBuffer.data();

    // enable iio channel, only channel-0 is enable due to scan_element error if trying to enable more than 1 channel
    for(int i=0;i<1;i++){
        cmdStr = QString("echo 1 >/sys/bus/iio/devices/%1/scan_elements/in_voltage%2_en").arg(fileName).arg(i);
        cmdBuffer = cmdStr.toLocal8Bit();
        cmd = cmdBuffer.data();
        system(cmd);
    }
    // setup trigger
    cmdStr = "echo 2 > /sys/bus/iio/devices/iio_sysfs_trigger/add_trigger";
    cmdBuffer = cmdStr.toLocal8Bit();
    cmd = cmdBuffer.data();
    system(cmd);

    cmdStr = QString("echo sysfstrig2 >/sys/bus/iio/devices/%1/trigger/current_trigger").arg(fileName);
    cmdBuffer = cmdStr.toLocal8Bit();
    cmd = cmdBuffer.data();
    system(cmd);

    // set buffer length
    cmdStr = "echo 8 > /sys/bus/iio/devices/"+fileName+"/buffer/length";
    cmdBuffer = cmdStr.toLocal8Bit();
    cmd = cmdBuffer.data();
    system(cmd);

    // enable buffer
    cmdStr = "echo 1 > /sys/bus/iio/devices/"+fileName+"/buffer/enable";
    system(cmdStr.toLocal8Bit().data());

    QString path = "/dev/"+fileName;
    QFile *file = new QFile();
    file->setFileName(path);
    if(!file->exists()){
        return -1;
    }

    fd = open(path.toUtf8().data(), O_RDONLY);
    if(fd == -1){
        return -2;
    }

    return 0;
}

int IIODevice::closeDevice()
{
    QString cmdStr;
    QByteArray cmdBuffer;
    char *cmd;
    // disable buffer
    cmdStr = "echo 0 > /sys/bus/iio/devices/"+fileName+"/buffer/enable";
    cmdBuffer = cmdStr.toLocal8Bit();
    cmd = cmdBuffer.data();
    system(cmd);

    // disable iio channel
    for(int i=0;i<1;i++){
        cmdStr = QString("echo 0 >/sys/bus/iio/devices/%1/in_voltage%2_en").arg(fileName).arg(i);
        cmdBuffer = cmdStr.toLocal8Bit();
        cmd = cmdBuffer.data();
        system(cmd);
    }

    if(fd){

        ::close(fd);
        fd = -1;
    }
    return 0;
}

void IIODevice::trigger()
{
    QString cmdStr;
    QByteArray cmdBuffer;
    char *cmd;
    cmdStr = "echo 1 > /sys/bus/iio/devices/trigger0/trigger_now";
    cmdBuffer = cmdStr.toLocal8Bit();
    cmd = cmdBuffer.data();
    system(cmd);
}

int IIODevice::readDevice(unsigned char *buffer, int size)
{
    int length = 0;
    //the size must meet the channel*data length
    // current 2 for single channel ad
    length = read(fd,buffer,2);
    return length;
}

IIODevice::~IIODevice()
{
    if(fd)
        close(fd);
}

void IIODevice::startSample()
{
    _mutex->lock();
    _abort = false;
    _started = true;
    if(!buffer->isEmpty())
        buffer->remove(0,buffer->size());
    _mutex->unlock();
    openDevice();
    start();
}

bool IIODevice::isStarted()
{
    return _started;
}

void IIODevice::stopSample()
{
    _mutex->lock();
    _abort = true;
    _started = false;
    closeDevice();
    _mutex->unlock();

    wait();
}

#define TEMP_BUFFER_SIZE 4096
void IIODevice::run()
{
    int bytes = 0;
    unsigned char devBuf[TEMP_BUFFER_SIZE];
    while(true){
        bytes = 4096;
        while(bytes == 4096){
            bytes = readDevice(devBuf,TEMP_BUFFER_SIZE);
            _mutex->lock();
            buffer->append((const char*)devBuf,bytes);
            _mutex->unlock();
        }
        if(_abort){
            return;
        }
        usleep(600);
    }
}

int IIODevice::readChannel(int channel)
{
    QString cmdStr;
    QByteArray cmdBuffer;
    cmdStr = QString("cat /sys/bus/iio/devices/%1/in_voltage%2_raw").arg(fileName).arg(channel);

    //QProcess process;
    //process.start(cmdStr);
    //process.waitForFinished();
    _iop->start(cmdStr);
    _iop->waitForFinished();
    QString ret = _iop->readAll();

    return ret.toInt();
}
