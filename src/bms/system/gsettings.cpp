#include "gsettings.h"
#include <QDebug>
#include <QApplication>
#include <QTextCodec>
#include <QDateTime>
#include <QTextStream>
#include <QDir>

QScopedPointer<GSettings> GSettings::_instance;

GSettings::GSettings(QObject *parent):QObject(parent)
{
    _pathes = new SystemPath();
    _pathes->LogFolder = QString("%1/../data/Log").arg(QCoreApplication::applicationDirPath());
    _pathes->TraceFolder = QString("%1/../data/Trace").arg(QCoreApplication::applicationDirPath());

    if(!QDir(_pathes->LogFolder).exists()){
        QDir().mkpath(_pathes->LogFolder);
    }
    if(!QDir(_pathes->TraceFolder).exists()){
        QDir().mkpath(_pathes->TraceFolder);
    }

    _serialConfig = new SerialSection("SERIAL");
    _canConfig = new SerialSection("CANBUS");
    _lanConfig = new LanSection("ETHERNET");
    _criteriaConfig = new AlarmCriteriaSection("ALARM_SETTINGS");
    _bcusection = new BCUSection();
    _systemSection = new SystemSection("SYSTEM");
    _accountSection = new AccountSection("ACCOUNT");

    _settingPath = "";
    _modified = false;
    _autoSaveInterval = 60; //seconds
    _saveTimer = new QTimer();
    connect(_saveTimer,&QTimer::timeout,this,&GSettings::handleAutoSave);
    _saveTimer->start(_autoSaveInterval*1000);
}

GSettings::~GSettings()
{

}

void GSettings::handleAutoSave()
{
    qDebug()<<Q_FUNC_INFO;
    if(_modified){
        StoreConfig();
        _modified = false;
    }
}

void GSettings::setModified()
{
    _modified = true;
}

void GSettings::LoadConfig(QString path)
{
    _settingPath = path;
    if(!QFileInfo(path).exists()){
        return;
    }

    QSettings *setting = new QSettings(path,QSettings::IniFormat);
    setting->setIniCodec(QTextCodec::codecForName("UTF-8"));


    _serialConfig->readSection(setting);
    _canConfig->readSection(setting);
    _lanConfig->readSection(setting);
    _criteriaConfig->readSection(setting);
    _bcusection->readSection(setting);
    _systemSection->readSection(setting);
    _accountSection->readSection(setting);

}

void GSettings::StoreConfig(QString path)
{
    QSettings *setting = nullptr;
    if(path != ""){
        setting = new QSettings(path,QSettings::IniFormat);
    }
    if(setting == nullptr){
        if(_settingPath != ""){
            setting = new QSettings(_settingPath,QSettings::IniFormat);
        }
    }

    if(setting == nullptr) return;
    setting->setIniCodec(QTextCodec::codecForName("UTF-8"));
    _serialConfig->writeSection(setting);
    _canConfig->writeSection(setting);
    _lanConfig->writeSection(setting);
    _criteriaConfig->writeSection(setting);
    _bcusection->writeSection(setting);
    _systemSection->writeSection(setting);
    _accountSection->writeSection(setting);

}

GSettings &GSettings::instance()
{
    if(_instance.isNull()){
        _instance.reset(new GSettings());
    }
    return *_instance.data();
}

QString GSettings::GetInstanceName() const
{
    return "GSettings";
}

void GSettings::Info(const QString &msg)
{
    QString dateString = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString formattedTime = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss.zzz");

    QString filePath = QString("%1/%2-log.txt").arg(instance()._pathes->LogFolder ,dateString);
    QString txt = QString("[INFO] %1 %2").arg(formattedTime, msg);
    QFile outFile(filePath);
    if(outFile.open(QIODevice::WriteOnly | QIODevice::Append)){
        QTextStream ts(&outFile);
        ts << txt << endl;
        outFile.close();
    }
}

