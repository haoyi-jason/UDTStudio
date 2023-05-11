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
#include "screen/bms_eventviewer.h"
#include "system/bms_eventmodel.h"
#include "bms_ui/qledindicator.h"
#include "system/peripheralinterface.h"
class QLedIndicator;

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
    QWidget *createEventViewWidget();

    void loadSettings();
    void saveSettings(QString section, QString key , QString value);


signals:
    void resetSystem();
    void setDigitalOutput(int id,bool state);
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
    void updateDigitalInputs(int id, int value);
    void updateDigitalOutputs(int id, int value);
    void updateNTC(int id, QString value);
    void handleDigitalOutput();

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
    FocusedEditor *_balancingVoltageMin;
    FocusedEditor *_balancingVoltageMax;
    FocusedEditor *_balancingVoltageFaultDiff;

    // alarm
    QGroupBox *_grpAlarm;
    QComboBox *_cboAlarmType;
    QList<FocusedEditor*> _alarmEdits; // high-set, high-reset, low-set, low-reset, duration
    QList<QLabel*> _alarmLabels;

    // events
    BMS_EventModel *_evtModel;

    QList<QLabel*> _ntcLabels;
    QList<QLedIndicator*> _digitalOuts;
    QList<QLedIndicator*> _digitalIns;

};

#endif // BMS_SYSTEMCONFIGWIDGET_H
