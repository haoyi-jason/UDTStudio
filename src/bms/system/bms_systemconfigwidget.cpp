#include <QApplication>
#include <QVBoxLayout>
#include <QSettings>
#include <QTextCodec>
#include <QDebug>
#include <QtGlobal>
#include <QGridLayout>
#include "bms_systemconfigwidget.h"
#include <QTableView>
#include <QHeaderView>
#include <QScrollArea>
#include <QFormLayout>
#include <QComboBox>
#include "gsettings.h"
#include <QSpacerItem>

BMS_SystemConfigWidget::BMS_SystemConfigWidget(QWidget *parent) : QStackedWidget(parent)
{
    createWidgets();

    QFont f = font();
    f.setPointSize(16);
    setFont(f);
}

void BMS_SystemConfigWidget::createWidgets()
{
    addWidget(createHardwareWidget());
    addWidget(createBCUWidget());
    addWidget(createAlarmWidget());
    addWidget(createEventViewWidget());
}

void BMS_SystemConfigWidget::setActivePage(int id)
{
    //qDebug()<<Q_FUNC_INFO<<" ID="<<id;
    int page = id-1;
    if(page >= 0 && page < count()){
        setCurrentIndex(page);
    }
}

QWidget *BMS_SystemConfigWidget::createHardwareWidget()
{
    QWidget *widget = new QWidget();
    widget->setWindowTitle("Hardware Config");
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(5,5,5,5);
    layout->setSpacing(20);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(5,5,5,5);
    hlayout->setSpacing(10);
    QLabel *l = new QLabel("Hardware Config");
    l->setAlignment(Qt::AlignCenter);
    QFont f = l->font();
    f.setBold(true);
    l->setFont(f);
    layout->addWidget(l);
    _cboPort = new QComboBox();
    _cboBaudrate = new QComboBox();
    _cboBaudrate->addItem("9600");
    _cboBaudrate->addItem("19200");
    _cboBaudrate->addItem("38400");
    _cboBaudrate->addItem("57600");
    _cboBaudrate->addItem("115200");
    connect(_cboPort,qOverload<int>(&QComboBox::currentIndexChanged),this,&BMS_SystemConfigWidget::serialPortConfigChanged);
    connect(_cboBaudrate,qOverload<int>(&QComboBox::currentIndexChanged),this,&BMS_SystemConfigWidget::serialPortConfigChanged);
    QPushButton *btn;
    for(int i=0;i<GSettings::instance().serialConfigSize();i++){
        SerialPortConfig *cfg = GSettings::instance().serialConfig(i);
        _cboPort->addItem(cfg->name,cfg->connection);
    }
    _cboPort->setMinimumWidth(200);
    hlayout->addWidget(new QLabel("Port #"));
    hlayout->addWidget(_cboPort);

    _cboBaudrate->setMinimumWidth(200);
    hlayout->addWidget(new QLabel("Baudrate "));
    hlayout->addWidget(_cboBaudrate);
//    btn = new QPushButton(tr("Set"));
//    btn->setMinimumWidth(160);
//    connect(btn,&QPushButton::clicked,this,&BMS_SystemConfigWidget::saveSerialPortSetting);
//    hlayout->addWidget(btn);
    hlayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Fixed));
    QGroupBox *gb = new QGroupBox("Serial Ports");
    gb->setLayout(hlayout);
    layout->addWidget(gb);

    // ethernet config
    QGridLayout *glayout = new QGridLayout();
    glayout->setContentsMargins(2,2,2,2);
//    hlayout = new QHBoxLayout();
//    hlayout->setContentsMargins(0,0,0,0);
    _chkEthernet = new QCheckBox(tr("Static"));
    connect(_chkEthernet,&QCheckBox::released,this,&BMS_SystemConfigWidget::checkBoxChanged);
    EthernetConfig *lancfg = GSettings::instance().lanConfig(0);

    _grpEthernet = new QGroupBox();
    connect(_chkEthernet,&QCheckBox::clicked,_grpEthernet,&QGroupBox::setEnabled);
    if(lancfg != nullptr){
        _chkEthernet->setChecked(!lancfg->UseDHCP);
//        _grpEthernet->setEnabled(!lancfg->UseDHCP);
    }
    else{
        _chkEthernet->setChecked(false);
//        _grpEthernet->setEnabled(false);
    }
