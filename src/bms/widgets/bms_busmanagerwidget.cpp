#include "bms_busmanagerwidget.h"
#include <QApplication>
#include <QFormLayout>
#include <QSettings>
#include <QTextCodec>

BMS_BusManagerWidget::BMS_BusManagerWidget(QWidget *parent)
  :BMS_BusManagerWidget(nullptr, parent)
{

}

BMS_BusManagerWidget::BMS_BusManagerWidget(CanOpenBus *bus, QWidget *parent)
    : QWidget(parent)
    , _bus(bus)
{
    createWidgets();
    setBus(bus);
}

CanOpenBus *BMS_BusManagerWidget::bus() const
{
    return _bus;
}

void BMS_BusManagerWidget::setBus(CanOpenBus *bus)
{
    if(_bus != nullptr){
        disconnect(_bus,nullptr,this,nullptr);
    }

    _bus = bus;

    if(_bus != nullptr){
        connect(_bus, &CanOpenBus::connectedChanged,this,&BMS_BusManagerWidget::updateBusData);
        connect(_bus,&CanOpenBus::busNameChanged,this,&BMS_BusManagerWidget::updateBusData);
    }

    _groupBox->setEnabled(_bus != nullptr);
    updateBusData();
}

void BMS_BusManagerWidget::updateBusData()
{
    if(_bus != nullptr){
        _actionTogleConnect->blockSignals(true);
        _actionTogleConnect->setEnabled(true);
        _actionTogleConnect->setCheckable(_bus->isConnected());
        _actionTogleConnect->setText(_bus->isConnected()?tr("Disconnect"):tr("Connect"));
        _actionTogleConnect->blockSignals(false);

        _actionSyncStart->blockSignals(true);
        actionSyncStart()->setChecked(_bus->sync()->status() == Sync::STARTED);
        _actionSyncStart->blockSignals(false);


        _busNameEdit->setText(_bus->busName());

        _actionExplore->setEnabled(_bus->isConnected());
        _actionSyncOne->setEnabled(_bus->isConnected());
        _actionSyncStart->setEnabled(_bus->isConnected());

        _busAddressEdit->setText(_bus->driverAddress());
        _btnSetBusAddress->setEnabled(true);
    }

    if(_bus == nullptr){
        _actionTogleConnect->setChecked(false);
        _actionTogleConnect->setEnabled(false);
        _actionExplore->setEnabled(false);
        _actionSyncOne->setEnabled(false);
        _actionSyncStart->setEnabled(false);

        _btnSetBusAddress->setEnabled(false);
    }
}


void BMS_BusManagerWidget::togleConnect()
{
    if (_bus != nullptr)
    {
        if (_bus->canBusDriver() != nullptr)
        {
            if (_bus->isConnected())
            {
                _bus->canBusDriver()->disconnectDevice();
            }
            else
            {
                _bus->canBusDriver()->connectDevice();
            }
        }
    }
}

void BMS_BusManagerWidget::exploreBus()
{
    if (_bus != nullptr)
    {
        _bus->exploreBus();
    }
}

void BMS_BusManagerWidget::sendSyncOne()
{
    if (_bus != nullptr)
    {
        _bus->sync()->sendSyncOne();
    }
}

void BMS_BusManagerWidget::toggleSync(bool start)
{
    if (_bus != nullptr)
    {
        if (start)
        {
            _bus->sync()->startSync(_syncTimerSpinBox->value());
        }
        else
        {
            _bus->sync()->stopSync();
        }
    }
}

void BMS_BusManagerWidget::setSyncTimer(int i)
{
    if (_actionSyncStart->isChecked())
    {
        if (_bus != nullptr)
        {
            _bus->sync()->startSync(i);
        }
    }
}

void BMS_BusManagerWidget::setBusName()
{
    if (_bus != nullptr)
    {
        _bus->setBusName(_busNameEdit->text());
    }
}

