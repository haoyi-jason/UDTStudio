#ifndef BMSSTACK_H
#define BMSSTACK_H

#include <QObject>
#include <QMutex>
#include <QThread>

class BatteryPack;

#include "canopenbus.h"
#include "node.h"
#include "system/bms_alarmcriteria.h"

class BCUPollThread;

class BCU : public Node
{
    Q_OBJECT
public:

    enum BCU_CHARGE_STATE{
        IDLE,
        CHARGING,
        DISCHARGING
    };

    BCU(quint8 nodeId, const QString &name = QString(), const QString &edsFileName = QString());
    ~BCU();

    double voltage() const;
    double current() const;

    QList<BatteryPack*> packs() const;

    Status status();
    QString statusStr();

    // BCU specified functions
    void accessVoltage(quint8 pack, quint8 cell);

    quint8 nofPacks() const;
    quint8 nofCellsPerPack() const;
    quint8 nofNtcsPerPack() const;

    BCU_CHARGE_STATE state() const;
    double maxCell() const;
    double minCell() const;
    double maxTemperature() const;
    double minTemperature() const;
    quint8 maxCellPos() const;
    quint8 minCellPos() const;
    quint8 maxTempPos() const;
    quint8 minTempPos() const;
    qint16 cellDifference() const;

    AlarmManager *alarmManager() const;

public slots:
    void startPollThread(int interval=50);
    void validate();

signals:
    void threadActive(bool);
    void sendEvent(QString event);

private:
    double _voltage;
    double _current;
    QList<BatteryPack*> _packs;
    quint8 _nofPacks;
    quint8 _cellsPerPack;
    quint8 _ntcsPerPack;

    QMutex _mutex;
    friend class BCUPollThread;
    BCUPollThread *_pollThread;

    quint8 _currentPollId;
    quint8 _maxPollId;

    BCU_CHARGE_STATE _state;
    quint8 _maxCVid;
    quint8 _minCVid;
    quint8 _maxTid;
    quint8 _minTid;
    double _maxCellVoltage;
    double _minCellVoltage;
    qint16 _maxCellDifferenceMv;
    double _maxTemperature;
    double _minTemperature;

    AlarmManager *_alarmManager;
};


class BatteryPack:public QObject
{
    Q_OBJECT
public:
    explicit BatteryPack(QObject *parent = nullptr);
    QString name() const;
    double voltage() const;

    QString status();
    QVariant cellVoltage(int cell) const;
    QVariant temperature(int ntc) const;

private:
    void updateState();

signals:

private:
    QString _name;
    double _voltage;
    QList<uint16_t> _cellVoltage;
    QList<float> _ntcTemperature;

};


class BCUPollThread:public QThread
{
    Q_OBJECT
public:
    BCUPollThread(BCU *bcu);
    void stop();
    void setInterval(int value);

protected:
    void run() override;
    BCU *_bcu;

signals:

private:
    bool _stop;
    int _interval;
};

#endif // BMSSTACK_H
