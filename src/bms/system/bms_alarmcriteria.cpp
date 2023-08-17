#include "bms_alarmcriteria.h"
#include <QDateTime>
#include "gsettings.h"

WAState::WAState()
{
    reset();
}

void WAState::reset()
{
    isSet = false;
    isReset = false;
    setTime = QDateTime::currentDateTime();
    resetTime = QDateTime::currentDateTime();
    isTransition = false;
    actionType = TYPE_NONE;
}

/*** SetResetPair  ***/
SetResetPair::SetResetPair(double set, double reset, Comparator compare, int duration)
    :_set(set)
    ,_reset(reset)
    ,_comparator(compare)
    ,_duration(duration)
{

}

void SetResetPair::setDuration(int secs)
{
    _duration = secs;
}

SetResetPair::Comparator SetResetPair::type() const
{
    return _comparator;
}

double SetResetPair::set()
{
    return _set;
}

double SetResetPair::reset()
{
    return _reset;
}

int SetResetPair::duration()
{
    return _duration;
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
            //int secs = QDateTime::currentDateTime().secsTo(state->setTime);
            if(state->setTime.secsTo(QDateTime::currentDateTime()) > _duration){
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
        //state->isSet = false;
    }
    if(reset){
        if(!state->isReset){
            if(state->resetTime.secsTo(QDateTime::currentDateTime()) > _duration){
                state->isReset = true;
                state->resetTime = QDateTime::currentDateTime();
                state->isTransition = state->isSet;
                state->isSet = false;
                state->actionType = type;
            }
        }
        else{
            state->actionType = type;
        }
    }
    else{
        state->resetTime = QDateTime::currentDateTime();
        //state->isReset = false;
    }
}

/* Criteria */
Criteria::Criteria()
{
}

QString Criteria::name() const{
    return _name;
}

bool Criteria::enabled() const{
    return _enabled;
}

int Criteria::time() const{
    return _time;
}

QString Criteria::label() const{
    return _label;
}

SetResetPair *Criteria::high() const{
    return _highLmt;
}

SetResetPair *Criteria::low() const{
    return _lowLmt;
}

void Criteria::setName(QString name)
{
    _name = name;
}
void Criteria::setEnable(bool set)
{
    _enabled = set;
}

void Criteria::setTime(int time)
{
    _time = time;
}
void Criteria::setLabel(QString label)
{
    _label = label;
}
void Criteria::setHigh(float set, float reset, SetResetPair::Comparator cmp, int time)
{
    _highLmt = new SetResetPair(set,reset,cmp,time);
}

void Criteria::setLow(float set, float reset, SetResetPair::Comparator cmp,int time)
{
    _lowLmt = new SetResetPair(set,reset,cmp,time);
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
    _cell_count = _packs * _cells_per_pack;
    _ntc_count = _packs * _ntcs_per_pack;

    for(int i=0;i<NOF_ALARM;i++){
        _criterias[i] = GSettings::instance().criteria(i);
        if(_criterias[i]->time()<=0){
            _criterias[i]->setTime(5);
        }
    }

    for(int i=0;i<_cell_count;i++){
        _cvhStates.append(new WAState());
        _cvlStates.append(new WAState());
    }
    for(int i=0;i<_ntc_count;i++){
        _cthStates.append(new WAState());
        _ctlStates.append(new WAState());
    }
    _socStates = new WAState();
    _pvhStates = new WAState();
    _pvlStates = new WAState();
    _pahStates = new WAState();
    _palStates = new WAState();

    _maxCv = 0;
    _minCv = 5000;
    _maxCvPos = 0;
    _minCvPos = 0;
    _maxCt = -100;
    _minCt = 200;
    _maxCtPos = 0;
    _minCtPos = 0;

    _balMask = (quint32*)malloc(_packs * sizeof(quint32));
    _openWire = (quint32*)malloc(_packs * sizeof(quint32));

    resetState();
}

