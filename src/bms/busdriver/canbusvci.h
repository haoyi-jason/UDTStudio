#ifndef CANBUSVCI_H
#define CANBUSVCI_H

#include "canopen_global.h"

#include "../lib/canopen/busdriver/canbusdriver.h"
#include <QMutex>
#include <QThread>
#include <QLibrary>

class CanBusVCINotifierThread;

class CanBusVCI : public CanBusDriver
{
    Q_OBJECT
public:
    CanBusVCI(const QString &address);
    ~CanBusVCI() override;

public:
    bool connectDevice() override;
    void disconnectDevice() override;

    QCanBusFrame readFrame() override;
    bool writeFrame(const QCanBusFrame &qtframe) override;

    int message_count(int id);


    void setAddress(const QString &address) ;

private:
    int _can_port;
    QMutex _mutex;
    friend class CanBusVCINotifierThread;
    CanBusVCINotifierThread *_readNotifier;

    void notifyRead();

    QLibrary _vciLibrary;

protected slots:
    void handleError();
};

class CanBusVCINotifierThread:public QThread
{
    Q_OBJECT
public:
    CanBusVCINotifierThread(CanBusVCI *driver);

protected:
    int _can_port;
    void run() override;
    CanBusVCI *_driver;
};

#endif // CANBUSVCI_H
