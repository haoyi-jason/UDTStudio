#ifndef BMSSTACKVIEW_H
#define BMSSTACKVIEW_H

#include <QWidget>

#include "busmanagerwidget.h"
#include "bms_busnodetreeview.h"
#include "bcu/bms_nodemanagerwidget.h"
#include "bms_busmanagerwidget.h"

#include "../lib/udtgui/canopen/nodemanagerwidget.h"
#include "../lib/udtgui/canopen/busnodestreeview.h"
#include "bcu/bms_bcumanagerwidget.h"
#include "system/bms_systemmanagerwidget.h"
#include <QActionGroup>
#include <QThread>
#include "system/bms_logger.h"
#include "system/bms_stackmanager.h"

#include "bms_ui/focusededitor.h"
#include "system/login.h"

// origin work styles
const QString style_normal = "padding:5px;border: 2px outset #AAAAAA; line-height:2; border-radius: 5px; color: #8888aa; font-weight:bold;background: #00FF00; font-size:16px;";
const QString style_warning = "padding:5px;border: 2px outset #AAAAAA; line-height:2; border-radius: 5px;color: #8888aa; font-weight:bold;background: #FFFF00; font-size:16px;";
const QString style_alarm = "padding:5px;border: 2px outset #AAAAAA; line-height:2; border-radius: 5px;  color: #8888aa; font-weight:bold;background: #FF0000; font-size:16px;";

const QString style_no = "padding:2px;border: 2px solid #AAAAAA; border-radius: 5px; font-weight:bold; color: #%1;background: %2; font-size:16px;";
//const QString style_ba = "padding:5px;border: 2px outset #AAAAAA; border-radius: 5px; color: #8888aa; font-weight:bold;background: #FFFF00; font-size:16px;";
//const QString style_ow = "padding:5px;border: 2px outset #AAAAAA; border-radius: 5px; color: #8888aa; font-weight:bold;background: #FF0000; font-size:16px;";
//const uint color_ow = 0x88FF88;
//const uint color_bal = 0xFF8888;
//const uint color_nor = 0x8888aa;
//const uint color_ow = 0xFFFF00;
//const unit color_oa = 0xFF0000;
//const uint color_uw = 0x00FFFF;
//const uint color_ua = 0x0000FF;

class BMSStackView : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    explicit BMSStackView(QWidget *parent = nullptr);
    BMSStackView(CanOpen *canopen, QWidget *parent = nullptr);

    CanOpen *canOpen() const;
    void setCanopen(CanOpen *canOpen);
    CanOpenBus* currentBus() const;
    void setLogger(BMS_Logger *logger);

    void setStackManager(BMS_StackManager *manager);
    BMS_StackManager *stackManager() const;

signals:
    void functionSelected(int);
public slots:
    void handleBcuChanged(BCU *bcu);
    void updateView();
    void enableUI(bool state);

protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags);  // TODO constify flags param

private slots:
    void handleStackSwitch();
    void handleHyperlink(QString link);
    void handleClearAlarm();

private:
    void createWidget();
    QString colorText(QString text, QString color);
    QString linkText(QString text, QString key);
    QString linkText2(QString text1,QString text2, QString key);

    CanOpen *_canopen;
    BMS_Logger *_logger;
    int _currentStack;
    BMS_StackManager *_stackManager;

    QList<QLabel*> _alarmLabels;
    QLabel *_stackInfo;
    QLabel *_hwInfo;
    QGroupBox *_gbConfig;
    QGroupBox *_gbSystem;
    QGroupBox *_gbStatus;
    QGroupBox *_gbSysControl;

    QList<FocusedEditor*> _editList;
    BCU *_activeBcu;

    QPushButton *_btnStartsStop;

    Login *_login;

};

#endif // BMSSTACKVIEW_H