AlarmManager::~AlarmManager()
{
    foreach (auto a, _cvhStates) {
        delete a;
    }
    foreach (auto a, _cvlStates) {
        delete a;
    }
    foreach (auto a, _cthStates) {
        delete a;
    }
    foreach (auto a, _ctlStates) {
        delete a;
    }
    delete _socStates;
    delete _pvhStates;
    delete _pvlStates;
    delete _pahStates;
    delete _palStates;
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
    else{
        if(value > _maxCv){


            _maxCv = value;
            _maxCvPos = id;
        }
        if(value < _minCv){
            _minCv = value;
            _minCvPos = id;
        }
    }

    _isCvEvent = false;
    // new criteria class
    _criterias[CV_ALARM]->high()->validate(value,_cvhStates[id],WAState::TYPE_ALARM);
    if((!_cvhStates[id]->isSet) && (!_cvhStates[id]->isReset)){
        _criterias[CV_WARNING]->high()->validate(value,_cvhStates[id],WAState::TYPE_WARNING);
    }
    _criterias[CV_ALARM]->low()->validate(value,_cvlStates[id],WAState::TYPE_ALARM);
    if((!_cvlStates[id]->isSet) && (!_cvlStates[id]->isReset)){
        _criterias[CV_WARNING]->low()->validate(value,_cvlStates[id],WAState::TYPE_WARNING);
    }
}

void AlarmManager::set_cell_temperature(int id, double value)
{
    if(id == 0){
        _maxCtPos = _minCtPos = 0;
        _maxCt = value;
        _minCt = value;
    }
    else{
        if(value > _maxCt){
            _maxCt = value;
            _maxCtPos = id;
        }
        if(value < _minCt){
            _minCt = value;
            _minCtPos = id;
        }
    }
    // new criteria
    _criterias[CT_ALARM]->high()->validate(value,_cthStates[id],WAState::TYPE_ALARM);
    if((!_cthStates[id]->isSet) && (!_cthStates[id]->isReset)){
        _criterias[CT_WARNING]->high()->validate(value,_cthStates[id],WAState::TYPE_WARNING);
    }
    _criterias[CT_ALARM]->low()->validate(value,_ctlStates[id],WAState::TYPE_ALARM);
    if((!_ctlStates[id]->isSet) && (!_ctlStates[id]->isReset)){
        _criterias[CT_WARNING]->low()->validate(value,_ctlStates[id],WAState::TYPE_WARNING);
    }

//    if(id == (_ntc_count-1)){
//        bool _otw = false;
//        bool _utw = false;
//        bool _ota = false;
//        bool _uta = false;
//        for(int i=0;i<_ntc_count;i++){
//            switch(_cthStates[i]->actionType){
//            case WAState::TYPE_WARNING: _otw |= _cvhStates[i]->isSet;break;
//            case WAState::TYPE_ALARM: _ota |= _cvhStates[i]->isSet;break;
//            }
//            switch(_ctlStates[i]->actionType){
//            case WAState::TYPE_WARNING: _utw |= _cvhStates[i]->isSet;break;
//            case WAState::TYPE_ALARM: _uta |= _cvhStates[i]->isSet;break;
//            }
//        }

//        _cthWarning = _otw;
//        _cthAlarm = _ota;
//        _ctlWarning = _utw;
//        _ctlAlarm = _uta;
//    }

}

void AlarmManager::set_soc(double value)
{
    _criterias[SOC_ALARM]->high()->validate(value,_socStates,WAState::TYPE_ALARM);
    if(!_socStates->isSet && !_socStates->isReset){
        _criterias[SOC_WARNING]->high()->validate(value,_socStates,WAState::TYPE_WARNING);
    }
    _criterias[SOC_ALARM]->low()->validate(value,_socStates,WAState::TYPE_ALARM);
    if(!_socStates->isSet && !_socStates->isReset){
        _criterias[SOC_WARNING]->low()->validate(value,_socStates,WAState::TYPE_WARNING);
    }
}

void AlarmManager::set_pack_voltage(double value)
{
    _packVoltage = value;
    _criterias[PV_ALARM]->high()->validate(value,_pvhStates,WAState::TYPE_ALARM);
    if(!_pvhStates->isSet && !_pvhStates->isReset){
        _criterias[PV_WARNING]->high()->validate(value,_pvhStates,WAState::TYPE_WARNING);
    }
    _criterias[PV_ALARM]->low()->validate(value,_pvlStates,WAState::TYPE_ALARM);
    if(!_pvlStates->isSet && !_pvlStates->isReset){
        _criterias[PV_WARNING]->low()->validate(value,_pvlStates,WAState::TYPE_WARNING);
    }
}

