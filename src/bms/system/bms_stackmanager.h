#ifndef BMS_STACKMANAGER_H
#define BMS_STACKMANAGER_H

#include <QObject>
#include "bmsstack.h"
#include "system/bms_logger.h"
#include "system/bms_alarmcriteria.h"
#include "system/bms_modbusslave.h"
#include "canopen.h"
#include <QTimer>

static const QStringList alarm_map ={"總壓過壓","總壓欠壓","電芯過壓","電芯欠壓","電池過溫","電池低溫","BCU斷線"};

class BMS_StackManager:public QObject
{
    Q_OBJECT
public:
    BMS_StackManager();
    ~BMS_StackManager();
    void setCanOpen(CanOpen *canoopen);

    BCU *bcu() ;
    BCU *nextBcu() ;
    BCU *prevBcu() ;
    int currentBcuId() const;
    int totalBcus() ;
    void validateState(BCU *b);
    void clearAlarm();
    BMS_Logger *logger() const;

    double maxCV() const;
    double minCV() const;
    double maxCT() const;
    double minCT() const;
    double cvDiff() const;
    int maxCvPos() const;
    int minCvPos() const;
    int maxCtPos() const;
    int minCtPos() const;
    int maxCvPID() const;
    int minCvPID() const;
    int maxCtPID() const;
    int minCtPID() const;
    double packVoltage() const;
    double packCurrent() const;
    bool setCurrent(Node *node);

    QAction *actionScanBus() const;
public slots:
    void scanBus();
    void startActivity();
    void addBus(quint8 busId);
    void addBcu(quint8 nodeId);
    void removeBcu(quint8 nodeId);

    void bcuConfigReady();
    //void nodeNameChanged(QString name);
    void bcuDataAccessed();

private:
    void updateStackStatus();

private slots:
    void pollState();
    void scanDone();
    void dailyTimeout();

signals:
    void activeBcuChannged(BCU *bcu);
    void statusUpdated();
    void updateStatusText(QString, int);

private:
    //QList<BCU*> _bcus;
    CanOpen *_canopen;
    QList<CanOpenBus*> _canopenbus;
    BMS_Logger *_logger;
    //QList<AlarmManager*> _alarmManager;
    QMap<Node*,BCU*> _bcusMap;
    QMap<Node*,BCU*>::iterator _bcuIterator;
    int _currentBcuId;

    double _maxCV, _minCV, _maxCT, _minCT,_cvDiff;
    int _maxCVID,_minCVID,_maxCTID,_minCTID;
    int _maxCVPID,_minCVPID,_maxCTPID,_minCTPID;
    double _totalCurrent,_packVoltage;
    QTimer *_tmr_statemachine;
    QTimer *_tmr_scanbus;

    BMS_ModbusSlave *_mbSlave;

    QMutex _mtx_poll;
    QAction *_actScanBus;
    int _pollCounter;
};

#endif // BMS_STACKMANAGER_H
