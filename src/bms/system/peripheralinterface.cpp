#include "peripheralinterface.h"
#include <QFile>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <QProcess>
#include <QString>

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
    QString cmdStr;
    QByteArray cmdBuffer;

    // open first 2 channel only
    for(int i=0;i<2;i++){
        cmdStr = QString("/sys/bus/iio/devices/%1/in_voltage%2_raw").arg(fileName).arg(i);
        QFile *file = new QFile();
        file->setFileName(cmdStr);
        if(file->exists()){
            int _fd = ::open(cmdStr.toUtf8().data(), O_RDONLY | O_ASYNC);
            if(_fd > 0){
                _fds.append(_fd);
            }
        }
    }
    return 0;
}

int IIODevice::closeDevice()
{
    foreach(int _fd , _fds){
        ::close(_fd);
        _fds.removeOne(_fd);
    }
    return 0;
}

int IIODevice::readDevice()
{
    QString cmdStr;
    char buf[8];

    int sz = 0;
    for(int i=0;i<2;i++){
        cmdStr = QString("/sys/bus/iio/devices/%1/in_voltage%2_raw").arg(fileName).arg(i);
        QFile *file = new QFile();
        file->setFileName(cmdStr);
        if(file->exists()){
            int _fd = ::open(cmdStr.toUtf8().data(), O_RDONLY | O_ASYNC);
            if(_fd > 0){
                memset(buf,0,8);
                sz = ::read(_fd,buf,8);
                if(sz > 0)
                    _results[i] = QString(buf).toInt();
                ::close(_fd);
            }
        }
    }
    return 0;
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
    //openDevice();
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
    //closeDevice();
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
            bytes = readDevice();
        }
        if(_abort){
            return;
        }
        msleep(100);
    }
}

int IIODevice::readChannel(int channel)
{
    if(channel < 2){
        return _results[channel];
    }
    return -1;
}