void GSettings::Warning(const QString &msg)
{
    QString dateString = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString formattedTime = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss.zzz");

    QString filePath = QString("%1/%2-log.txt").arg(instance()._pathes->LogFolder ,dateString);
    QString txt = QString("[WARNING] %1 %2").arg(formattedTime, msg);
    QFile outFile(filePath);
    if(outFile.open(QIODevice::WriteOnly | QIODevice::Append)){
        QTextStream ts(&outFile);
        ts << txt << endl;
        outFile.close();
    }
}

void GSettings::Debug(const QString &msg)
{
    QString dateString = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString formattedTime = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss.zzz");

    QString filePath = QString("%1/%2-log.txt").arg(instance()._pathes->TraceFolder ,dateString);
    QString txt = QString("[DEBUG] %1 %2").arg(formattedTime, msg);
    QFile outFile(filePath);
    if(outFile.open(QIODevice::WriteOnly | QIODevice::Append)){
        QTextStream ts(&outFile);
        ts << txt << endl;
        outFile.close();
    }

}

Criteria *GSettings::criteria(int id)
{
    return _criteriaConfig->data(id);
}

int GSettings::criteriaSize()
{
    return _criteriaConfig->sectionSize();
}

SerialPortConfig *GSettings::serialConfig(int id)
{
    return _serialConfig->data(id);
}
int GSettings::serialConfigSize()
{
    return _serialConfig->sectionSize();
}

SerialPortConfig *GSettings::canConfig(int id)
{
    return _canConfig->data(id);
}
int GSettings::canconfigSize()
{
    return _canConfig->sectionSize();
}

EthernetConfig *GSettings::lanConfig(int id)
{
    return _lanConfig->data(id);
}

int GSettings::lanConfigSize()
{
    return _lanConfig->sectionSize();
}

SerialSection *GSettings::serialSection() const
{
    return _serialConfig;
}

BCUSection *GSettings::bcuSection() const
{
    return _bcusection;
}

SystemSection *GSettings::systemSection() const
{
    return _systemSection;
}

AccountSection *GSettings::accountSection() const
{
    return _accountSection;
}

/* serialportsection */
SerialSection::SerialSection(QString sectionName)
:AbstractSection(sectionName)
{

}

SerialPortConfig *SerialSection::data(int index) const
{
    if(index < _data.size()){
        return _data.at(index);
    }
    return nullptr;
}

void SerialSection::readSection(QSettings *s)
{
    int sz = s->beginReadArray(_sectionName);
    _size = sz;
    SerialPortConfig *config;
    for(int i=0;i<sz;i++){
        if(i < _data.size()){
            config = _data.at(i);
        }
        else{
            config = new SerialPortConfig();
            _data.append(config);
        }
        s->setArrayIndex(i);
        config->name = s->value("PORT").toString();
        config->baudrate = s->value("BAUDRATE").toInt();
        config->parity = s->value("PARITY").toString();
        config->databits = s->value("DATABITS").toInt();
        config->stopbits = s->value("STOPBITS").toString();
        config->flowControl = s->value("FLOWCONTROL").toString();
        config->mode = s->value("MODE").toString();
        config->connection = s->value("CONNECTION").toString();
    }
    s->endArray();
}
void SerialSection::writeSection(QSettings *s)
{
    s->beginWriteArray(_sectionName);
    for(int i=0;i<_data.size();i++){
        s->setArrayIndex(i);
        SerialPortConfig *cfg = _data.at(i);
        s->setValue("PORT"       ,cfg->name );
        s->setValue("BAUDRATE"   ,QString::number(cfg->baudrate   ));
        s->setValue("PARITY"     ,cfg->parity     );
        s->setValue("DATABITS"   ,cfg->databits   );
        s->setValue("STOPBITS"   ,cfg->stopbits   );
        s->setValue("FLOWCONTROL",cfg->flowControl);
        s->setValue("MODE",cfg->mode);
        s->setValue("CONNECTION",cfg->connection);
    }

    s->endArray();
    s->sync();
}

/* lan section */
LanSection::LanSection(QString sectionName)
    :AbstractSection(sectionName)
{

}

