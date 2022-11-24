#ifndef BMS_ALARMCRITERIA_H
#define BMS_ALARMCRITERIA_H

#include <QObject>
#include <QDateTime>

class WAState
{
public:
    explicit WAState();
    bool isSet;
    bool isReset;
    QDateTime setTime;
    QDateTime resetTime;
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

    explicit SetResetPair(double set, double reset, Comparator compare);
    void validate(double value, WAState *state);
    void setDuration(int secs);

private:
    double _set;
    double _reset;
    Comparator _comparator;
    int _duration;
};

class AlarmManager : public QObject
{
    Q_OBJECT
public:
    explicit AlarmManager(QObject *parent = nullptr);
    explicit AlarmManager(int packs, int cells, int ntcs, QObject *parent = nullptr);
    enum AlarmType{
        CV_WARNING,
        CV_ALARM,
        CT_WARNING,
        CT_ALARM,
        SOC_WARNING,
        SOC_ALARM
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

    void addCriteria(SetResetPair *criteria, CriteriaType type);

    bool isCvAlarm() const;
    bool isCvWarning() const;
    bool isCtAlarm() const;
    bool isCtWarning() const;
    bool isSocAlarm() const;
    bool isSocWarning() const;

    void resetState();

    void set_cell_voltage_criteria(QList<SetResetPair*> warning, QList<SetResetPair*> alarm);
    void set_cell_temperature_criteria(QList<SetResetPair*> warning, QList<SetResetPair*> alarm);
    void set_soc_criteria(QList<SetResetPair*> warning, QList<SetResetPair*> alarm);


signals:

public slots:

private:
    QList<SetResetPair*> _cv_alarm;
    QList<SetResetPair*> _ct_alarm;
    QList<SetResetPair*> _cv_warning;
    QList<SetResetPair*> _ct_warning;
    QList<SetResetPair*> _soc_alarm;
    QList<SetResetPair*> _soc_warning;

    bool _cvAlarm;
    bool _cvWarning;
    bool _ctAlarm;
    bool _ctWarning;
    bool _socAlarm;
    bool _socWarning;

    int _packs;
    int _cells_per_pack;
    int _ntcs_per_pack;

    QList<WAState *> _cvStates;
    QList<WAState *> _ctStates;
    WAState *_socStates;
};

#endif // BMS_ALARMCRITERIA_H
