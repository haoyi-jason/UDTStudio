#include "bms_alarmcriteria.h"
#include <QDateTime>

WAState::WAState()
{
    isSet = false;
    isReset = false;
    setTime = QDateTime::currentDateTime();
    resetTime = QDateTime::currentDateTime();
    isTransition = false;
    actionType = TYPE_NONE;
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

void SetResetPair::validate(double value, WAState *state, WAState::Type type)
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
    state->isTransition = false;
    state->actionType = WAState::TYPE_NONE;
    state->value = value;
    if(set){
        if(!state->isSet){
            if(QDateTime::currentDateTime().secsTo(state->setTime) > _duration){
                state->isSet = true;
                state->isReset = false;
                state->setTime = QDateTime::currentDateTime();
                state->isTransition = true;
                state->actionType = type;
            }
        }
        else{
            state->actionType = type;
        }
    }
    else{
        state->setTime = QDateTime::currentDateTime();
        state->isSet = false;
    }
    if(reset){
        if(!state->isReset){
            if(QDateTime::currentDateTime().secsTo(state->resetTime) > _duration){
                state->isReset = true;
                state->isSet = false;
                state->resetTime = QDateTime::currentDateTime();
                state->isTransition = true;
                state->actionType = type;
            }
        }
        else{
            state->actionType = type;
        }
    }
    else{
        state->resetTime = QDateTime::currentDateTime();
        state->isReset = false;
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
    if(id == 0){
        _maxCvPos = _minCvPos = 0;
        _maxCv = value;
        _minCv = value;
    }
    else if(value > _maxCv){
        _maxCv = value;
        _maxCvPos = id;
    }
    else if(value < _minCv){
        _minCv = value;
        _minCvPos = id;
    }

    if(id < _cvStates.count()){
        _isCvEvent = false;
        foreach (SetResetPair *s, _cv_alarm) {
            s->validate(value,_cvStates[id], WAState::TYPE_ALARM);
        }
        foreach (SetResetPair *s, _cv_warning) {
            s->validate(value,_cvStates[id], WAState::TYPE_WARNING);
        }
    }
}

void AlarmManager::set_cell_temperature(int id, double value)
{
    if(id == 0){
        _maxCtPos = _minCtPos = 0;
        _maxCt = value;
        _minCt = value;
    }
    else if(value > _maxCt){
        _maxCt = value;
        _maxCtPos = id;
    }
    else if(value < _minCt){
        _minCt = value;
        _minCtPos = id;
    }

    foreach (SetResetPair *s, _ct_alarm) {
        s->validate(value,_ctStates[id],WAState::TYPE_ALARM);
//        foreach (WAState *w, _ctStates) {
//            s->validate(value,w);
//        }
    }
    foreach (SetResetPair *s, _ct_warning) {
        s->validate(value,_ctStates[id],WAState::TYPE_WARNING);
//        foreach (WAState *w, _ctStates) {
//            s->validate(value,w);
//        }
    }
}

void AlarmManager::set_soc(double value)
{
    foreach (SetResetPair *s, _soc_alarm) {
        s->validate(value,_socStates,WAState::TYPE_ALARM);
//        foreach (WAState *w, _socStates) {
//            s->validate(value,w);
//        }
    }
    foreach (SetResetPair *s, _soc_warning) {
        s->validate(value,_socStates,WAState::TYPE_WARNING);
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

bool AlarmManager::isWarning()
{
    return (_cvWarning || _ctWarning || _socWarning);
}

bool AlarmManager::isAlarm()
{
    return(_cvAlarm || _ctAlarm || _socAlarm);
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


int AlarmManager::maxCvPos() const
{
    return _maxCvPos;
}

int AlarmManager::minCvPos() const
{
    return _minCvPos;
}

int AlarmManager::maxCtPos() const
{
    return _maxCtPos;
}

int AlarmManager::minCtPos() const
{
    return _minCtPos;
}

double AlarmManager::maxCv() const
{
    return _maxCv;
}

double AlarmManager::minCv() const
{
    return _minCv;
}
double AlarmManager::maxCt() const
{
    return _maxCt;
}
double AlarmManager::minCt() const
{
    return _minCt;
}

bool AlarmManager::isEvent()
{
    bool ret = false;
    foreach (WAState *w, _cvStates) {
        ret |= w->isTransition;
    }
    foreach (WAState *w, _ctStates) {
        ret |= w->isTransition;
    }
    ret |= _socStates->isTransition;

    _eventString = "";
    for(int i=0;i<_cvStates.count();i++){
        if(_cvStates[i]->isTransition){
            ret |= _cvStates[i]->isTransition;
            _eventString += QString("Cell[%1] ,%2 ,Value=%3, Level=%4\n")
                    .arg(i+1)
                    .arg(_cvStates[i]->isSet?"OV":"UV")
                    .arg(_cvStates[i]->value)
                    .arg(_cvStates[i]->actionType == WAState::TYPE_WARNING?"Warning":"Alarm");
        }

    }

    for(int i=0;i<_ctStates.count();i++){
        if(_ctStates[i]->isTransition){
            ret |= _ctStates[i]->isTransition;
            _eventString += QString("NTC[%1] ,%2 ,Value=%3, Level=%4\n")
                    .arg(i+1)
                    .arg(_ctStates[i]->isSet?"OT":"UT")
                    .arg(_ctStates[i]->value)
                    .arg(_ctStates[i]->actionType == WAState::TYPE_WARNING?"Warning":"Alarm");
        }
    }

    if(_socStates->isTransition){
        ret |= _socStates->isTransition;
        _eventString += QString("SOC %1 ,Value=%2, Level=%3\n")
                .arg(_socStates->isSet?"HIGH":"LOW")
                .arg(_socStates->value)
                .arg(_socStates->actionType == WAState::TYPE_WARNING?"Warning":"Alarm");
    }

    return ret;
}

QString AlarmManager::eventString()
{
    return _eventString;
}


/** alarm system */
