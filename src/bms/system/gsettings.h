#ifndef GSETTINGS_H
#define GSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMutex>

class SystemPath;
class SerialPortConfig;


class GSettings:public QObject
{
    Q_OBJECT
public:
    QString GetInstanceName() const;
    static GSettings &instance();
    static void release();
    virtual ~GSettings();
    void LoadConfig(QString path);
    void StoreConfig(QString path);
    // log function
    static void Info(const QString &msg);
    static void Warning(const QString &msg);
    static void Debug(const QString &msg);


protected:
    GSettings(QObject *parent = nullptr);
    GSettings(const GSettings &other);
    GSettings& operator=(const GSettings &other);

    static QScopedPointer<GSettings> _instance;
    SystemPath *_pathes;
    QList<SerialPortConfig*> _serialPortConfig;
    QList<SerialPortConfig*> _canPortConfig;

};

class SystemPath
{
public:
    QString LogFolder;
    QString TraceFolder;
};

class SerialPortConfig
{
public:
    QString connection;
    ulong baudrate;
    QString parity;
    quint8 databits;
    QString stopbits;
    QString flowControl;
    QString mode;

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

#endif // GSETTINGS_H
