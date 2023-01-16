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

    Status status();
    QString statusStr();
    QString chargeStr();
    QString cvStr();
    QString ctStr();

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
    quint8 startMode() const;
    void setStartMode(quint8 mode);
    bool isConfigReady() ;
    void reConfig();
    bool canPoll();

    void reset();
    void identify();

public slots:
    void startPollThread(int interval=50);
    bool validate();
    void configReceived(quint16 index, quint8 subindex);
    void readConfig();
    void resetError();
    void startPoll(int interval = 50);
    void stopPoll();

signals:
    void threadActive(bool);
    void sendEvent(QString event);
    void configReady();
    void updateState();
    void identified();

private slots:
    void accessData();

private:
    double _voltage;
    double _current;
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
    quint16 _configReady;
    quint16 _identifyReady;

    quint8 _startMode;

    double _currentGap;

    QTimer *_pollTimer;
    QList<NodeObjectId> _accessIds;
    QList<NodeObjectId>::iterator _accessIdIterator;
    bool _accessOnece;
    NodeObjectId _currentOd;
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
