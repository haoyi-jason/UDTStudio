#include "bms_alarmcriteria.h"
#include <QDateTime>

WAState::WAState()
{
    isSet = false;
    isReset = false;
    setTime = QDateTime::currentDateTime();
    resetTime = QDateTime::currentDateTime();
}

/*** SetResetPair  ***/
SetResetPair::SetResetPair(double set, double reset, Comparator compare)
    :_set(set)
    ,_reset(reset)
    ,_comparator(compare)
{
}

void SetResetPair::setDuration(int secs)
{
    _duration = secs;
}

void SetResetPair::validate(double value, WAState *state)
{
    bool set = false;
    bool reset = false;
    switch(_comparator){
    case CMP_GT:
        if(value > _set){
            set = true;
        }
        if(value < _reset){
            reset = true;
        }
        break;
    case CMP_GE:
        if(value >= _set){
            set = true;
        }
        if(value <= _reset){
            reset = true;
        }
        break;
    case CMP_LT:
        if(value < _set){
            set = true;
        }
        if(value > _reset){
            reset = true;
        }
        break;
    case CMP_LE:
        if(value <= _set){
            set = true;
        }
        if(value >= _reset){
            reset = true;
        }
        break;
    }

    // = false;
    if(set){
        if(!state->isSet){
            if(QDateTime::currentDateTime().secsTo(state->setTime) > _duration){
                state->isSet = true;
                state->isReset = false;
                state->setTime = QDateTime::currentDateTime();
            }
        }
    }
    else{
        state->setTime = QDateTime::currentDateTime();
    }
    if(reset){
        if(!state->isReset){
            if(QDateTime::currentDateTime().secsTo(state->resetTime) > _duration){
                state->isReset = true;
                state->isSet = false;
                state->resetTime = QDateTime::currentDateTime();
            }
        }
    }
    else{
        state->resetTime = QDateTime::currentDateTime();
    }
}

/*** alarm criteria ***/

AlarmManager::AlarmManager(QObject *parent)
    :AlarmManager(2,12,4,parent)
{

}

AlarmManager::AlarmManager(int packs, int cells, int ntcs, QObject *parent)
    :QObject(parent)
    ,_packs(packs)
    ,_cells_per_pack(cells)
    ,_ntcs_per_pack(ntcs)
{
//    SetResetPair *srPair;
//    srPair = new SetResetPair(3.7,3.5,SetResetPair::Comparator::CMP_GT);
//    _cv_warning.append(srPair);
//    srPair = new SetResetPair(3.9,3.7,SetResetPair::Comparator::CMP_GT);
//    _cv_alarm.append(srPair);
//    srPair = new SetResetPair(3.2,3.3,SetResetPair::Comparator::CMP_LT);
//    _cv_warning.append(srPair);
//    srPair = new SetResetPair(3.0,3.1,SetResetPair::Comparator::CMP_LT);
//    _cv_alarm.append(srPair);

//    srPair = new SetResetPair(45,40,SetResetPair::Comparator::CMP_GT);
//    _ct_warning.append(srPair);
//    srPair = new SetResetPair(50,45,SetResetPair::Comparator::CMP_GT);
//    _ct_alarm.append(srPair);
//    srPair = new SetResetPair(0,10,SetResetPair::Comparator::CMP_LT);
//    _ct_warning.append(srPair);
//    srPair = new SetResetPair(-10,-5,SetResetPair::Comparator::CMP_LT);
//    _ct_alarm.append(srPair);

    for(int i=0;i<_packs * _cells_per_pack;i++){
        _cvStates.append(new WAState());
    }
    for(int i=0;i<_packs * _ntcs_per_pack;i++){
        _ctStates.append(new WAState());
    }
    _socStates = new WAState();
}

void AlarmManager::addCriteria(SetResetPair *criteria, CriteriaType type)
{

}

void AlarmManager::set_cell_voltage(int id,double value)
{
    if(id < _cvStates.count()){
        foreach (SetResetPair *s, _cv_alarm) {
            s->validate(value,_cvStates[id]);
//            foreach (WAState *w, _cvStates) {
//                s->validate(value,w);
//            }
        }
        foreach (SetResetPair *s, _cv_warning) {
            s->validate(value,_cvStates[id]);
//            foreach (WAState *w, _cvStates) {
//                s->validate(value,w);
//            }
        }
    }
}

void AlarmManager::set_cell_temperature(int id, double value)
{
    foreach (SetResetPair *s, _ct_alarm) {
        s->validate(value,_ctStates[id]);
//        foreach (WAState *w, _ctStates) {
//            s->validate(value,w);
//        }
    }
    foreach (SetResetPair *s, _ct_warning) {
        s->validate(value,_ctStates[id]);
//        foreach (WAState *w, _ctStates) {
//            s->validate(value,w);
//        }
    }
}

void AlarmManager::set_soc(double value)
{
    foreach (SetResetPair *s, _soc_alarm) {
        s->validate(value,_socStates);
//        foreach (WAState *w, _socStates) {
//            s->validate(value,w);
//        }
    }
    foreach (SetResetPair *s, _soc_warning) {
        s->validate(value,_socStates);
//        foreach (WAState *w, _socStates) {
//            s->validate(value,w);
//        }
    }

}

void AlarmManager::resetState()
{
    _cvAlarm = false;
    _cvWarning = false;
    _ctAlarm = false;
    _ctWarning = false;
    _socAlarm = false;
    _socWarning = false;
}

bool AlarmManager::isCvWarning() const
{
    return _cvWarning;
}

bool AlarmManager::isCvAlarm() const
{
    return _cvAlarm;
}

bool AlarmManager::isCtWarning() const
{
    return _ctWarning;
}

bool AlarmManager::isCtAlarm() const
{
    return _ctAlarm;
}

bool AlarmManager::isSocWarning() const
{
    return _socWarning;
}

bool AlarmManager::isSocAlarm() const
{
    return _socAlarm;
}

void AlarmManager::set_cell_voltage_criteria(QList<SetResetPair *> warning, QList<SetResetPair *> alarm)
{
    _cv_warning = warning;
    _cv_alarm = alarm;
}

void AlarmManager::set_cell_temperature_criteria(QList<SetResetPair *> warning, QList<SetResetPair *> alarm)
{
    _ct_warning = warning;
    _ct_alarm = alarm;
}

void AlarmManager::set_soc_criteria(QList<SetResetPair *> warning, QList<SetResetPair *> alarm)
{
    _soc_warning = warning;
    _soc_alarm = alarm;
}












/** alarm system */