//    hlayout->addWidget(_chkEthernet);
//    hlayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum));
//    layout->addItem(hlayout);
    //layout->addWidget(_chkEthernet);
    FocusedEditor *editor;
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(5);

    glayout->addWidget(new QLabel("IP:"),1,0);
    for(int i=0;i<4;i++){
        editor = new FocusedEditor();
        connect(editor,&FocusedEditor::edited,this,&BMS_SystemConfigWidget::saveLanSetting);
        if(lancfg != nullptr){
            editor->setText(QString::number(lancfg->ip[i]));
        }
        _ip.append(editor);
        glayout->addWidget(editor,1,i+1);
    }
    glayout->addWidget(new QLabel("Gateway:"),2,0);
    for(int i=0;i<4;i++){
        editor = new FocusedEditor();
        connect(editor,&FocusedEditor::edited,this,&BMS_SystemConfigWidget::saveLanSetting);
        if(lancfg != nullptr){
            editor->setText(QString::number(lancfg->gw[i]));
        }
        _gw.append(editor);
        glayout->addWidget(editor,2,i+1);
    }

    glayout->addWidget(new QLabel("Net Mask:"),3,0);
    for(int i=0;i<4;i++){
        editor = new FocusedEditor();
        connect(editor,&FocusedEditor::edited,this,&BMS_SystemConfigWidget::saveLanSetting);
        if(lancfg != nullptr){
            editor->setText(QString::number(lancfg->mask[i]));
        }
        _mask.append(editor);
        glayout->addWidget(editor,3,i+1);
    }
    hlayout = new QHBoxLayout();
    hlayout->addItem(glayout);
    hlayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Fixed));


    _grpEthernet->setLayout(hlayout);

    gb = new QGroupBox(tr("Lan Settings"));
    QVBoxLayout *vl = new QVBoxLayout();
    vl->addWidget(_chkEthernet);
    vl->addWidget(_grpEthernet);
    gb->setLayout(vl);

    layout->addWidget(gb);



    _sdLabel = new QLabel("SD Card Status:");
    gb = new QGroupBox("SD Card");
    vl = new QVBoxLayout();
    vl->addWidget(_sdLabel);
    gb->setLayout(vl);
    layout->addWidget(gb);


    BCUSection *bcu = GSettings::instance().bcuSection();
    _chkModbusTcp = new QCheckBox("MODBUS TCP");
    _chkModbusRtu = new QCheckBox("MODBUS RTU");
    _cboRtuPort = new QComboBox();
    for(int i=0;i<_cboPort->count();i++){
        _cboRtuPort->addItem(_cboPort->itemText(i));
    }

    _rtuSlaveId = new FocusedEditor();
    _tcpPort = new FocusedEditor();
    connect(_chkModbusTcp,&QCheckBox::toggled,_tcpPort,&FocusedEditor::setEnabled);
    connect(_chkModbusRtu,&QCheckBox::toggled,_rtuSlaveId,&FocusedEditor::setEnabled);
    connect(_chkModbusRtu,&QCheckBox::toggled,_cboRtuPort,&QComboBox::setEnabled);

    connect(_chkModbusTcp,&QCheckBox::released,this,&BMS_SystemConfigWidget::checkBoxChanged);
    connect(_chkModbusRtu,&QCheckBox::released,this,&BMS_SystemConfigWidget::checkBoxChanged);
    //connect(_cboRtuPort,&QComboBox::currentIndexChanged,this,[=](int index){modbusSettingChanged();});
    connect(_rtuSlaveId,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::checkBoxChanged);
    connect(_tcpPort,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::checkBoxChanged);

    _chkModbusTcp->setChecked(bcu->mb_tcp_enabled());
    _chkModbusRtu->setChecked(bcu->mb_rtu_enabled());
    QStringList sl = bcu->mb_rtu_connection().split(",");
    if(sl.size() > 0){
        int index = _cboRtuPort->findText(sl[0]);
        if(index != -1){
            _cboRtuPort->setCurrentIndex(index);
        }
    }
    _rtuSlaveId->setText(QString::number(bcu->mb_rtu_id()));
    _tcpPort->setText(QString::number(bcu->mb_tcp_port()));

    QGridLayout *fl = new QGridLayout();
    //gl->setContentsMargins(5,5,5,5);
    //gl->addWidget(new QLabel("MODBUS Function:"));
    fl->addWidget(_chkModbusTcp,0,0);
    fl->addWidget(new QLabel("Port#"),0,1);
    fl->addWidget(_tcpPort,0,2);
    fl->addWidget(_chkModbusRtu,1,0);
    fl->addWidget(new QLabel("Port#"),1,1);
    fl->addWidget(_cboRtuPort,1,2);
    fl->addWidget(new QLabel("ID#"),1,3);
    fl->addWidget(_rtuSlaveId,1,4);
