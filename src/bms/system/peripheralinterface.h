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
    explicit GPIOHandler(QObject *parent = nullptr, int gpioId = 1, GPIODIR dir = INPUT);

    int readValue();
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
signals:
    void updateValue(int id, int value);

private slots:
    void handleTimeout();
private:
    void openDevice();
private:
    QFile *_file;
    QString _devName;
    int _lastValue;
    QTimer *_timer;
    float _scale;
    int _offset;
    int _ch;
    float _scaledValue;
};

#endif // PERIPHERALINTERFACE_H
