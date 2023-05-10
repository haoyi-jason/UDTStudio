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

#include "../../hymcw/HYMCW/source/widgets/qledindicator.h"

BMS_SystemConfigWidget::BMS_SystemConfigWidget(QWidget *parent) : QStackedWidget(parent)
{
    createWidgets();

    QFont f = font();
    f.setPointSize(16);
    setFont(f);

    loadSettings();
}

void BMS_SystemConfigWidget::createWidgets()
{
//    QVBoxLayout *layout = new QVBoxLayout();

//    layout->setContentsMargins(0,0,0,0);
//    layout->setSpacing(2);

//    layout->addWidget(createHardwareWidget());


    //setLayout(layout);
    addWidget(createHardwareWidget());
    addWidget(createBCUWidget());
    addWidget(createAlarmWidget());
    addWidget(createEventViewWidget());

    //qDebug()<<Q_FUNC_INFO<<" Widgets:"<<count();
}

void BMS_SystemConfigWidget::setActivePage(int id)
{
    //qDebug()<<Q_FUNC_INFO<<" ID="<<id;
    int page = id-1;
    if(page >= 0 && page < count()){
       //qDebug()<<"Set to Page:"<<page;
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
//    connect(_cboPort,&QComboBox::currentIndexChanged,this,&BMS_SystemConfigWidget::serialPortConfigChanged);
//    connect(_cboBaudrate,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&BMS_SystemConfigWidget::serialPortConfigChanged);
//    connect(_cboBaudrate,qOverload<int>(&QComboBox::currentIndexChanged),this,[=](int index){serialPortConfigChanged(index);});
    QObject::connect(_cboBaudrate,qOverload<int>(&QComboBox::currentIndexChanged),this,&BMS_SystemConfigWidget::serialPortConfigChanged);
    QPushButton *btn;
    _cboPort->addItem("COM1","/dev/ttymxc0");
    _cboPort->addItem("COM2","/dev/ttymxc1");
    _cboPort->addItem("COM3","/dev/ttymxc3");
    _cboPort->addItem("COM4","/dev/ttymxc4");
    _cboPort->setMinimumWidth(200);
    hlayout->addWidget(new QLabel("Port #"));
    hlayout->addWidget(_cboPort);

    _cboBaudrate = new QComboBox();
    _cboBaudrate->addItem("9600");
    _cboBaudrate->addItem("19200");
    _cboBaudrate->addItem("38400");
    _cboBaudrate->addItem("57600");
    _cboBaudrate->addItem("115200");
    _cboBaudrate->setMinimumWidth(200);
    hlayout->addWidget(new QLabel("Baudrate "));
    hlayout->addWidget(_cboBaudrate);
    btn = new QPushButton(tr("Set"));
    btn->setMinimumWidth(160);
    connect(btn,&QPushButton::clicked,this,&BMS_SystemConfigWidget::saveSerialPortSetting);
    hlayout->addWidget(btn);
    hlayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Fixed));
    QGroupBox *gb = new QGroupBox("Serial Ports");
    gb->setLayout(hlayout);
    layout->addWidget(gb);

    // ethernet config
    QGridLayout *glayout = new QGridLayout();
    glayout->setContentsMargins(2,2,2,2);