//    fl->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum));

    gb = new QGroupBox("Modbus Config");
    gb->setLayout(fl);

    layout->addWidget(gb);

    gb = new QGroupBox("NTC Inputs");
    fl = new QGridLayout();
    fl->addWidget(new QLabel("CH#1"),0,0);
    fl->addWidget(new QLabel("CH#2"),0,2);
    l = new QLabel("0.0");
    fl->addWidget(l,0,1);
    _ntcLabels.append(l);
    l = new QLabel("0.0");
    fl->addWidget(l,0,3);
    //fl->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Fixed),0,4);
    _ntcLabels.append(l);
    gb->setLayout(fl);
    layout->addWidget(gb);

    gb = new QGroupBox("Digital I/Os");
    fl = new QGridLayout();
    fl->addWidget(new QLabel("In #1"),0,0);
    QLedIndicator *li = new QLedIndicator();
    _digitalIns.append(li);
    fl->addWidget(li,0,1);
    fl->addWidget(new QLabel("In #2"),0,2);
    li = new QLedIndicator();
    _digitalIns.append(li);
    fl->addWidget(li,0,3);

    fl->addWidget(new QLabel("Out #1"),0,4);
    li = new QLedIndicator();
    li->setProperty("ID",0);
    connect(li,&QLedIndicator::clicked,this,&BMS_SystemConfigWidget::handleDigitalOutput);
    _digitalOuts.append(li);
    fl->addWidget(li,0,5);

    fl->addWidget(new QLabel("Out #2"),0,6);
    li = new QLedIndicator();
    li->setProperty("ID",1);
    connect(li,&QLedIndicator::clicked,this,&BMS_SystemConfigWidget::handleDigitalOutput);
    _digitalOuts.append(li);
    fl->addWidget(li,0,7);
    //fl->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Fixed),0,8);
    gb->setLayout(fl);
    layout->addWidget(gb);

    layout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding));

    QWidget *w = new QWidget();
    w->setLayout(layout);

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setWidget(w);

    QHBoxLayout *hh = new QHBoxLayout();
    hh->addWidget(scroll);
    widget->setLayout(hh);

    return widget;
}

QWidget *BMS_SystemConfigWidget::createBCUWidget()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(5,5,5,5);
    layout->setSpacing(20);
    QLabel *l = new QLabel("BCU Config");
    l->setAlignment(Qt::AlignCenter);
    QFont f = l->font();
    f.setBold(true);
    l->setFont(f);
    layout->addWidget(l);


    BCUSection *bcu = GSettings::instance().bcuSection();


    _grpBalancing = new QGroupBox("均衡設定");
    _balancingVoltage = new FocusedEditor();
    _balancingTime = new FocusedEditor();
    _balancingHystersis = new FocusedEditor();
    _balancingVoltageMin = new FocusedEditor();
    _balancingVoltageMax = new FocusedEditor();
    _balancingVoltageFaultDiff = new FocusedEditor();

    _balancingVoltage->setText(QString::number(bcu->balancing_voltage()));
    _balancingTime->setText(QString::number(bcu->balancing_time()));
    _balancingHystersis->setText(QString::number(bcu->balancing_gap()));
    _balancingVoltageMin->setText(QString::number(bcu->balancing_min()));
    _balancingVoltageMax->setText(QString::number(bcu->balancing_max()));
    _balancingVoltageFaultDiff->setText(QString::number(bcu->balancing_fault()));

    connect(_balancingVoltage,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::balanceSettingChanged);
    connect(_balancingTime,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::balanceSettingChanged);
    connect(_balancingHystersis,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::balanceSettingChanged);
    connect(_balancingVoltageMin,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::balanceSettingChanged);
    connect(_balancingVoltageMax,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::balanceSettingChanged);
    connect(_balancingVoltageFaultDiff,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::balanceSettingChanged);

    QGridLayout *gl = new QGridLayout();
    //gl->setContentsMargins(5,5,5,5);
    gl->setSpacing(20);
    gl->addWidget(new QLabel(tr("最低均衡電壓(V)")),0,0);
    gl->addWidget(_balancingVoltageMin,0,1);
    gl->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Fixed),0,2);
    gl->addWidget(new QLabel(tr("最高均衡電壓(V)")),1,0);
    gl->addWidget(_balancingVoltageMax,1,1);
    gl->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Fixed),1,2);
    gl->addWidget(new QLabel(tr("停止均衡壓差(V)")),2,0);
    gl->addWidget(_balancingVoltageFaultDiff,2,1);
    gl->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Fixed),2,2);