EthernetConfig *LanSection::data(int index) const
{
    if(index < _data.size()){
        return _data.at(index);
    }
    return nullptr;
}

void LanSection::readSection(QSettings *s)
{
    int sz = s->beginReadArray(_sectionName);
    _size = sz;
    EthernetConfig *config;
    for(int i=0;i<sz;i++){
        if(i < _data.size()){
            config = _data.at(i);
        }
        else{
            config = new EthernetConfig();
            _data.append(config);
        }
        s->setArrayIndex(i);
        config->UseDHCP = s->value("DHCP").toString().contains("YES")?true:false;
        QStringList sl;
        QString str;
        str = s->value("IP").toString();
        sl = str.split(".");
        if(sl.size() == 4){
            for(int i=0;i<4;i++){
                config->ip[i] = sl[i].toInt();
            }
        }
        str = s->value("GATEWAY").toString();
        sl = str.split(".");
        if(sl.size() == 4){
            for(int i=0;i<4;i++){
                config->gw[i] = sl[i].toInt();
            }
        }
        str = s->value("MASK").toString();
        sl = str.split(".");
        if(sl.size() == 4){
            for(int i=0;i<4;i++){
                config->mask[i] = sl[i].toInt();
            }
        }
    }

    s->endArray();
}

void LanSection::writeSection(QSettings *s)
{
    int sz = _data.size();
    EthernetConfig *config;
    QString str;
    s->beginWriteArray(_sectionName);
    for(int i=0;i<sz;i++){
        config = _data.at(i);
        s->setArrayIndex(i);
        s->setValue("DHCP",config->UseDHCP?"YES":"NO");
        str = QString("%1.%2.%3.%4").arg(config->ip[0]).arg(config->ip[1]).arg(config->ip[2]).arg(config->ip[3]);
        s->setValue("IP",str);
        str = QString("%1.%2.%3.%4").arg(config->gw[0]).arg(config->gw[1]).arg(config->gw[2]).arg(config->gw[3]);
        s->setValue("GATEWAY",str);
        str = QString("%1.%2.%3.%4").arg(config->mask[0]).arg(config->mask[1]).arg(config->mask[2]).arg(config->mask[3]);
        s->setValue("MASK",str);
    }
    s->endArray();
    s->sync();
}


/* alarm criteria */
AlarmCriteriaSection::AlarmCriteriaSection(QString sectionName)
    :AbstractSection(sectionName){

}

Criteria *AlarmCriteriaSection::data(int index) const
{
    if(index < _data.size()){
        return _data.at(index);
    }
    return nullptr;
}

void AlarmCriteriaSection::readSection(QSettings *s)
{
    int sz = s->beginReadArray(_sectionName);
    _size = sz;
    Criteria* config;
    QStringList sl;
    int _time;
    for(int i=0;i<sz;i++){
        if(i < _data.size()){
            config = _data.at(i);
        }
        else{
            config = new Criteria();
            _data.append(config);
        }

        s->setArrayIndex(i);
        _time = s->value("TIME").toInt();

        config->setName(s->value("NAME").toString());
        config->setEnable(s->value("ENABLE").toString().contains("TRUE")?true:false);
        config->setTime(_time);
        config->setLabel(s->value("LABEL").toString());
        sl = s->value("HIGH").toString().split(",");
        if(sl.size() == 3){
            double set = sl[0].toDouble();
            double reset = sl[1].toDouble();
            SetResetPair::Comparator type = SetResetPair::CMP_GT;
            if(sl[2].contains("GE")){
                type = SetResetPair::CMP_GE;
            }
            else if(sl[2].contains("GT")){
                type = SetResetPair::CMP_GT;
            }
            else if(sl[2].contains("LE")){
                type = SetResetPair::CMP_LE;
            }
            else if(sl[2].contains("LT")){
                type = SetResetPair::CMP_LT;
            }
            config->setHigh(set,reset,type,_time);
        }
        sl = s->value("LOW").toString().split(",");
        if(sl.size() == 3){
            double set = sl[0].toDouble();
            double reset = sl[1].toDouble();
            SetResetPair::Comparator type = SetResetPair::CMP_GT;
            if(sl[2].contains("GE")){
                type = SetResetPair::CMP_GE;
            }
            else if(sl[2].contains("GT")){
                type = SetResetPair::CMP_GT;
            }
            else if(sl[2].contains("LE")){
                type = SetResetPair::CMP_LE;
            }
            else if(sl[2].contains("LT")){
                type = SetResetPair::CMP_LT;
            }
            config->setLow(set,reset,type,_time);
        }
    }
    s->endArray();
}