//    hlayout = new QHBoxLayout();
//    hlayout->setContentsMargins(0,0,0,0);
    _chkEthernet = new QCheckBox(tr("Static IP"));
    _chkEthernet->setChecked(false);
    _grpEthernet = new QGroupBox("IP Config");
    connect(_chkEthernet,&QCheckBox::clicked,_grpEthernet,&QGroupBox::setEnabled);
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
        _ip.append(editor);
        glayout->addWidget(editor,1,i+1);
    }
    glayout->addWidget(new QLabel("Gateway:"),2,0);
    for(int i=0;i<4;i++){
        editor = new FocusedEditor();
        _gw.append(editor);
        glayout->addWidget(editor,2,i+1);
    }

    glayout->addWidget(new QLabel("Net Mask:"),3,0);
    for(int i=0;i<4;i++){
        editor = new FocusedEditor();
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

    connect(_chkModbusTcp,&QCheckBox::released,this,&BMS_SystemConfigWidget::modbusSettingChanged);
    connect(_chkModbusRtu,&QCheckBox::released,this,&BMS_SystemConfigWidget::modbusSettingChanged);
    //connect(_cboRtuPort,&QComboBox::currentIndexChanged,this,[=](int index){modbusSettingChanged();});
    connect(_rtuSlaveId,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::modbusSettingChanged);
    connect(_tcpPort,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::modbusSettingChanged);

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

    fl->addWidget(new QLabel("Out #1"),1,0);
    li = new QLedIndicator();
    li->setProperty("ID",0);
    connect(li,&QLedIndicator::clicked,this,&BMS_SystemConfigWidget::handleDigitalOutput);
    _digitalOuts.append(li);
    fl->addWidget(li,1,1);

    fl->addWidget(new QLabel("Out #2"),1,2);
    li = new QLedIndicator();
    li->setProperty("ID",1);
    connect(li,&QLedIndicator::clicked,this,&BMS_SystemConfigWidget::handleDigitalOutput);
    _digitalOuts.append(li);
    fl->addWidget(li,1,3);
    gb->setLayout(fl);
    layout->addWidget(gb);

    layout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding));
    widget->setLayout(layout);
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

    _grpRecords = new QGroupBox(tr("系統記錄"));
    _storedDays = new FocusedEditor();
    _eventRecordInterval = new FocusedEditor();
    _eventRecordCount = new FocusedEditor();
    connect(_storedDays,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::recordSettingChanged);
    connect(_eventRecordInterval,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::recordSettingChanged);
    QHBoxLayout *hlayout = new QHBoxLayout();

    hlayout->setContentsMargins(5,5,5,5);
    hlayout->setSpacing(20);
    hlayout->addWidget(new QLabel(tr("儲存天數")));
    hlayout->addWidget(_storedDays);
    hlayout->addWidget(new QLabel(tr("記錄間隔(秒)")));
    hlayout->addWidget(_eventRecordInterval);
    _grpRecords->setLayout(hlayout);
    layout->addWidget(_grpRecords);

    _grpBalancing = new QGroupBox("均衡設定");
    _balancingVoltage = new FocusedEditor();
    _balancingTime = new FocusedEditor();
    _balancingHystersis = new FocusedEditor();
    _balancingVoltageMin = new FocusedEditor();
    _balancingVoltageMax = new FocusedEditor();
    _balancingVoltageFaultDiff = new FocusedEditor();

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
    gl->addWidget(new QLabel(tr("最高均衡電壓(V)")),1,0);
    gl->addWidget(_balancingVoltageMax,1,1);
    gl->addWidget(new QLabel(tr("停止均衡壓差(V)")),2,0);
    gl->addWidget(_balancingVoltageFaultDiff,2,1);

//    hlayout->addWidget(new QLabel(tr("均衡時間(秒)")));
//    hlayout->addWidget(_balancingTime);
    gl->addWidget(new QLabel(tr("允許誤差(V)")),3,0);
    gl->addWidget(_balancingHystersis,3,1);

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
    glayout->addWidget(new QLabel("復歸"),0,2);

    for(int i=0;i<5;i++){
        FocusedEditor *editor = new FocusedEditor();
        _alarmEdits.append(editor);
        QLabel *label = new QLabel("label");
        _alarmLabels.append(label);
        connect(editor,&FocusedEditor::editingFinished,this,&BMS_SystemConfigWidget::alarmSettingChanged);
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


    /*******************/

    layout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding));
    widget->setLayout(layout);
    return widget;
}

void BMS_SystemConfigWidget::saveSerialPortSetting()
{
    serialPortConfigChanged(0);

}

void BMS_SystemConfigWidget::saveLanSetting()
{

}

void BMS_SystemConfigWidget::saveCanSetting()
{

}