//    hlayout->addWidget(new QLabel(tr("均衡時間(秒)")));
//    hlayout->addWidget(_balancingTime);
    gl->addWidget(new QLabel(tr("允許誤差(V)")),3,0);
    gl->addWidget(_balancingHystersis,3,1);
    gl->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Fixed),3,2);

    _grpBalancing->setLayout(gl);

    layout->addWidget(_grpBalancing);


    layout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding));
    widget->setLayout(layout);
    return widget;

}

QWidget *BMS_SystemConfigWidget::createEventViewWidget()
{
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();

//    BMS_EventViewer *ev = new BMS_EventViewer();
    _evtModel = new BMS_EventModel();
    _evtModel->setEventFile("./events/event.log");
    QTableView *tview = new QTableView();
    tview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tview->setSelectionBehavior(QAbstractItemView::SelectRows);
    tview->setModel(_evtModel);

    QHBoxLayout *hlayout = new QHBoxLayout();
    QPushButton *btn = new QPushButton("Previous");
    connect(btn,&QPushButton::clicked,_evtModel,&BMS_EventModel::prevPage);
    hlayout->addWidget(btn);
    btn = new QPushButton("Next");
    connect(btn,&QPushButton::clicked,_evtModel,&BMS_EventModel::nextPage);
    hlayout->addWidget(btn);

    btn = new QPushButton("清除事件");
    connect(btn,&QPushButton::clicked,_evtModel,&BMS_EventModel::clearData);
    hlayout->addWidget(btn);

    layout->addLayout(hlayout);
    layout->addWidget(tview);

    widget->setLayout(layout);

    return widget;
}

QWidget *BMS_SystemConfigWidget::createAlarmWidget()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(5,5,5,5);
    layout->setSpacing(20);
    QLabel *l = new QLabel("Alarm Config");
    l->setAlignment(Qt::AlignCenter);
    QFont f = l->font();
    f.setBold(true);
    l->setFont(f);
    layout->addWidget(l);


    /*****************/
    // alarm
    QGridLayout *glayout = new QGridLayout();
    glayout->setContentsMargins(5,5,5,5);
    glayout->setSpacing(20);
    _grpAlarm = new QGroupBox("警報設定");
    _cboAlarmType = new QComboBox();
    _cboAlarmType->setMinimumWidth(250);
    connect(_cboAlarmType,qOverload<int>(&QComboBox::currentIndexChanged),this,&BMS_SystemConfigWidget::changeAlarmItem);
    glayout->addWidget(_cboAlarmType,0,0);
    glayout->addWidget(new QLabel("作動"),0,1);
    glayout->addWidget(new QLabel("復歸"),0,3);

    for(int i=0;i<5;i++){
        FocusedEditor *editor = new FocusedEditor();
        _alarmEdits.append(editor);
        QLabel *label = new QLabel("");
        _alarmLabels.append(label);
        connect(editor,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::alarmSettingChanged);
    }
    int sz = GSettings::instance().criteriaSize();
    for(int i=0;i<sz;i++){
        Criteria *cfg = GSettings::instance().criteria(i);
        _cboAlarmType->addItem(cfg->name());
    }

    glayout->addWidget(_alarmLabels[0],1,0);
    glayout->addWidget(_alarmEdits[0],1,1);
    glayout->addWidget(_alarmLabels[1],1,2);
    glayout->addWidget(_alarmEdits[1],1,3);

    glayout->addWidget(_alarmLabels[2],2,0);
    glayout->addWidget(_alarmEdits[2],2,1);
    glayout->addWidget(_alarmLabels[3],2,2);
    glayout->addWidget(_alarmEdits[3],2,3);

    glayout->addWidget(_alarmLabels[4],3,0);
    glayout->addWidget(_alarmEdits[4],3,1);
    _grpAlarm->setLayout(glayout);

    layout->addWidget(_grpAlarm);

    BCUSection *bcu = GSettings::instance().bcuSection();

    _grpRecords = new QGroupBox(tr("系統記錄"));
    _storedDays = new FocusedEditor();
    _eventRecordInterval = new FocusedEditor();
    _eventRecordCount = new FocusedEditor();
    connect(_storedDays,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::recordSettingChanged);
    connect(_eventRecordInterval,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::recordSettingChanged);
    _eventRecordInterval->setText(QString::number(bcu->log_interval()));
    //_eventRecordCount->setText(bcu->log_keep_days());
    _storedDays->setText(QString::number(bcu->log_keep_days()));

    QHBoxLayout *hlayout = new QHBoxLayout();

    hlayout->setContentsMargins(5,5,5,5);
    hlayout->setSpacing(20);
    hlayout->addWidget(new QLabel(tr("儲存天數")));
    hlayout->addWidget(_storedDays);
    hlayout->addWidget(new QLabel(tr("記錄間隔(秒)")));
    hlayout->addWidget(_eventRecordInterval);
    hlayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Fixed));
    _grpRecords->setLayout(hlayout);
    layout->addWidget(_grpRecords);
    /*******************/

    layout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding));
    widget->setLayout(layout);
    return widget;
}