void AlarmManager::set_pack_current(double value)
{
    _packCurrent = value;
    _criterias[PA_ALARM]->high()->validate(value,_pahStates,WAState::TYPE_ALARM);
    if(!_pahStates->isSet && !_pahStates->isReset){
        _criterias[PA_WARNING]->high()->validate(value,_pahStates,WAState::TYPE_WARNING);
    }
    _criterias[PA_ALARM]->low()->validate(value,_palStates,WAState::TYPE_ALARM);
    if(!_palStates->isSet && !_palStates->isReset){
        _criterias[PA_WARNING]->low()->validate(value,_palStates,WAState::TYPE_WARNING);
    }
}

void AlarmManager::resetState()
{
    _cvhAlarm = false;
    _cvhWarning = false;
    _cthAlarm = false;
    _cthWarning = false;
    _cvlAlarm = false;
    _cvlWarning = false;
    _ctlAlarm = false;
    _ctlWarning = false;
    _socAlarm = false;
    _socWarning = false;
    _pvhAlarm = false;
    _pvhWarning = false;
    _pvlAlarm = false;
    _pvlWarning = false;
    _paAlarm = false;
    _paWarning = false;

    foreach (WAState *s, _cvhStates) {
        s->reset();
    }
    foreach (WAState *s, _cvlStates) {
        s->reset();
    }
    foreach (WAState *s, _cthStates) {
        s->reset();
    }
    foreach (WAState *s, _ctlStates) {
        s->reset();
    }
    _socStates->reset();
    _pvhStates->reset();
    _pvlStates->reset();
    _pahStates->reset();
    _palStates->reset();
}

bool AlarmManager::isCvHWarning() const
{
    return _cvhWarning;
}

bool AlarmManager::isCvHAlarm() const
{
    return _cvhAlarm;
}

bool AlarmManager::isCvLWarning() const
{
    return _cvlWarning;
}

bool AlarmManager::isCvLAlarm() const
{
    return _cvlAlarm;
}

bool AlarmManager::isCtHWarning() const
{
    return _cthWarning;
}

bool AlarmManager::isCtHAlarm() const
{
    return _cthAlarm;
}

bool AlarmManager::isCtLWarning() const
{
    return _ctlWarning;
}

bool AlarmManager::isCtLAlarm() const
{
    return _ctlAlarm;
}

bool AlarmManager::isSocWarning() const
{
    return _socWarning;
}

bool AlarmManager::isSocAlarm() const
{
    return _socAlarm;
}

bool AlarmManager::isPvHWarning() const
{
    return _pvhWarning;
}

bool AlarmManager::isPvHAlarm() const
{
    return _pvhAlarm;
}

bool AlarmManager::isPvLWarning() const
{
    return _pvlWarning;
}

bool AlarmManager::isPvLAlarm() const
{
    return _pvlAlarm;
}
bool AlarmManager::isPaWarning() const
{
    return _paWarning;
}

bool AlarmManager::isPaAlarm() const
{
    return _paAlarm;
}

bool AlarmManager::isWarning()
{
    return (_cvhWarning || _cthWarning || _socWarning);
}

