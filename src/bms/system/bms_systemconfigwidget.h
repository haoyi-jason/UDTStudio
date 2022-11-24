#ifndef BMS_SYSTEMCONFIGWIDGET_H
#define BMS_SYSTEMCONFIGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QStackedWidget>
#include "bms_ui/focusededitor.h"


class BMS_SystemConfigWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit BMS_SystemConfigWidget(QWidget *parent = nullptr);
    void setActivePage(int id);

private:
    void createWidgets();
    QWidget *createHardwareWidget();
    QWidget *createBCUWidget();
    QWidget *createAlarmWidget();

    void loadSettings();
    void saveSettings(QString section, QString key , QString value);


signals:
    void resetSystem();

public slots:
    void saveSerialPortSetting();
    void saveCanSetting();
    void saveLanSetting();
    void serialPortConfigChanged(int index);
    void changeAlarmItem(int index);
    void alarmSettingChanged();
    void recordSettingChanged();
    void balanceSettingChanged();
    void modbusSettingChanged();

private:
    QComboBox *_cboPort;
    QComboBox *_cboBaudrate;
    // ethernet
    QCheckBox *_chkEthernet;
    QGroupBox *_grpEthernet;
    QList<FocusedEditor*> _ip;
    QList<FocusedEditor*> _gw;
    QList<FocusedEditor*> _mask;

    //QStackedWidget *_stackWidget;
    QLabel *_sdLabel;

    // modbus
    QCheckBox *_chkModbusRtu;
    QCheckBox *_chkModbusTcp;
    QComboBox *_cboRtuPort;
    FocusedEditor *_rtuSlaveId;
    FocusedEditor *_tcpPort;

    // records
    QGroupBox *_grpRecords;
    FocusedEditor *_storedDays;
    FocusedEditor *_eventRecordInterval;
    FocusedEditor *_eventRecordCount;

    //balancing
    QGroupBox *_grpBalancing;
    FocusedEditor *_balancingVoltage;
    FocusedEditor *_balancingTime;
    FocusedEditor *_balancingHystersis;

    // alarm
    QGroupBox *_grpAlarm;
    QComboBox *_cboAlarmType;
    QList<FocusedEditor*> _alarmEdits; // high-set, high-reset, low-set, low-reset, duration
    QList<QLabel*> _alarmLabels;


};

#endif // BMS_SYSTEMCONFIGWIDGET_H