void BMS_SystemConfigWidget::saveSerialPortSetting()
{
    SerialPortConfig *cfg = GSettings::instance().serialConfig(_cboPort->currentIndex());
    if(cfg != nullptr){
        cfg->baudrate = _cboBaudrate->currentText().toInt();
        GSettings::instance().setModified();
    }

}

void BMS_SystemConfigWidget::saveLanSetting()
{

}

void BMS_SystemConfigWidget::saveCanSetting()
{

}

void BMS_SystemConfigWidget::serialPortConfigChanged(int index)
{
    //qDebug()<<Q_FUNC_INFO;
    QComboBox *cb = static_cast<QComboBox*> (sender());
    if(cb == _cboPort){
        SerialPortConfig *cfg = GSettings::instance().serialConfig(index);
        if(cfg != nullptr){
            int index = _cboBaudrate->findText(QString::number(cfg->baudrate));
            if(index != -1){
                _cboBaudrate->setCurrentIndex(index);
            }
        }
    }
    else if(cb == _cboBaudrate){
        SerialPortConfig *cfg = GSettings::instance().serialConfig(_cboPort->currentIndex());
        if(cfg != nullptr){
            cfg->baudrate = _cboBaudrate->currentText().toInt();
            GSettings::instance().setModified();
        }
    }
}

void BMS_SystemConfigWidget::changeAlarmItem(int index)
{
    Criteria *cfg = GSettings::instance().criteria(index);
    SetResetPair *sr;
    if(cfg == nullptr) return;

//    QString path = QCoreApplication::applicationDirPath();
//    path  += "//config.ini";
//    QString keySearch;

//    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
//    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

//    int sz = progSetting->beginReadArray("ALARM_SETTINGS");
//    if(index < sz){
//        progSetting->setArrayIndex(index);
        QStringList sl = cfg->label().split(",");
        for(int i=0;i<sl.count();i++){
            if(_alarmLabels[i] != nullptr){
                if((i%2) == 0){
                    _alarmLabels[i]->setText(sl[i]);
                }
            }
        }
        sr = cfg->high();
        _alarmEdits[0]->setText(QString::number(sr->set()));
        _alarmEdits[1]->setText(QString::number(sr->reset()));
        sr = cfg->low();
        _alarmEdits[2]->setText(QString::number(sr->set()));
        _alarmEdits[3]->setText(QString::number(sr->reset()));
        _alarmEdits[4]->setText(QString::number(sr->duration()));
//    }

//    progSetting->endArray();
}

void BMS_SystemConfigWidget::alarmSettingChanged()
{
    int id = _cboAlarmType->currentIndex();
    Criteria *cfg = GSettings::instance().criteria(id);
    if(cfg == nullptr) return;
    SetResetPair *sr = cfg->high();
    cfg->setHigh(_alarmEdits[0]->text().toDouble(),_alarmEdits[1]->text().toDouble(),sr->type(),_alarmEdits[4]->text().toInt());
    sr = cfg->low();
    cfg->setLow(_alarmEdits[2]->text().toDouble(),_alarmEdits[3]->text().toDouble(),sr->type(),_alarmEdits[4]->text().toInt());
    //cfg->setTime(_alarmEdits[4]->text().toInt());
    GSettings::instance().setModified();
//    QString path = QCoreApplication::applicationDirPath();
//    path  += "//config.ini";

//    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
//    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

//    int sz = progSetting->beginReadArray("ALARM_SETTINGS");
//    if(id < sz){
//        progSetting->setArrayIndex(id);
//        QStringList sl ;
//        sl.clear();
//        for(int i=0;i<2;i++){
//            sl<<_alarmEdits[i]->text();
//        }
//        progSetting->setValue("HIGH",sl.join(","));

//        sl.clear();
//        for(int i=0;i<2;i++){
//            sl<<_alarmEdits[i+2]->text();
//        }
//        progSetting->setValue("LOW",sl.join(","));

//        progSetting->setValue("TIME",_alarmEdits[4]->text());
//    }

//    progSetting->endArray();

}

