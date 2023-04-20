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
    _pathes->LogFolder = QString("%1/Log").arg(QCoreApplication::applicationDirPath());
    _pathes->TraceFolder = QString("%1/Trace").arg(QCoreApplication::applicationDirPath());

    if(!QDir(_pathes->LogFolder).exists()){
        QDir().mkpath(_pathes->LogFolder);
    }
    if(!QDir(_pathes->TraceFolder).exists()){
        QDir().mkpath(_pathes->TraceFolder);
    }
}

GSettings::~GSettings()
{

}

void GSettings::LoadConfig(QString path)
{
    if(!QFileInfo(path).exists()){
        return;
    }

    QSettings *setting = new QSettings(path,QSettings::IniFormat);
    setting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    int sz = setting->beginReadArray("SERIAL");
    if(sz > 0){
        for(int i=0;i<sz;i++){
            setting->setArrayIndex(i);
            SerialPortConfig *cfg = new SerialPortConfig();
            cfg->connection = setting->value("PORT").toString();
            cfg->baudrate = setting->value("BAUDRATE").toInt();
            cfg->parity = setting->value("PARITY").toString();
            cfg->databits = setting->value("DATABITS").toInt();
            cfg->stopbits = setting->value("STOPBITS").toString();
            cfg->flowControl = setting->value("FLOWCONTROL").toString();
            _serialPortConfig.append(cfg);
        }
    }
    setting->endArray();
    sz = setting->beginReadArray("CANBUS");
    if(sz > 0){
        for(int i=0;i<sz;i++){
            setting->setArrayIndex(i);
            SerialPortConfig *cfg = new SerialPortConfig();
            cfg->connection = setting->value("PORT").toString();
            cfg->baudrate = setting->value("BITRATE").toInt();
            cfg->mode = setting->value("MODE").toString();
            _canPortConfig.append(cfg);
        }
    }
    setting->endArray();


    sz = setting->beginReadArray("INTERFACES");

}

void GSettings::StoreConfig(QString path)
{

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