void BMS_SystemConfigWidget::serialPortConfigChanged(int index)
{
    qDebug()<<Q_FUNC_INFO;
    saveSettings("SERIAL",_cboPort->currentText(),_cboBaudrate->currentText());
}

void BMS_SystemConfigWidget::loadSettings()
{
    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";
    QString keySearch;

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    QStringList keys = progSetting->allKeys();

    /* 0/PORT=COMx,9600,n,8,1,n */
    _cboPort->clear();
    //if(progSetting->contains("SERIAL")){
    int sz = progSetting->beginReadArray("SERIAL");
    if(sz >0){
        for(int i=0;i<sz;i++){
            progSetting->setArrayIndex(i);
            QStringList sl = progSetting->value("PORT").toString().split(",");
            _cboPort->addItem(sl[0]);
        }
        progSetting->endArray();
    }
    else{
        progSetting->beginWriteArray("SERIAL");
        for(int i=0;i<4;i++){
            QString arg = QString("COM%1,9600,n,8,1,n").arg(i+1);
            progSetting->setArrayIndex(i);
            progSetting->setValue("PORT",arg);
            _cboPort->addItem(QString("COM%1").arg(i+1));
        }
        progSetting->endArray();
        progSetting->sync();
    }

    progSetting->beginGroup("ETHERNET");
    QString ip = "192.168.1.240";
    QString gw = "192.168.1.1";
    QString mask = "255.255.255.0";
    if(progSetting->contains("DHCP")){
        if(progSetting->value("DHCP") == "YES"){
            _chkEthernet->setChecked(true);
        }
        else{
            _chkEthernet->setChecked(false);
        }
    }
    else{
        progSetting->setValue("DHCP","NO");
        _chkEthernet->setChecked(false);
    }
    QStringList sl;
    if(progSetting->contains("IP")){
        sl = progSetting->value("IP").toString().split(".");
        if(sl.size() != 4){
            progSetting->setValue("IP",ip);
            sl = ip.split(".");
        }
    }
    else{
        progSetting->setValue("IP",ip);
        sl = ip.split(".");
    }
    for(int i=0;i<4;i++){
        _ip[i]->setText(sl[i]);
    }

    if(progSetting->contains("GATEWAY")){
        sl = progSetting->value("GATEWAY").toString().split(".");
        if(sl.size() != 4){
            progSetting->setValue("GATEWAY",gw);
            sl = gw.split(".");
        }
    }
    else{
        progSetting->setValue("GATEWAY",gw);
        sl = gw.split(".");
    }
    for(int i=0;i<4;i++){
        _gw[i]->setText(sl[i]);
    }

    if(progSetting->contains("MASK")){
        sl = progSetting->value("MASK").toString().split(".");
        if(sl.size() != 4){
            progSetting->setValue("MASK",mask);
            sl = mask.split(".");
        }
    }
    else{
        progSetting->setValue("MASK",mask);
        sl = mask.split(".");
    }
    for(int i=0;i<4;i++){
        _mask[i]->setText(sl[i]);
    }
    progSetting->endGroup();
    progSetting->sync();


    progSetting->beginGroup("MODBUS");
    if(progSetting->contains("TCP_ENABLE")){
        if(progSetting->value("TCP_ENABLE").toString().contains("TRUE")){
            _chkModbusTcp->setChecked(true);
        }
        else{
            _chkModbusTcp->setChecked(false);
        }
    }
    else{
        progSetting->setValue("TCP_ENABLE","FALSE");
        _chkModbusTcp->setChecked(false);
    }
    if(progSetting->contains("RTU_ENABLE")){
        if(progSetting->value("RTU_ENABLE").toString().contains("TRUE")){
            _chkModbusRtu->setChecked(true);
        }
        else{
            _chkModbusRtu->setChecked(false);
        }
    }
    else{
        progSetting->setValue("RTU_ENABLE","FALSE");
        _chkModbusRtu->setChecked(false);
    }

    if(progSetting->contains("RTU_PORT")){
        for(int i=0;i<_cboRtuPort->count();i++){
            if(_cboRtuPort->itemText(i) == progSetting->value("RTU_PORT").toString()){
                _cboRtuPort->setCurrentIndex(i);
                break;
            }
        }
    }
    else{
        _cboRtuPort->setCurrentIndex(0);
        progSetting->setValue("RTU_PORT",_cboRtuPort->currentText());
    }

    if(progSetting->contains("ID")){
        _rtuSlaveId->setText(progSetting->value("ID").toString());
    }
    else{
        _rtuSlaveId->setText("1");
        progSetting->setValue("ID","1");
    }

    if(progSetting->contains("TCP_PORT")){
        _tcpPort->setText(progSetting->value("TCP_PORT").toString());
    }
    else{
        _tcpPort->setText("502");
        progSetting->setValue("TCP_PORT","502");
    }

    progSetting->endGroup();


    // log
    progSetting->beginGroup("LOG");

//    if(progSetting->contains("TYPE")){
//        //if(progSetting->value("TYPE").toString() == "DAY")
//    }
//    else{
//    }

    if(progSetting->contains("VALUE")){
        _storedDays->setText(progSetting->value("VALUE").toString());
    }
    else{
        _storedDays->setText("180");
        progSetting->setValue("VALUE","180");
    }

    if(progSetting->contains("INTERVAL")){
        _eventRecordInterval->setText(progSetting->value("INTERVAL").toString());
    }
    else{
        _eventRecordInterval->setText("60");
        progSetting->setValue("INTERVAL","60");
    }

    progSetting->endGroup();

    // cell balance
    progSetting->beginGroup("CELL_BALANCE");
    if(progSetting->contains("VOLTAGE")){
        _balancingVoltage->setText(progSetting->value("VOLTAGE").toString());
    }
    else{
        _balancingVoltage->setText("5.0");
        progSetting->setValue("VOLTAGE","5.0");
    }

    if(progSetting->contains("HYSTERSIS")){
        _balancingHystersis->setText(progSetting->value("HYSTERSIS").toString());
    }
    else{
        _balancingHystersis->setText("8");
        progSetting->setValue("HYSTERSIS","8");
    }

    if(progSetting->contains("MINIMUM")){
        _balancingVoltageMin->setText(progSetting->value("MINIMUM").toString());
    }
    else{
        _balancingVoltageMin->setText("3.4");
        progSetting->setValue("MINIMUM","3.4");
    }

    if(progSetting->contains("MAXIMUM")){
        _balancingVoltageMax->setText(progSetting->value("MAXIMUM").toString());
    }
    else{
        _balancingVoltageMax->setText("4.2");
        progSetting->setValue("MAXIMUM","4.2");
    }

    if(progSetting->contains("FAULT_DIFF")){
        _balancingVoltageFaultDiff->setText(progSetting->value("FAULT_DIFF").toString());
    }
    else{
        _balancingVoltageFaultDiff->setText("0.5");
        progSetting->setValue("FAULT_DIFF","0.5");
    }

    progSetting->endGroup();

    // alarm settings
    sz = progSetting->beginReadArray("ALARM_SETTINGS");
    if(sz > 0){
        _cboAlarmType->clear();
        for(int i=0;i<sz;i++){
            progSetting->setArrayIndex(i);
            if(progSetting->contains("NAME")){
                _cboAlarmType->addItem(progSetting->value("NAME").toString());
            }
        }
    }
    progSetting->endArray();

}

