#ifndef BMS_LOGGER_H
#define BMS_LOGGER_H

#include <QObject>
#include <QThread>

#include "bmsstack.h"
#include "node.h"

class BMS_Logger : public QThread
{
    Q_OBJECT
public:
    explicit BMS_Logger(QObject *parent = nullptr);
    void addBCU(BCU *bcu);
    void removeBCU(BCU *bcu);
    void setLogPath(QString path);
    QString logPath() const;


signals:

public slots:
    void startLog(int interval);
    void stopLog();
    void logEvent(QString event);
    void dataAccessed();

private slots:
    void log();
    void writeRecord(QString record);
    void generateRecord(BCU *bcu);
    QString generateHeader(BCU *bcu);

private:
    QList<BCU *> _bcuList;
    QTimer *_logTimer;
    QString _logPath;
    QString _currentFileName;
    QString _recordPath;
    QString _eventPath;
    QString _appPath;
    QMutex _mutex;

};


#endif // BMS_LOGGER_H
