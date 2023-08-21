#ifndef GSETTINGS_H
#define GSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMutex>
#include <QTimer>

#include "system/bms_alarmcriteria.h"

class SystemPath;
class SerialPortConfig;
class AbstractSection;
class SerialSection;
class LanSection;
class AlarmCriteriaSection;
class EthernetConfig;
class BCUSection;
class SystemSection;
class AccountSection;

class GSettings:public QObject
{
    Q_OBJECT
public:
    QString GetInstanceName() const;
    static GSettings &instance();
    static void release();
    virtual ~GSettings();
    void LoadConfig(QString path);
    void StoreConfig(QString path = "");
    // log function
    static void Info(const QString &msg);
    static void Warning(const QString &msg);
    static void Debug(const QString &msg);
    // settings
    Criteria* criteria(int id);
    int criteriaSize();
    SerialPortConfig *serialConfig(int id);
    int serialConfigSize();
    SerialPortConfig *canConfig(int id);
    int canconfigSize();
    EthernetConfig *lanConfig(int id);
    int lanConfigSize();

    SerialSection *serialSection() const;
    BCUSection *bcuSection() const;
    SystemSection *systemSection() const;
    AccountSection *accountSection() const;

    void setModified();
protected:
    GSettings(QObject *parent = nullptr);
    GSettings(const GSettings &other);
    GSettings& operator=(const GSettings &other);

    static QScopedPointer<GSettings> _instance;
    SystemPath *_pathes;
    QList<SerialPortConfig*> _serialPortConfig;
    QList<SerialPortConfig*> _canPortConfig;

private slots:
    void handleAutoSave();

private:
    SerialSection *_serialConfig;
    SerialSection *_canConfig;
    LanSection *_lanConfig;
    AlarmCriteriaSection *_criteriaConfig;
    QString _settingPath;
    BCUSection *_bcusection;
    SystemSection *_systemSection;
    AccountSection *_accountSection;

    bool _modified;
    int _autoSaveInterval;
    QTimer *_saveTimer;
};

class AbstractSection{

public:
    explicit AbstractSection(QString sectionName="SECTION"):_sectionName(sectionName){}
    QString sectionName() const{return _sectionName;}
    void setSectionName(QString name){_sectionName = name;}
    virtual void readSection(QSettings *s)=0;
    virtual void writeSection(QSettings *s)=0;
    int sectionSize() const{return _size;}
protected:
    QString _sectionName;
    int _size;
};

class SystemPath
{
public:
    explicit SystemPath(){}

    QString LogFolder;
    QString TraceFolder;
};

class SerialPortConfig
{
public:
    QString name;
    ulong baudrate;
    QString parity;
    quint8 databits;
    QString stopbits;
    QString flowControl;
    QString mode;
    QString connection;

    void operator=(SerialPortConfig &b){
        this->connection = b.connection;
        this->baudrate = b.baudrate;
        this->parity = b.parity;
        this->databits = b.databits;
        this->stopbits = b.stopbits;
        this->flowControl = b.flowControl;
        this->mode = b.mode;
    }
};

class EthernetConfig
{
public:
    bool UseDHCP;
    quint8 ip[4],gw[4],mask[4];
};

class SerialSection:public AbstractSection
{
public:
    explicit SerialSection(QString sectionName="SERIAL");
    SerialPortConfig *data(int index) const;
    void readSection(QSettings *s) override;
    void writeSection(QSettings *s) override;

private:
    QList<SerialPortConfig*> _data;
};

class LanSection:public AbstractSection
{
public:
    explicit LanSection(QString sectionName="LAN");
    EthernetConfig *data(int index) const;
    void readSection(QSettings *s) override;
    void writeSection(QSettings *s) override;

private:
    QList<EthernetConfig*> _data;
};

class AlarmCriteriaSection:public AbstractSection
{
public:
    enum GROUP_e{
        GRP_CVWARN,
        GRP_CVALM,
        GRP_CTWARN,
        GRP_CTALM,
        GRP_SOCWARN,
        GRP_SOCALM,
        GRP_SVWARN,
        GRP_SVALM,
        GRP_SAWARN,
        GRP_SAALM
    };
    explicit AlarmCriteriaSection(QString sectionName="LAN");
    Criteria *data(int index) const;
    void readSection(QSettings *s) override;
    void writeSection(QSettings *s) override;

private:
    QList<Criteria*> _data;

};

class OutputControl{
public:
    int _level;
    quint16 objid;
    quint8 subid;
    quint16 on;
    quint16 mask;
};

class BCUSection:public AbstractSection
{
public:
    explicit BCUSection(QString sectionName="LAN");
    void readSection(QSettings *s) override;
    void writeSection(QSettings *s) override;
    // access functions
    void set_mb_tcp_enable(bool set);
    bool mb_tcp_enabled() const;
    void set_mb_rtu_enable(bool set);
    bool mb_rtu_enabled() const;
    void set_mb_rtu_connection(QString connection);
    QString mb_rtu_connection() const;
    void set_mb_tcp_port(int port);
    int mb_tcp_port() const;
    void set_mb_rtu_id(int id);
    int mb_rtu_id() const;

    void set_log_type(QString type);
    QString log_type() const;
    void set_log_keep_days(int value);
    int log_keep_days() const;
    void set_log_interval(int value);
    int log_interval() const;
    void set_rec_path(QString value);
    QString rec_path() const;
    void set_evt_path(QString value);
    QString evt_path() const;
    void set_sys_path(QString value);
    QString sys_path() const;
    void set_log_root(QString path);
    QString log_root_path() const;
    void set_move_to_path(QString path);
    QString move_to_path() const;


    void set_balancing_min(double value);
    double balancing_min() const;
    void set_balancing_max(double value);
    double balancing_max() const;
    void set_balancing_fault(double value);
    double balancing_fault() const;
    void set_balancing_gap(double value);
    double balancing_gap() const;
    void set_balancing_time(int value);
    int balancing_time() const;
    void set_balancing_voltage(double value);
    double balancing_voltage() const;

    OutputControl *outputConctrol(int id);


private:
    //modbus
    bool _mb_tcp_enable;
    bool _mb_rtu_enable;
    QString _mb_rtu_connection;
    int _mb_tcp_port;
    int _mb_sid;
    // log
    QString _log_keepType;
    int _log_keep_days;
    int _log_interval_sec;
    QString _recPath;
    QString _evtPath;
    QString _sysPath;
    QString _logRoot;
    QString _logMoveTo;
    // balancing
    double _min_bal_volt;
    double _max_bal_volt;
    double _fault_diff;
    double _gap;
    double _bal_voltage;
    int _bal_active_time_sec;
    // alarm i/o control
    QList<OutputControl*> _outputControl;
};

class SystemSection:public AbstractSection
{
public:
    explicit SystemSection(QString sectionName="SYSTEM");
    void readSection(QSettings *s) override;
    void writeSection(QSettings *s) override;

    int backlightTime() const;

private:
    int _backlight_off_time_sec;
    int _auto_save_interval_sec;


};

class AccountSection:public AbstractSection
{
public:
    explicit AccountSection(QString sectionName="ACCOUNT");
    void readSection(QSettings *s) override;
    void writeSection(QSettings *s) override;

    void setPasswd(QString name, QString passwd);
    QString passwd(QString name);

private:
    QMap<QString,QString> _accounts;

};

#endif // GSETTINGS_H