void BMS_SystemConfigWidget::changeAlarmItem(int index)
{
    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";
    QString keySearch;

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    int sz = progSetting->beginReadArray("ALARM_SETTINGS");
    if(index < sz){
        progSetting->setArrayIndex(index);
        QStringList sl = progSetting->value("LABEL").toString().split(",");
        for(int i=0;i<sl.count();i++){
            if(_alarmLabels[i] != nullptr){
                _alarmLabels[i]->setText(sl[i]);
            }
        }

        sl = progSetting->value("HIGH").toString().split(",");
        if(sl.count() == 2){
            _alarmEdits[0]->setText(sl[0]);
            _alarmEdits[1]->setText(sl[1]);
        }
        sl = progSetting->value("LOW").toString().split(",");
        if(sl.count() == 2){
            _alarmEdits[2]->setText(sl[0]);
            _alarmEdits[3]->setText(sl[1]);
        }
        _alarmEdits[4]->setText(progSetting->value("TIME").toString());
    }

    progSetting->endArray();
}

void BMS_SystemConfigWidget::alarmSettingChanged()
{
    int id = _cboAlarmType->currentIndex();
    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    int sz = progSetting->beginReadArray("ALARM_SETTINGS");
    if(id < sz){
        progSetting->setArrayIndex(id);
        QStringList sl ;
        sl.clear();
        for(int i=0;i<2;i++){
            sl<<_alarmEdits[i]->text();
        }
        progSetting->setValue("HIGH",sl.join(","));

        sl.clear();
        for(int i=0;i<2;i++){
            sl<<_alarmEdits[i+2]->text();
        }
        progSetting->setValue("LOW",sl.join(","));

        progSetting->setValue("TIME",_alarmEdits[4]->text());
    }

    progSetting->endArray();

}