void BMS_SystemConfigWidget::recordSettingChanged()
{
//    QString path = QCoreApplication::applicationDirPath();
//    path  += "//config.ini";

//    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
//    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

//    progSetting->beginGroup("LOG");
//    progSetting->setValue("VALUE",_storedDays->text());
//    progSetting->setValue("INTERVAL",_eventRecordInterval->text());

//    progSetting->endGroup();
    BCUSection *bcu = GSettings::instance().bcuSection();
    bcu->set_log_keep_days(_storedDays->text().toInt());
    bcu->set_log_interval(_eventRecordInterval->text().toInt());
    GSettings::instance().setModified();
}

void BMS_SystemConfigWidget::balanceSettingChanged()
{
//    QString path = QCoreApplication::applicationDirPath();
//    path  += "//config.ini";

//    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
//    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

//    progSetting->beginGroup("CELL_BALANCE");
//    progSetting->setValue("VOLTAGE",_balancingVoltage->text());
//    progSetting->setValue("HYSTERSIS",_balancingHystersis->text());
//    progSetting->setValue("TIME",_balancingTime->text());
//    progSetting->endGroup();
    BCUSection *bcu = GSettings::instance().bcuSection();
    bcu->set_balancing_voltage(_balancingVoltage->text().toDouble());
    bcu->set_balancing_gap(_balancingHystersis->text().toDouble());
    bcu->set_balancing_min(_balancingVoltageMin->text().toDouble());
    bcu->set_balancing_max(_balancingVoltageMax->text().toDouble());
    bcu->set_balancing_time(_balancingTime->text().toDouble());
    bcu->set_balancing_fault(_balancingVoltageFaultDiff->text().toDouble());
    GSettings::instance().setModified();
}

void BMS_SystemConfigWidget::checkBoxChanged()
{
    QCheckBox *cb = static_cast<QCheckBox*>(sender());
    if(cb == _chkModbusRtu || cb == _chkModbusTcp){
        BCUSection *bcu = GSettings::instance().bcuSection();
        bcu->set_mb_rtu_enable(_chkModbusRtu->isChecked());
        bcu->set_mb_tcp_enable(_chkModbusTcp->isChecked());
        bcu->set_mb_rtu_connection(_cboRtuPort->currentText());
        bcu->set_mb_tcp_port(_tcpPort->text().toInt());
        bcu->set_mb_rtu_id(_rtuSlaveId->text().toInt());
    }
    else if(cb == _chkEthernet){
        EthernetConfig *cfg = GSettings::instance().lanConfig(0);
        if(cfg != nullptr){
            cfg->UseDHCP = cb->isChecked()?"YES":"NO";
        }
    }
    GSettings::instance().setModified();
}


void BMS_SystemConfigWidget::updateDigitalInputs(int id, int value)
{
    //GPIOHandler *h = static_cast<GPIOHandler*>(sender());
    //qDebug()<<Q_FUNC_INFO<<id<<value<<h->direction()<<h->index();
    int index = id & 0xf;
    bool isInput = ((id & 0x8000)==0);
    if( isInput){
        if(index < _digitalIns.size()){
            _digitalIns[index]->setChecked(value==0);
        }
    }
    else {
        if(index < _digitalOuts.size()){
            _digitalOuts[index]->setChecked(value!=0);
        }
    }
}

void BMS_SystemConfigWidget::updateDigitalOutputs(int id, int value)
{
    if(id < _digitalOuts.size()){
        _digitalOuts[id]->setChecked(value != 0);
    }
}

void BMS_SystemConfigWidget::updateNTC(int id, QString value)
{
    //qDebug()<<Q_FUNC_INFO;
    if(id < _ntcLabels.size()){
        _ntcLabels[id]->setText(value);
    }
}

void BMS_SystemConfigWidget::handleDigitalOutput()
{
    qDebug()<<Q_FUNC_INFO;
    QLedIndicator *li = static_cast<QLedIndicator*>(sender());

    int id = li->property("ID").toInt();

    emit setDigitalOutput(id,li->isChecked());
}
