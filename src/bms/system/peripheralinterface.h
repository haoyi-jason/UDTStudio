#ifndef PERIPHERALINTERFACE_H
#define PERIPHERALINTERFACE_H

#include <QObject>
#include <QMutex>
#include <QThread>

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
    int readDevice(unsigned char *buffer, int size);
    bool isStarted();

protected:
    void run() override;

private:
    int fd;
    QString fileName;
    bool _abort;
    QByteArray *buffer;
    QMutex *_mutex;
    bool _started;
};


#endif // PERIPHERALINTERFACE_H
