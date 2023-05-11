#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QList>
#include <QSocketNotifier>
#include <QFile>
#include <QTimer>

class QProcess;

class PeripheralInterface
{
public:
    PeripheralInterface();
};


class IIODevice:public QThread
{
    Q_OBJECT
public:
    explicit IIODevice(QObject *parent = nullptr,QString devName = "iio:device0", QByteArray *pBuffer = nullptr, QMutex *pMutex = nullptr);
    ~IIODevice();

    int openDevice();
    int closeDevice();
    void startSample();
    void stopSample();
    int readDevice();
    bool isStarted();
    int readChannel(int channel);



protected:
    void run() override;

private:
    int fd;
    QString fileName;
    bool _abort;
    QByteArray *buffer;
    QMutex *_mutex;
    bool _started;
    QProcess *_iop;
    QList<int> _fds;
    int _results[2];
};

typedef enum{
    INPUT,
    OUTPUT,
}GPIODIR;

typedef enum{
    LOW,
    HIGH,
}GPIOSTATE;

class GPIOHandler:public QObject
{
    Q_OBJECT
public:
    explicit GPIOHandler(QObject *parent = nullptr);
    explicit GPIOHandler(int gpioId = 1, GPIODIR dir = INPUT, QObject *parent = nullptr);

    int readValue();
    void writeValue(int v);
    GPIODIR direction() const;
    int index() const;
    void setIndex(int index);
signals:
    void gpioChanged(int id, int value);

private slots:
    void handleActivate(int socket);
    void handleTimeout();
private:
    void openDevice();
private:
    QSocketNotifier *_notifier;
    QFile *_file;
    int _gpio;
    int _lastValue;
    GPIODIR _dir;
    int _fd;
    QTimer *_timer;
    int _index;
};

class ADCHandler:public QObject
{
    Q_OBJECT
public:
    explicit ADCHandler(QObject *parent = nullptr);
    explicit ADCHandler(QString devName ="", int channel=0, QObject *parent = nullptr);
    void setScale(float scale);
    void setOffset(int offset);

    int readValue();
    float scaledValue();

protected:
    virtual void processInternal() = 0;
    void handleTimeout();
    void openDevice();

signals:
    void updateValue(int id, QString value);

protected:
    QFile *_file;
    QString _devName;
    int _lastValue;
    QTimer *_timer;
    float _scale;
    int _offset;
    int _ch;
    float _scaledValue;
};

class NTCHandler:public ADCHandler
{
    Q_OBJECT
public:
    typedef enum {
        NTC_SHUNT_TOP,
        NTC_SHUNT_BOTTOM,
    }_NTC_SHUNT;
    Q_ENUM(_NTC_SHUNT);
    explicit NTCHandler(QString devName ="", int channel=0, QObject *parent = nullptr);
    void setParam(float ntc_res, float res_shunt, float beta, float beta_temp, float v_drive);
protected:
    void processInternal() override;

signals:
    void updateValue(int id, QString value);

private:
    float _shunt_res;
    float _ntc_beta;
    float _ntc_beta_temp;
    float _ntc_drive_v;
    float _ntc_resistance;
};
#endif // PERIPHERALINTERFACE_H
