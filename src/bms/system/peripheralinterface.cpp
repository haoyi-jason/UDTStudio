#include "peripheralinterface.h"
#include <QFile>

#include <sys/stat.h>
#include <fcntl.h>

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
}

int IIODevice::openDevice()
{
    QString cmdStr = "echo 1 > /sys/bus/iio/devices/"+fileName+"/scan_elements/in_voltage0_en";
    QByteArray cmdBuffer = cmdStr.toLocal8Bit();
    char *cmd = cmdBuffer.data();
    system(cmd);

    cmdStr = "echo 1 > /sys/bus/iio/devices/"+fileName+"/buffer/enable";
    cmdBuffer = cmdStr.toLocal8Bit();
    cmd = cmdBuffer.data();
    system(cmd);

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
    cmdStr = "echo 0 > /sys/bus/iio/devices/" + fileName + "/buffer/enable";
    cmdBuffer = cmdStr.toLocal8Bit();
    char *cmd = cmdBuffer.data();
    system(cmd);

    cmdStr = "echo 0 > /sys/bus/iio/devices/"+fileName + "/scan_elements/in_voltage0_en";
    cmdBuffer = cmdStr.toLocal8Bit();
    cmd = cmdBuffer.data();
    system(cmd);

    if(fd){
        close(fd);
        fd = -1;
    }
    return 0;
}

int IIODevice::readDevice(unsigned char *buffer, int size)
{
    int length = 0;
    length = read(fd,buffer,size);
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
