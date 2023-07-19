#include "bms_logger.h"
#include <QApplication>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include "gsettings.h"

BMS_Logger::BMS_Logger(QObject *parent) : QThread(parent)
{
    _appPath = QApplication::applicationDirPath()+"/";
    _logTimer = new QTimer();
    connect(_logTimer,&QTimer::timeout,this,&BMS_Logger::log);

    _logPath = _appPath + GSettings::instance().bcuSection()->rec_path();
    _recordPath = _appPath + GSettings::instance().bcuSection()->rec_path();
}

void BMS_Logger::addBCU(BCU *bcu)
{
    if(bcu != nullptr){
        bool found = false;
        foreach (auto b, _bcuList) {
            if(b == bcu){
                found = true;
            }
        }
        if(!found){
            QMutexLocker locker(&_mutex);
            _bcuList.append(bcu);
            //connect(bcu,&BCU::sendEvent,this,&BMS_Logger::logEvent);
            connect(bcu,&BCU::dataAccessed,this,&BMS_Logger::dataAccessed);
        }
    }
}

void BMS_Logger::removeBCU(BCU *bcu)
{
    if(bcu != nullptr){
        bool found = false;
        foreach (auto b, _bcuList) {
            if(b == bcu){
                found = true;
            }
        }
        if(found){
            QMutexLocker locker(&_mutex);
            _bcuList.removeOne(bcu);
            disconnect(bcu,nullptr,this,nullptr);
        }
    }
}

void BMS_Logger::setLogPath(QString path)
{
    if(!QDir(path).exists()){
        QDir().mkdir(path);
    }
    _logPath = path;
}

QString BMS_Logger::logPath() const
{
    return _logPath;
}


void BMS_Logger::startLog(int interval)
{
//    qDebug()<<Q_FUNC_INFO;
    if(interval > 0 && !_logTimer->isActive()){
        _logTimer->start(interval * 1000);
    }
}

void BMS_Logger::stopLog()
{
    _logTimer->stop();
}

void BMS_Logger::log()
{
    QMutexLocker locker(&_mutex);
    foreach(BCU *b, _bcuList){
        if(b->node()->status() == Node::Status::STARTED && b->isConfigReady()){
            generateRecord(b);
        }
    }
}

void BMS_Logger::writeRecord(QString record)
{

}

QString BMS_Logger::generateHeader(BCU *bcu)
{
    if(bcu == nullptr) return QString();
    QString header = "";
    header += "Date,Time";
    header += ",總壓(V),電流(A),狀態,最高電芯電壓, 最低電芯電壓, 最高電芯位置, 最低電芯位置, 壓差(mv)";
    header += ",最高溫,最低溫, 最高溫位置, 最低溫位置";
    for(int i=0;i<bcu->nofPacks();i++){
        for(int j=0;j<bcu->nofCellsPerPack();j++){
            header += QString(",V[%1][%2](mV)").arg(i+1).arg(j+1);
        }
        for(int j=0;j<bcu->nofNtcsPerPack();j++){
            header += QString(",T[%1][%2](%3C)").arg(i+1).arg(j).arg(QChar(0xb0));
        }
    }
    header += "\n";
    return header;
}

void BMS_Logger::generateRecord(BCU *bcu)
{
    if(bcu == nullptr) return;
    AlarmManager *m = bcu->alarmManager();
    QString fileName =QString("%1/BCU[%2]-%3.csv").arg(_recordPath).arg(bcu->node()->nodeId()).arg(QDateTime::currentDateTime().toString("yyyyMMdd_hh"));
    QFile f(fileName);
    f.open(QIODevice::ReadWrite| QIODevice::Append);
    QTextStream ds(&f);

    if(f.size() == 0){ // new file
        ds << generateHeader(bcu);
    }

    QString dt = QDateTime::currentDateTime().toString("yyyy/MM/dd,hh:mm:ss");
    ds << dt;
    ds << QString(",%1").arg(m->voltage());
    ds << QString(",%1").arg(m->current());
    ds << QString(",%1").arg(bcu->chargeStr()); // state
    ds << QString(",%1").arg(m->maxCv()); // max cell
    ds << QString(",%1").arg(m->minCv()); // min cell
    ds << QString(",%1#%2").arg(m->maxCvPos()/bcu->nofCellsPerPack()+1).arg(m->maxCvPos()%bcu->nofCellsPerPack()+1); // max pos
    ds << QString(",%1#%2").arg(m->minCvPos()/bcu->nofCellsPerPack()+1).arg(m->minCtPos()%bcu->nofCellsPerPack()+1); // max pos
    ds << QString(",%1").arg(m->maxCv() - m->minCv()); // v diff
    ds << QString(",%1").arg(m->maxCt()); // max t
    ds << QString(",%1").arg(m->minCt()); // min t
    ds << QString(",%1#%2").arg(m->maxCtPos()/bcu->nofNtcsPerPack()+1).arg(m->maxCtPos()%bcu->nofNtcsPerPack()+1); // max t
    ds << QString(",%1#%2").arg(m->minCtPos()/bcu->nofNtcsPerPack()+1).arg(m->minCtPos()%bcu->nofNtcsPerPack()+1); // min t

    for(int i=0;i< bcu->nofPacks();i++){
        for(int j=0;j<bcu->nofCellsPerPack();j++){
            ds << QString(",%1").arg(bcu->node()->nodeOd()->value(0x2100 + i, 0x0a + j).toInt());
        }
        for(int j=0;j<bcu->nofNtcsPerPack();j++){
            ds << QString(",%1").arg(bcu->node()->nodeOd()->value(0x2100 + i, 0x1c + j).toDouble()/10.);
        }
    }
    ds <<"\n";
    f.close();
}

void BMS_Logger::logEvent(QString event)
{
    QString fileName =QString("%1/events/event%2.log").arg(_recordPath).arg(QDateTime::currentDateTime().toString("yyyyMMdd"));
    QFile f(fileName);
    f.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream ds(&f);
    ds << event;
    f.close();
}

void BMS_Logger::dataAccessed()
{
    BCU *bcu = static_cast<BCU*>(sender());
    generateRecord(bcu);
}
