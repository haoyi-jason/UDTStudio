#include "peripheralinterface.h"
#include <QFile>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <QProcess>
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QDebug>
#include <QtMath>

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
    :GPIOHandler(1,INPUT,parent)
{

}

GPIOHandler::GPIOHandler(int gpioId, GPIODIR dir,QObject *parent)
    :QObject(parent)
    ,_gpio(gpioId)
    ,_dir(dir)
{
    QString cmdStr;
    _fd = -1;
    _file = nullptr;
    _timer = new QTimer();
    _lastValue = -1;
    _index = -1;
    connect(_timer,&QTimer::timeout,this,&GPIOHandler::handleTimeout);
    QString path = QString("/sys/class/gpio/gpio%1/").arg(_gpio);
    if(!QFile(path).exists()){
        cmdStr = QString("echo %1 > /sys/class/gpio/export").arg(_gpio);
        system(cmdStr.toUtf8().constData());
    }

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
            emit gpioChanged(_index,_lastValue);
        }
    }
    return value;
}

void GPIOHandler::writeValue(int v)
{
    if(v == _lastValue) return;

    int value = -1;
    if(_file != nullptr){
        _file->open(QFile::ReadWrite);
        QTextStream ts(_file);
        ts << ((v == 0)?0:1);
        value = ts.readLine().toInt();
        _file->close();
        if(v == value){
            _lastValue = value;
            emit gpioChanged(_gpio,_lastValue);
        }
    }
}

void GPIOHandler::handleActivate(int socket)
{
    readValue();
}

void GPIOHandler::handleTimeout()
{
    readValue();
}

GPIODIR GPIOHandler::direction() const
{
    return _dir;
}

int GPIOHandler::index() const
{
    return _index;
}

void GPIOHandler::setIndex(int index)
{
    qDebug()<<Q_FUNC_INFO<<index;
    _index = index;
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
    //qDebug()<<Q_FUNC_INFO;
    QString cmdStr = QString("/sys/bus/iio/devices/%1/in_voltage%2_raw").arg(_devName).arg(_ch);
    _file = new QFile(cmdStr);
    if(_file->exists()){
        _timer->start(1000);
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

//void ADCHandler::processInternal()
//{
//    qDebug()<<Q_FUNC_INFO;
//    emit updateValue(_ch,QString::number(readValue()));
//}

void ADCHandler::handleTimeout()
{
    processInternal();
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
//        emit updateValue(_ch,value);
    }
    return value;
}

float ADCHandler::scaledValue()
{
    return _scaledValue;
}


/**** NTC handler *****/
NTCHandler::NTCHandler(QString devName, int channel, QObject *parent)
    :ADCHandler(devName,channel,parent)
{
    _shunt_res = 10000;
    _ntc_resistance = 10000;
    _ntc_beta = 3435;
    _ntc_beta_temp = 25;
    _ntc_drive_v = 4.0;
}

void NTCHandler::setParam(float ntc_res,float res_shunt, float beta, float beta_temp, float v_drive)
{
    _ntc_resistance = ntc_res;
    _shunt_res = res_shunt;
    _ntc_beta = beta;
    _ntc_beta_temp = beta_temp;
    _ntc_drive_v = v_drive;
}

void NTCHandler::processInternal()
{
    //qDebug()<<Q_FUNC_INFO;
    float meas_volt = 0.;
    int ad_raw = readValue();
    meas_volt = 4.096 * ad_raw /32768;
    float r_act = (_ntc_drive_v - meas_volt)*_shunt_res/meas_volt;

    float temp = r_act / _ntc_resistance;
    temp = ::log(temp);
    temp /= _ntc_beta;
    temp += 1/(273.15 + _ntc_beta_temp);
    temp = 1.0/temp;
    temp -= 273.15;

    if(temp < -40) temp = -40;
    if(temp > 85) temp = 85;
    emit updateValue(_ch,QString::number(temp,'f',1));

}
