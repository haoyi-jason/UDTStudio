#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QList>

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


#endif // PERIPHERALINTERFACE_H