void BMS_BusManagerWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    _groupBox = new QGroupBox(tr("Bus"));
    QFormLayout *layoutGroupBox = new QFormLayout();
    layoutGroupBox->setSpacing(2);
    layoutGroupBox->setContentsMargins(2, 2, 2, 2);

    _toolBar = new QToolBar(tr("Bus commands"));
    _toolBar->setIconSize(QSize(20, 20));

    // connect
    _actionTogleConnect = _toolBar->addAction(tr("Connect"));
    QIcon iconConnect;
    iconConnect.addFile(":/icons/img/icons8-disconnected.png", QSize(), QIcon::Normal, QIcon::Off);
    iconConnect.addFile(":/icons/img/icons8-connected.png", QSize(), QIcon::Normal, QIcon::On);
    _actionTogleConnect->setIcon(iconConnect);
    _actionTogleConnect->setCheckable(true);
    _actionTogleConnect->setStatusTip(tr("Connect/disconnect bus"));
    connect(_actionTogleConnect, &QAction::triggered, this, &BMS_BusManagerWidget::togleConnect);

    // explore
    _actionExplore = _toolBar->addAction(tr("Explore"));
    _actionExplore->setIcon(QIcon(":/icons/img/icons8-search-database.png"));
    _actionExplore->setShortcut(QKeySequence("Ctrl+E"));
    _actionExplore->setStatusTip(tr("Explore bus for new nodes"));
    connect(_actionExplore, &QAction::triggered, this, &BMS_BusManagerWidget::exploreBus);

    // Sync one
    _actionSyncOne = _toolBar->addAction(tr("Sync one"));
    _actionSyncOne->setIcon(QIcon(":/icons/img/icons8-sync1.png"));
    _actionSyncOne->setStatusTip(tr("Send one sync command"));
    connect(_actionSyncOne, &QAction::triggered, this, &BMS_BusManagerWidget::sendSyncOne);

    // Sync timer
    _syncTimerSpinBox = new QSpinBox();
    _syncTimerSpinBox->setRange(1, 5000);
    _syncTimerSpinBox->setValue(100);
    _syncTimerSpinBox->setSuffix(" ms");
    _syncTimerSpinBox->setStatusTip(tr("Sets the interval of sync timer in ms"));
    _toolBar->addWidget(_syncTimerSpinBox);
    connect(_syncTimerSpinBox,
            &QSpinBox::editingFinished,
            this,
            [=]()
            {
                setSyncTimer(_syncTimerSpinBox->value());
            });

    _actionSyncStart = _toolBar->addAction(tr("Start / stop sync"));
    _actionSyncStart->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    _actionSyncStart->setCheckable(true);
    _actionSyncStart->setStatusTip(tr("Start / stop sync timer"));
    connect(_actionSyncStart, &QAction::triggered, this, &BMS_BusManagerWidget::toggleSync);
    layoutGroupBox->addRow(_toolBar);
    layoutGroupBox->addItem(new QSpacerItem(0, 2));

    _busNameEdit = new QLineEdit();
    layoutGroupBox->addRow(tr("Name:"), _busNameEdit);
    connect(_busNameEdit, &QLineEdit::returnPressed, this, &BMS_BusManagerWidget::setBusName);

    QHBoxLayout *busNameLayout = new QHBoxLayout();
    busNameLayout->setContentsMargins(0,0,0,0);
    _busAddressEdit = new QLineEdit();
//    busNameLayout->addWidget(new QLabel("Address"));
    busNameLayout->addWidget(_busAddressEdit);
    _btnSetBusAddress = new QPushButton("Set");
    connect(_btnSetBusAddress,&QPushButton::clicked,this,&BMS_BusManagerWidget::setBusAddress);
    busNameLayout->addWidget(_btnSetBusAddress);
    layoutGroupBox->addRow(tr("Address"),busNameLayout);
    _groupBox->setLayout(layoutGroupBox);
    layout->addWidget(_groupBox);

    setLayout(layout);
}

QAction *BMS_BusManagerWidget::actionTogleConnect() const
{
    return _actionTogleConnect;
}

QAction *BMS_BusManagerWidget::actionSyncStart() const
{
    return _actionSyncStart;
}

QAction *BMS_BusManagerWidget::actionSyncOne() const
{
    return _actionSyncOne;
}

QAction *BMS_BusManagerWidget::actionExplore() const
{
    return _actionExplore;
}

void BMS_BusManagerWidget::setBusAddress()
{
    _bus->setDriverAddress(_busAddressEdit->text());
    // save to ini file
    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    progSetting->beginGroup("INTERFACE");
    progSetting->setValue(_bus->busName(),_busAddressEdit->text());
    progSetting->endGroup();

    progSetting->sync();

}