void BMS_SystemConfigWidget::recordSettingChanged()
{
    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    progSetting->beginGroup("LOG");
    progSetting->setValue("VALUE",_storedDays->text());
    progSetting->setValue("INTERVAL",_eventRecordInterval->text());

    progSetting->endGroup();

}

void BMS_SystemConfigWidget::balanceSettingChanged()
{
    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    progSetting->beginGroup("CELL_BALANCE");
    progSetting->setValue("VOLTAGE",_balancingVoltage->text());
    progSetting->setValue("HYSTERSIS",_balancingHystersis->text());
    progSetting->setValue("TIME",_balancingTime->text());
    progSetting->endGroup();
}

void BMS_SystemConfigWidget::modbusSettingChanged()
{
    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    progSetting->beginGroup("MODBUS");

    progSetting->setValue("TCP_ENABLE",_chkModbusTcp->isChecked()?"TRUE":"FALSE");
    progSetting->setValue("RTU_ENABLE",_chkModbusRtu->isChecked()?"TRUE":"FALSE");
    progSetting->setValue("RTU_PORT",_cboRtuPort->currentText());
    progSetting->setValue("TCP_PORT",_tcpPort->text());
    progSetting->setValue("ID",_rtuSlaveId->text());

    progSetting->endGroup();
}

void BMS_SystemConfigWidget::saveSettings(QString section, QString key, QString value)
{
    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    if(section == "SERIAL"){
        int sz = progSetting->beginReadArray(section);
        for(int i=0;i<sz;i++){
            progSetting->setArrayIndex(i);
            QStringList sl = progSetting->value("PORT").toString().split(",");
            if(sl[0] == key){
                sl[1] = value;
            }
            QString v = sl.join(",");
            progSetting->setValue("PORT",v);
        }
        progSetting->endArray();

    }
}


void BMS_SystemConfigWidget::updateDigitalInputs(int id,bool state)
{
    if(id < _digitalIns.size()){
        _digitalIns[id]->setChecked(state);
    }
}

void BMS_SystemConfigWidget::updateDigitalOutputs(int id, bool state)
{
    if(id < _digitalOuts.size()){
        _digitalOuts[id]->setChecked(state);
    }
}

void BMS_SystemConfigWidget::updateNTC(int id, QString msg)
{
    if(id < _ntcLabels.size()){
        _ntcLabels[id]->setText(msg);
    }
}

void BMS_SystemConfigWidget::handleDigitalOutput()
{
    QLedIndicator *li = static_cast<QLedIndicator*>(sender());

    int id = li->property("ID").toInt();

    emit setDigitalOutput(id,li->isChecked());
}
