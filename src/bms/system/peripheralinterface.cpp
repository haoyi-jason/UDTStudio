#include "peripheralinterface.h"
#include <QFile>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <QProcess>
#include <QString>
#include <QTextStream>
#include <QTimer>

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


/*  gpio handler */

GPIOHandler::GPIOHandler(QObject *parent)
    :GPIOHandler(parent,1,INPUT)
{

}

GPIOHandler::GPIOHandler(QObject *parent, int gpioId, GPIODIR dir)
    :QObject(parent)
    ,_gpio(gpioId)
    ,_dir(dir)
{
    QString cmdStr = QString("echo %1 > /sys/class/gpio/export").arg(_gpio);
    system(cmdStr.toUtf8().constData());

    _fd = -1;
    _file = nullptr;
    _timer = new QTimer();
    _lastValue = -1;
    connect(_timer,&QTimer::timeout,this,&GPIOHandler::handleTimeout);
    QString path = QString("/sys/class/gpio/gpio%1/").arg(_gpio);
    if(QFile(path).exists()){
        // set direction
        cmdStr = QString("echo %1 >/sys/class/gpio/gpio%2/direction").arg((dir == INPUT)?"in":"out").arg(_gpio);
        system(cmdStr.toUtf8().constData());

        openDevice();
        // read initial value
        readValue();


    }
}

void GPIOHandler::openDevice()
{
    _timer->stop();
    if(_file  == nullptr){
        QString path = QString("/sys/class/gpio/gpio%1/value").arg(_gpio);
        _file  = new QFile();
        _file->setFileName(path);
        _timer->start(100);
        //_file->open(QFile::ReadOnly);


//        if(_file->exists()){
//            _file->open(QFile::ReadOnly);
//            _notifier = new QSocketNotifier(_file->handle(),QSocketNotifier::Read);
//            connect(_notifier,&QSocketNotifier::activated,this,&GPIOHandler::handleActivate);
//        }
    }
}

int GPIOHandler::readValue()
{
    int value = -1;
    if(_file != nullptr){
        _file->open(QFile::ReadOnly);
        QTextStream ts(_file);
        value = ts.readLine().toInt();
        _file->close();
        if(_lastValue != value){
            _lastValue = value;
            emit gpioChanged(_gpio,_lastValue);
        }
    }
    return value;
}

void GPIOHandler::handleActivate(int socket)
{
    readValue();
}

void GPIOHandler::handleTimeout()
{
    readValue();
}


/* adc handler */

ADCHandler::ADCHandler(QObject *parent)
    :ADCHandler("",0,parent)
{

}

ADCHandler::ADCHandler(QString devName,int channel, QObject *parent)
    :_devName(devName)
    ,_ch(channel)
{
    _offset = 0;
    _scale = 1.0;
    _timer = new QTimer();
    connect(_timer,&QTimer::timeout,this,&ADCHandler::handleTimeout);
    openDevice();
}

void ADCHandler::openDevice()
{
    QString cmdStr = QString("/sys/bus/iio/devices/%1/in_voltage%2_raw").arg(_devName).arg(_ch);
    _file = new QFile(cmdStr);
    if(_file->exists()){
        _timer->start(100);
    }
    else{
        delete _file;
        _file = nullptr;
    }

}

void ADCHandler::setScale(float scale)
{
    _scale = scale;
}

void ADCHandler::setOffset(int offset)
{
    _offset = offset;
}



void ADCHandler::handleTimeout()
{
    readValue();
}

int ADCHandler::readValue()
{
    int value = -1;
    if(_file != nullptr){
        _file->open(QFile::ReadOnly);
        QTextStream ts(_file);
        value = ts.readLine().toInt();
        _file->close();
//        value -= _offset;
//        _scaledValue = value * _scale;
        emit updateValue(_ch,value);
    }
}

float ADCHandler::scaledValue()
{
    return _scaledValue;
}