bool AlarmManager::isAlarm()
{
    return(_cvhAlarm || _cthAlarm || _socAlarm);
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
double AlarmManager::voltage() const
{
    return _packVoltage;
}
double AlarmManager::current() const
{
    return _packCurrent;
}

bool AlarmManager::isEvent()
{
    bool ret = false;
    foreach (WAState *w, _cvhStates) {
        ret |= w->isTransition;
    }
    foreach (WAState *w, _cvlStates) {
        ret |= w->isTransition;
    }
    foreach (WAState *w, _cthStates) {
        ret |= w->isTransition;
    }
    foreach (WAState *w, _ctlStates) {
        ret |= w->isTransition;
    }
    ret |= _socStates->isTransition;
    ret |= _pvhStates->isTransition;
    ret |= _pvlStates->isTransition;
    ret |= _pahStates->isTransition;
    ret |= _palStates->isTransition;

    QString _dateTime =QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss => ");
    _eventString ="" ;
    //int evtOrd = 1;
    QString sta = "";
    bool alarm = false;
    bool warning = false;
    for(int i=0;i<_cvhStates.count();i++){
        if(_cvhStates[i]->isTransition){
            ret |= _cvhStates[i]->isTransition;
            sta = "";
            if(_cvhStates[i]->isSet){
                sta = "OV";
                if(_cvhStates[i]->actionType == WAState::TYPE_WARNING){
                    warning = true;
                }
                else if(_cvhStates[i]->actionType == WAState::TYPE_ALARM){
                    alarm = true;
                }

            }
            else if(_cvhStates[i]->isReset){
                sta = "OV Clr";
            }
            _eventString += _dateTime;
            _eventString += QString("Cell[%1] ,%2 ,Value=%3, Level=%4\n")
                    .arg(QString("#%1-%2").arg(i/_cells_per_pack+1).arg(i%_cells_per_pack+1))
                    .arg(sta)
                    .arg(_cvhStates[i]->value)
                    .arg(_cvhStates[i]->actionType == WAState::TYPE_WARNING?"Warning":"Alarm");
        }
    }
    _cvhWarning = warning;
    _cvhAlarm = alarm;

    warning = alarm = false;
    for(int i=0;i<_cvlStates.count();i++){
        if(_cvlStates[i]->isTransition){
            ret |= _cvlStates[i]->isTransition;
            sta = "";
            if(_cvlStates[i]->isSet){
                sta = "UV";
                if(_cvlStates[i]->actionType == WAState::TYPE_WARNING){
                    warning = true;
                }
                else if(_cvlStates[i]->actionType == WAState::TYPE_WARNING){
                    alarm = true;
                }
            }
            else if(_cvlStates[i]->isReset){
                sta = "UV Clr";
            }
            _eventString += _dateTime;
            _eventString += QString("Cell[%1] ,%2 ,Value=%3, Level=%4\n")
                    .arg(QString("#%1-%2").arg(i/_cells_per_pack+1).arg(i%_cells_per_pack+1))
                    .arg(sta)
                    .arg(_cvlStates[i]->value)
                    .arg(_cvlStates[i]->actionType == WAState::TYPE_WARNING?"Warning":"Alarm");
        }

    }
    _cvlWarning = warning;
    _cvlAlarm = alarm;

    warning = alarm = false;
    for(int i=0;i<_cthStates.count();i++){
        if(_cthStates[i]->isTransition){
            if(_cthStates[i]->isSet){
                ret |= _cthStates[i]->isTransition;
                sta = "";
                if(_cthStates[i]->isSet){
                    sta = "OT";
                    if(_cthStates[i]->actionType == WAState::TYPE_WARNING){
                        warning = true;
                    }
                    else if(_cthStates[i]->actionType == WAState::TYPE_ALARM){
                        alarm = true;
                    }

                }
                else if(_cthStates[i]->isReset){
                    sta = "OT Clr";
                }
                _eventString += _dateTime;
                _eventString += QString("NTC[%1] ,%2 ,Value=%3, Level=%4\n")
                        .arg(QString("#%1-%2").arg(i/_ntcs_per_pack+1).arg(i%_ntcs_per_pack+1))
                        .arg(sta)
                        .arg(_cthStates[i]->value)
                        .arg(_cthStates[i]->actionType == WAState::TYPE_WARNING?"Warning":"Alarm");
            }
        }
    }
    _cthWarning = warning;
    _cthAlarm = alarm;

    warning = alarm = false;
    for(int i=0;i<_ctlStates.count();i++){
        if(_ctlStates[i]->isTransition){
            if(_ctlStates[i]->isSet){
                ret |= _ctlStates[i]->isTransition;
                sta = "";
                if(_ctlStates[i]->isSet){
                    sta = "UT";
                    if(_ctlStates[i]->actionType == WAState::TYPE_WARNING){
                        warning = true;
                    }
                    else if(_ctlStates[i]->actionType == WAState::TYPE_ALARM){
                        alarm = true;
                    }
                }
                else if(_ctlStates[i]->isReset){
                    sta = "UT Clr";
                }
                _eventString += _dateTime;
                _eventString += QString("NTC[%1] ,%2 ,Value=%3, Level=%4\n")
                        .arg(QString("#%1-%2").arg(i/_ntcs_per_pack+1).arg(i%_ntcs_per_pack+1))
                        .arg(sta)
                        .arg(_ctlStates[i]->value)
                        .arg(_ctlStates[i]->actionType == WAState::TYPE_WARNING?"Warning":"Alarm");
            }
        }
    }

    _ctlWarning = warning;
    _ctlAlarm = alarm;
    alarm = warning = false;

    if(_socStates->isTransition){
        ret |= _socStates->isTransition;
        sta = "";
        if(_socStates->isSet){
            sta = "USOC";
            if(_socStates->actionType == WAState::TYPE_WARNING){
                _socWarning = true;
                _socAlarm = false;
            }
            else if(_pvhStates->actionType == WAState::TYPE_ALARM){
                _socWarning = false;
                _socAlarm = true;
            }
        }
        else if(_socStates->isReset){
            sta = "USOC Clr";
            _socWarning = _socAlarm = false;
        }
        _eventString += _dateTime;
        _eventString += QString("SOC %1 ,Value=%2, Level=%3\n")
                .arg(sta)
                .arg(_socStates->value)
                .arg(_socStates->actionType == WAState::TYPE_WARNING?"Warning":"Alarm");
    }

    if(_pvhStates->isTransition){
        ret |= _pvhStates->isTransition;
        sta = "";
        if(_pvhStates->isSet){
            sta = "OPV";
            if(_pvhStates->actionType == WAState::TYPE_WARNING){
                _pvhWarning = true;
                _pvhAlarm = false;
            }
            else if(_pvhStates->actionType == WAState::TYPE_ALARM){
                _pvhWarning = false;
                _pvhAlarm = true;
            }
        }
        else if(_pvhStates->isReset){
            sta = "OPV Clr";
            _pvhWarning = _pvhAlarm = false;
        }
        _eventString += _dateTime;
        _eventString += QString("PV %1 ,Value=%2, Level=%3\n")
                .arg(sta)
                .arg(_pvhStates->value)
                .arg(_pvhStates->actionType == WAState::TYPE_WARNING?"Warning":"Alarm");
    }

    if(_pvlStates->isTransition){
        ret |= _pvlStates->isTransition;
        sta = "";
        if(_pvlStates->isSet){
            sta = "UPV";
            if(_pvlStates->actionType == WAState::TYPE_WARNING){
                _pvlWarning = true;
                _pvlAlarm = false;
            }
            else if(_pvlStates->actionType == WAState::TYPE_ALARM){
                _pvlWarning = false;
                _pvlAlarm = true;
            }
        }
        else if(_pvlStates->isReset){
            sta = "UPV Clr";
            _pvlWarning = _pvlAlarm = false;
        }
        _eventString += _dateTime;
        _eventString += QString("PV %1 ,Value=%2, Level=%3\n")
                .arg(sta)
                .arg(_pvlStates->value)
                .arg(_pvlStates->actionType == WAState::TYPE_WARNING?"Warning":"Alarm");
    }

    _eventString += "\n";

    return ret;
}

QString AlarmManager::eventString()
{
    return _eventString;
}

void AlarmManager::setOpenWire(int pack, quint32 val)
{
    if(pack < _packs){
        _openWire[pack] = val;
    }
}

quint32 AlarmManager::openWire(int pack) const
{
    if(pack < _packs){
        return _openWire[pack];
    }
    return 0xffffffff;
}

void AlarmManager::setBalMask(int pack, quint32 val)
{
    if(pack < _packs){
        _balMask[pack] = val;
    }
}

quint32 AlarmManager::balMask(int pack) const
{
    if(pack <_packs){
        return _balMask[pack];
    }
    return 0xffffffff;
}


/** alarm system */