void AlarmCriteriaSection::writeSection(QSettings *s)
{
    int sz = _data.size();
    QString str,type;
    Criteria *config;
    s->beginWriteArray(_sectionName);
    for(int i=0;i<sz;i++){
        config = _data.at(i);
        SetResetPair *sr = config->high();
        s->setArrayIndex(i);
        s->setValue("NAME",config->name());
        switch(sr->type()){
        case SetResetPair::CMP_GE:type="GE";break;
        case SetResetPair::CMP_GT:type="GT";break;
        case SetResetPair::CMP_LE:type="LE";break;
        case SetResetPair::CMP_LT:type="LT";break;
        }
        str = QString("%1,%2,%3").arg(sr->set()).arg(sr->reset()).arg(type);
        s->setValue("HIGH",str);

        sr = config->low();
        switch(sr->type()){
        case SetResetPair::CMP_GE:type="GE";break;
        case SetResetPair::CMP_GT:type="GT";break;
        case SetResetPair::CMP_LE:type="LE";break;
        case SetResetPair::CMP_LT:type="LT";break;
        }
        str = QString("%1,%2,%3").arg(sr->set()).arg(sr->reset()).arg(type);
        s->setValue("LOW",str);
        s->setValue("ENABLE",config->enabled()?"TRUE":"FALSE");
        s->setValue("TIME",config->time());
        s->setValue("LABEL",config->label());
    }
    s->endArray();
    s->sync();

}


/** BCU section **/
BCUSection::BCUSection(QString sectionName)
    :AbstractSection(sectionName)
{

}

void BCUSection::readSection(QSettings *s)
{
    // modbus
    s->beginGroup("MODBUS");
    _mb_tcp_enable = s->value("TCP_ENABLE").toString().contains("TRUE");
    _mb_rtu_enable = s->value("RTU_ENABLE").toString().contains("TRUE");
    _mb_rtu_connection = s->value("RTU_PORT").toString();
    _mb_tcp_port = s->value("TCP_PORT").toInt();
    _mb_sid = s->value("ID").toInt();
    s->endGroup();

    // log
    s->beginGroup("LOG");
   _log_keepType = s->value("TYPE").toString();
   _log_keep_days = s->value("VALUE").toInt();
   _log_interval_sec = s->value("INTERVAL").toInt();
   _recPath = s->value("PATH_REC").toString();
   _evtPath = s->value("PATH_EVENT").toString();
   _sysPath = s->value("PATH_SYS").toString();
   _logRoot = s->value("LOG_ROOT").toString();
   _logMoveTo = s->value("LOG_MOVE_TO").toString();
   s->endGroup();

    // balancing
   s->beginGroup("CELL_BALANCE");
   _min_bal_volt = s->value("MINIMUM").toDouble();
   _max_bal_volt = s->value("MAXIMUM").toDouble();
   _fault_diff = s->value("FAULT_DIFF").toDouble();
   _gap = s->value("HYSTERSIS").toDouble();
   _bal_active_time_sec = s->value("TIME").toDouble();
   _bal_voltage = s->value("VOLTAGE").toDouble();
   s->endGroup();

   // alarm i/o
   int sz = s->beginReadArray("ALARM_CONTROL");
   for(int i=0;i<sz;i++){
       OutputControl *cfg;
       if(i < _outputControl.size()){
           cfg = _outputControl.at(i);
       }
       else{
           cfg = new OutputControl();
           _outputControl.append(cfg);
       }
       s->setArrayIndex(i);
       bool bOk;
       cfg->_level = s->value("LEVEL").toInt();
       cfg->objid = s->value("OBJID").toString().toInt(&bOk,16);
       cfg->subid = s->value("SUBID").toString().toInt(&bOk,16);
       cfg->on = s->value("ON").toString().toInt(&bOk,16);
       cfg->mask = s->value("MASK").toString().toInt(&bOk,16);
   }
    s->endArray();

}

