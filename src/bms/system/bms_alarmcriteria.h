#ifndef BMS_ALARMCRITERIA_H
#define BMS_ALARMCRITERIA_H

#include <QObject>
#include <QDateTime>

class WAState
{
public:
    enum Type{
        TYPE_NONE,
        TYPE_WARNING,
        TYPE_ALARM,
    };

    explicit WAState();
    void reset();
    bool isSet;
    bool isReset;
    QDateTime setTime;
    QDateTime resetTime;
    bool isTransition;
    Type actionType;
    double value;

    bool warningSet;
    bool warningReset;
    bool alarmSet;
    bool alarmReset;
};

class SetResetPair
{
public:
    enum Comparator{
        CMP_GT,
        CMP_GE,
        CMP_LT,
        CMP_LE
    };

    explicit SetResetPair(double set, double reset, Comparator compare, int duration = 10);
    void validate(double value, WAState *state, WAState::Type type);
    void setDuration(int secs);
    SetResetPair::Comparator type() const;
    double set();
    double reset();
    int duration();

private:
    double _set;
    double _reset;
    Comparator _comparator;
    int _duration;
};

class Criteria{
public:
    explicit Criteria();
    QString name() const;
    bool enabled() const;
    int time() const;
    QString label() const;
    SetResetPair *high() const;
    SetResetPair *low() const;

    void setName(QString name);
    void setEnable(bool set);
    void setTime(int time);
    void setLabel(QString label);
    void setHigh(float set, float reset, SetResetPair::Comparator cmp, int time);
    void setLow(float set, float reset, SetResetPair::Comparator cmp, int time);
private:
    SetResetPair *_highLmt;
    SetResetPair *_lowLmt;
    QString _name;
    bool _enabled;
    QString _label;
    int _time;
};

class AlarmManager : public QObject
{
    Q_OBJECT
public:
    explicit AlarmManager(QObject *parent = nullptr);
    explicit AlarmManager(int packs, int cells, int ntcs, QObject *parent = nullptr);
    // AlarmType should map to config.ini
    enum AlarmType{
        CV_WARNING,
        CV_ALARM,
        CT_WARNING,
        CT_ALARM,
        SOC_WARNING,
        SOC_ALARM,
        PV_WARNING,
        PV_ALARM,
        PA_WARNING,
        PA_ALARM,
        NOF_ALARM
    };
    enum CriteriaType{
        CELL_VOLTAGE,
        CELL_TEMPERATURE,
        SOC
    };

    QList<AlarmType> alarm();
    void set_cell_voltage(int id, double value);
    void set_cell_temperature(int id, double value);
    void set_soc(double value);
    void set_pack_voltage(double value);
    void set_pack_current(double value);

    void addCriteria(SetResetPair *criteria, CriteriaType type);

    bool isCvHAlarm() const;
    bool isCvHWarning() const;
    bool isCtHAlarm() const;
    bool isCtHWarning() const;
    bool isCvLAlarm() const;
    bool isCvLWarning() const;
    bool isCtLAlarm() const;
    bool isCtLWarning() const;
    bool isSocAlarm() const;
    bool isSocWarning() const;
    bool isPvHWarning() const;
    bool isPvHAlarm() const;
    bool isPvLWarning() const;
    bool isPvLAlarm() const;
    bool isPaWarning() const;
    bool isPaAlarm() const;

    bool isWarning();
    bool isAlarm();
    bool isEvent();

    void resetState();

    //void set_cell_voltage_criteria(QList<SetResetPair*> warning, QList<SetResetPair*> alarm);
    //void set_cell_temperature_criteria(QList<SetResetPair*> warning, QList<SetResetPair*> alarm);
    //void set_soc_criteria(QList<SetResetPair*> warning, QList<SetResetPair*> alarm);

    int maxCvPos() const;
    int minCvPos() const;
    int maxCtPos() const;
    int minCtPos() const;
    double maxCv() const;
    double minCv() const;
    double maxCt() const;
    double minCt() const;
    double voltage() const;
    double current() const;

    QString eventString();

signals:

public slots:

private:
//    QList<SetResetPair*> _cv_alarm;
//    QList<SetResetPair*> _ct_alarm;
//    QList<SetResetPair*> _cv_warning;
//    QList<SetResetPair*> _ct_warning;
//    QList<SetResetPair*> _soc_alarm;
//    QList<SetResetPair*> _soc_warning;

    Criteria *_criterias[NOF_ALARM];

    bool _cvhAlarm;
    bool _cvhWarning;
    bool _cvlAlarm;
    bool _cvlWarning;
    bool _cthAlarm;
    bool _cthWarning;
    bool _ctlAlarm;
    bool _ctlWarning;
    bool _socAlarm;
    bool _socWarning;
    bool _pvhAlarm;
    bool _pvhWarning;
    bool _pvlAlarm;
    bool _pvlWarning;
    bool _paAlarm;
    bool _paWarning;

    int _packs;
    int _cells_per_pack;
    int _ntcs_per_pack;

    QList<WAState *> _cvhStates;
    QList<WAState *> _cthStates;
    QList<WAState *> _cvlStates;
    QList<WAState *> _ctlStates;
    WAState *_socStates;
    WAState *_pvhStates;
    WAState *_pvlStates;
    WAState *_pahStates;
    WAState *_palStates;

    int _maxCvPos,_minCvPos;
    int _maxCtPos,_minCtPos;
    double _maxCv,_minCv;
    double _maxCt,_minCt;

    bool _isCvEvent;
    bool _isCtEvent;
    bool _isSocEvent;

    QString _eventString;

    double _packVoltage,_packCurrent;
    int _cell_count;
    int _ntc_count;

};

#endif // BMS_ALARMCRITERIA_H
