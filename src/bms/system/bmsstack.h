#ifndef BMSSTACK_H
#define BMSSTACK_H

#include <QObject>
#include <QMutex>
#include <QThread>

class BatteryPack;

#include "canopenbus.h"
#include "node.h"
#include "system/bms_alarmcriteria.h"
#include "nodeodsubscriber.h"

class BCUPollThread;

class BCU : public QObject,public NodeOdSubscriber
{
    Q_OBJECT
public:

    enum BCU_CHARGE_STATE{
        IDLE,
        CHARGING,
        DISCHARGING
    };

    const int CV_START_ADDR= 0x0A;
    const int CT_START_ADDR = 0x1A;

//    BCU(quint8 nodeId, const QString &name = QString(), const QString &edsFileName = QString());
    BCU(QObject *parent = nullptr);
    BCU(Node *node,bool autoStart = false, QObject *parent = nullptr);
    ~BCU();

    Node *node() const;

    double voltage() const;
    double current() const;
    double soc() const;
    double soh() const;

    //Status status();
    QString statusStr();
    QString chargeStr();
//    QString cvStr();
//    QString ctStr();

    // BCU specified functions
    //void accessVoltage(quint8 pack, quint8 cell);

    quint8 nofPacks() const;
    quint8 nofCellsPerPack() const;
    quint8 nofNtcsPerPack() const;
    quint8 bmuType() const;
    quint8 cmdState() const;

    BCU_CHARGE_STATE state() const;
//    double maxCell() const;
//    double minCell() const;
//    double maxTemperature() const;
//    double minTemperature() const;
//    quint8 maxCellPos() const;
//    quint8 minCellPos() const;
//    quint8 maxTempPos() const;
//    quint8 minTempPos() const;
//    qint16 cellDifference() const;

    void setAlarmManager(AlarmManager *alarm);
    AlarmManager *alarmManager() const;
    quint8 startMode() const;
    void setStartMode(quint8 mode);
    bool isConfigReady() ;
    bool isConfigFail() const;
    void reConfig();
    bool canPoll();
    bool dataReady() const;
    //bool isPolling();
    void reset();
    //void identify();

    void setSimulate(bool state);
    bool isSimulate() const;
    int pollRetry() const;
    QDateTime lastSeen() const;
    void notifyUpdate();
    int scannedDevices() const;
    void startTransfer();
    void stopTransfer();
    bool isTransfer() const;
public slots:
    //void startPollThread(int interval=50);
    bool validate();
    //void configReceived(quint16 index, quint8 subindex);
    void readConfig();
    void resetError();
    void startPoll(int interval = 200);
    void stopPoll();
    void setNode(Node *node);
    void nodeNameChanged(QString name);
    void accessConfig();

signals:
    void threadActive(bool);
    void sendEvent(QString event);
    void configReady();
    void updateState();
    void identified();
    void stateChanged();
    void configFail();
    void dataAccessed();
    void modeChanged(bool); // simulation or not
    void deviceLost();
    void statusReported();
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;

private slots:
    void accessData();

private:
    double _voltage;
    double _current;
    double _soc;
    double _soh;
    quint8 _nofPacks;
    quint8 _cellsPerPack;
    quint8 _ntcsPerPack;


    QMutex _mutex;
    friend class BCUPollThread;
    //BCUPollThread *_pollThread;

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
    bool _configFail;

    quint8 _startMode;

    double _currentGap;

    QTimer *_pollTimer;
    QList<NodeObjectId> _accessIds;
    QList<NodeObjectId>::iterator _accessIdIterator;
    bool _accessOnece;
    NodeObjectId _currentOd;

    bool _pollConfig;
    int _pollRetry;

    Node *_node;
    uint16_t _errorCode;
    uint8_t _errorClass;
    QByteArray _errorDest;
    bool _autoStart;
    bool _simulate;
    QDateTime _lastSeen;
    int _pollTimes;
    bool _nameDefined;
    bool _configDone;
    quint8 _bmuType;
    quint8 _cmdState;
    bool _dataReady;
    QDateTime _firstPollTime;

    int _scannedDevice;

};


#endif // BMSSTACK_H