void BCUSection::writeSection(QSettings *s)
{
    s->beginGroup("MODBUS");
    s->setValue("TCP_ENABLE",_mb_tcp_enable?"TRUE":"FALSE");
    s->setValue("RTU_ENABLE",_mb_rtu_enable?"TRUE":"FALSE");
    s->setValue("RTU_PORT",_mb_rtu_connection);
    s->setValue("TCP_PORT",_mb_tcp_port);
    s->setValue("ID",_mb_sid);
    s->endGroup();

    s->beginGroup("LOG");
    s->setValue("TYPE",_log_keepType);
    s->setValue("VALUE",_log_keep_days);
    s->setValue("INTERVAL",_log_interval_sec);
    s->setValue("PATH_REC",_recPath);
    s->setValue("PATH_EVENT",_evtPath);
    s->setValue("PATH_SYS",_sysPath);
    s->setValue("LOG_ROOT",_logRoot);
    s->setValue("LOG_MOVE_TO",_logMoveTo);
    s->endGroup();

    s->beginGroup("CELL_BALANCE");
    s->setValue("MINIMUM",_min_bal_volt);
    s->setValue("MAXIMUM",_max_bal_volt);
    s->setValue("FAULT_DIFF",_fault_diff);
    s->setValue("HYSTERSIS",_gap);
    s->setValue("TIME",_bal_active_time_sec);
    s->setValue("VOLTAGE",_bal_voltage);
    s->endGroup();

    s->beginWriteArray("ALARM_CONTROL");
    for(int i=0;i<_outputControl.size();i++){
        OutputControl *cfg = _outputControl.at(i);
        s->setArrayIndex(i);
        s->setValue("LEVEL",cfg->_level);
        s->setValue("OBJID",QString("0x%1").arg(cfg->objid,4,16,QLatin1Char('0')));
        s->setValue("SUBID",QString("0x%1").arg(cfg->subid,4,16,QLatin1Char('0')));
        s->setValue("ON",QString("0x%1").arg(cfg->on,4,16,QLatin1Char('0')));
        s->setValue("MASK",QString("0x%1").arg(cfg->mask,4,16,QLatin1Char('0')));
    }
    s->endArray();
}

void BCUSection::set_mb_tcp_enable(bool set)
{
    _mb_tcp_enable = set;
}
bool BCUSection::mb_tcp_enabled() const
{
    return _mb_tcp_enable;
}
void BCUSection::set_mb_rtu_enable(bool set)
{
    _mb_rtu_enable = set;
}
bool BCUSection::mb_rtu_enabled() const
{
    return _mb_rtu_enable;
}
void BCUSection::set_mb_rtu_connection(QString connection)
{
    _mb_rtu_connection = connection;
}
QString BCUSection::mb_rtu_connection() const
{
    return _mb_rtu_connection;
}
void BCUSection::set_mb_tcp_port(int port)
{
    _mb_tcp_port = port;
}
int BCUSection::mb_tcp_port() const
{
    return _mb_tcp_port;
}
void BCUSection::set_mb_rtu_id(int id)
{
    _mb_sid = id;
}
int BCUSection::mb_rtu_id() const
{
    return _mb_sid;
}

void BCUSection::set_log_type(QString type)
{
    _log_keepType = type;
}
QString BCUSection::log_type() const
{
    return _log_keepType;
}
void BCUSection::set_log_keep_days(int value)
{
    _log_keep_days = value;
}
int BCUSection::log_keep_days() const
{
    return _log_keep_days;
}
void BCUSection::set_log_interval(int value)
{
    _log_interval_sec = value;
}
int BCUSection::log_interval() const
{
    return _log_interval_sec;
}
void BCUSection::set_rec_path(QString value)
{
    _recPath = value;
}
QString BCUSection::rec_path() const
{
    return _recPath;
}
void BCUSection::set_evt_path(QString value)
{
    _evtPath = value;
}
QString BCUSection::evt_path() const
{
    return _evtPath;
}
void BCUSection::set_sys_path(QString value)
{
    _sysPath = value;
}
QString BCUSection::sys_path() const
{
    return _sysPath;
}
void BCUSection::set_log_root(QString path)
{
    _logRoot = path;
}
QString BCUSection::log_root_path() const
{
    return _logRoot;
}
void BCUSection::set_move_to_path(QString path)
{
    _logMoveTo = path;
}
QString BCUSection::move_to_path() const
{
   return _logMoveTo;
}

void BCUSection::set_balancing_min(double value)
{
    _min_bal_volt = value;
}
double BCUSection::balancing_min() const
{
    return _min_bal_volt;
}
void BCUSection::set_balancing_max(double value)
{
    _max_bal_volt = value;
}
double BCUSection::balancing_max() const
{
    return _max_bal_volt;
}
void BCUSection::set_balancing_fault(double value)
{
    _fault_diff = value;
}
double BCUSection::balancing_fault() const
{
    return _fault_diff;
}
void BCUSection::set_balancing_gap(double value)
{
    _gap = value;
}
double BCUSection::balancing_gap() const
{
    return _gap;
}
void BCUSection::set_balancing_time(int value)
{
    _bal_active_time_sec = value;
}
int BCUSection::balancing_time() const
{
    return _bal_active_time_sec;
}
void BCUSection::set_balancing_voltage(double value)
{
    _bal_voltage = value;
}
double BCUSection::balancing_voltage() const
{
    return _bal_voltage;
}
OutputControl *BCUSection::outputConctrol(int id)
{
    if(id < _outputControl.size()){
        return _outputControl.at(id);
    }
    return nullptr;
}

/* system config */
SystemSection::SystemSection(QString sectionName)
    :AbstractSection(sectionName)
{

}

void SystemSection::readSection(QSettings *s)
{
    s->beginGroup(_sectionName);
    _backlight_off_time_sec = s->value("BACKLIGHT_OFF_TIME").toInt();
    _auto_save_interval_sec = s->value("AUTO_SAVE_INTERVAL_SEC").toInt();
    s->endGroup();

}
void SystemSection::writeSection(QSettings *s)
{
    s->beginGroup(_sectionName);
    s->setValue("BACKLIGHT_OFF_TIME",_backlight_off_time_sec);
    s->setValue("AUTO_SAVE_INTERVAL_SEC",_auto_save_interval_sec);
    s->endGroup();

}

int SystemSection::backlightTime() const
{
    return _backlight_off_time_sec;
}

/* account */
AccountSection::AccountSection(QString sectionName)
    :AbstractSection(sectionName)
{

}

void AccountSection::readSection(QSettings *s)
{
    int sz = s->beginReadArray(_sectionName);
    for(int i=0;i<sz;i++){
        s->setArrayIndex(i);
        QString uname = s->value("NAME").toString();
        QString pass = s->value("PASSWD").toString();
        _accounts.insert(uname,pass);
    }
     s->endArray();
}
void AccountSection::writeSection(QSettings *s)
{
    s->beginWriteArray(_sectionName);

    for(int i=0;i<_accounts.size();i++){
        s->setArrayIndex(i);
        s->setValue("NAME",_accounts.keys().at(i));
        s->setValue("PASSWD",_accounts.values().at(i));
    }
    s->endArray();
}

void AccountSection::setPasswd(QString name, QString passwd)
{
    _accounts[name] = passwd;
}

QString AccountSection::passwd(QString name)
{
    return _accounts[name];
}
