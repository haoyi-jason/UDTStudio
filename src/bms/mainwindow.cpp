#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QScreen>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QTextCodec>
#include <QTabWidget>
#include "canopen/datalogger/dataloggersingleton.h"

#include <QDebug>
#include <QStringList>

#ifdef Q_OS_UNIX
#include "busdriver/canbussocketcan.h"
#endif

#include "busdriver/canbustcpudt.h"
#include "busdriver/canbusvci.h"
#include "system/login.h"
#include "system/gsettings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    resize(1280,800);
    QString path = QCoreApplication::applicationDirPath();
#ifdef Q_OS_WIN
    path  += "//config.ini";
#else
    path  += "/config.ini";
#endif

#ifdef Q_OS_UNIX
    setWindowState(Qt::WindowFullScreen);
    showFullScreen();
#endif
    GSettings::Info(QString("Load Config File from %1").arg(path));
#ifdef Q_OS_UNIX
    GSettings::instance().LoadConfig(path);
#else
    GSettings::instance().LoadConfig(path);
#endif

    setWindowTitle(tr("BMS"));
    statusBar()->setVisible(true);

    createDocks();
    createWidgets();
    readSettings();
//    _logger = new BMS_Logger();
//    _busNodesManagerView->setLogger(_logger);
//    createMenus();
   // connect(_busNodesManagerView,&BMS_BusNodesManagerView::bcuSelected,_nodeScreens,&BcuScreenWidget::bcuSelected);
    connect(_busNodesManagerView,&BMS_BusNodesManagerView::nodeSelected,_nodeScreens,&BcuScreenWidget::setActiveNode);
    connect(_busNodesManagerView,&BMS_BusNodesManagerView::bcuSelected,_nodeScreens,&BcuScreenWidget::setActiveBcu);
    connect(_busNodesManagerView,&BMS_BusNodesManagerView::nodeSelected,this,&MainWindow::setActiveNode);
    connect(_busNodesManagerView,&BMS_BusNodesManagerView::functionSelected,this,&MainWindow::setFunction);

    QString sl_conn ;
    //QList<CanOpenBus*> busList;
    _stackManager = new BMS_StackManager();
    _stackManager->setCanOpen(CanOpen::instance());
    _stackview->setStackManager(_stackManager);
    connect(_stackManager,&BMS_StackManager::activeBcuChannged,_nodeScreens,&BcuScreenWidget::setActiveBcu);
    _busNodesManagerView->setStackManager(_stackManager);

    CanOpenBus *b = nullptr;
    //    b = new CanOpenBus(new CanBusVCI("COM10"));
   //     CanOpen::addBus(b);


    //int sz = progSetting->beginReadArray("INTERFACE");
    int sz = GSettings::instance().canconfigSize();
    //qDebug()<<"Interface:"<<sz;
    if(sz > 0){
        for(int i=0;i<sz;i++){
            SerialPortConfig *cfg = GSettings::instance().canConfig(i);
//            progSetting->setArrayIndex(i);
//            QStringList sl = progSetting->value("BUS").toString().split(",");
            //qDebug()<<Q_FUNC_INFO<<sl;
            b = nullptr;
//            if(sl.size() > 2){
#ifdef Q_OS_WIN
                if(cfg->mode.contains("VCI")){
                    b = new CanOpenBus(new CanBusVCI(cfg->connection));
                    b->setBusName(cfg->mode);
                }
#endif
#ifdef Q_OS_UNIX
                if(cfg->mode.contains("SOCKETCAN")){
                    b = new CanOpenBus(new CanBusSocketCAN(cfg->connection));
                    b->setBusName(cfg->mode);
                }
#endif
                if(b != nullptr){
                    CanOpen::addBus(b);
                }
//            }
        }
//        progSetting->endArray();
    }
    _stackManager->scanBus();

//    for (const QString &edsFile : qAsConst(OdDb::edsFiles()))
//    {
//        Node *node;
//        if(edsFile.contains("BCU")){
//            node = new Node(0x1, QFileInfo(edsFile).completeBaseName(), edsFile);
//            _nodeScreens->setActiveNode(node);
//            b->addNode(node);
//            break;
//        }
//    }

//    CanOpenBus *bus = nullptr;
    //resize(QApplication::screens().at(0)->size()*3/4);


    //AlarmManager *alm = new AlarmManager(2,8,2);

//    foreach (CanOpenBus *b, CanOpen::buses()) {
//        foreach (Node *n, b->nodes()) {
//            BCU *bcu = static_cast<BCU*>(n);
//            bcu->alarmManager()->set_cell_voltage_criteria(_cvwarning,_cvalarm);
//            bcu->alarmManager()->set_cell_temperature_criteria(_ctwarning,_ctalarm);
//            bcu->alarmManager()->set_soc_criteria(_socwarning,_socalarm);
//            _logger->addBCU(bcu);
//        }
//    }
    //

    // trigger "reloadEds"
//    _busNodesManagerView->reloadEds(-1);
    //_busNodesManagerView->startBcu(-1);
    //_busNodesManagerView->startPoll(2000);
//    _logger->startLog(10);

    //Login::instance()->show();
    connect(Login::instance(),&Login::expired,this,&MainWindow::exitSuperUser);
#ifdef Q_OS_UNIX
    initSettings();
#endif
}

MainWindow::~MainWindow()
{
    CanOpen::release();
}

void MainWindow::exportCfgFile()
{
    Node *node = _busNodesManagerView->currentNode();
    if(node  == nullptr){
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save configuration file"),"",tr("Configuration file(*.conf)"));

    node->nodeOd()->exportConf(fileName);
}

void MainWindow::exportDCF()
{
    Node *node = _busNodesManagerView->currentNode();
    if(node  == nullptr){
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save DCF file"),"",tr("DCF file(*.dcf)"));

    node->nodeOd()->exportDcf(fileName);
}

void MainWindow::createDocks()
{
    setCorner(Qt::TopLeftCorner,Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);

    _busNodesManagerDock = new QDockWidget(tr("裝置資訊"),this);
    _busNodesManagerDock->setObjectName("busNodesManagerDock");
    _busNodesManagerView = new BMS_BusNodesManagerView(CanOpen::instance());
    _busNodesManagerDock->setWidget(_busNodesManagerView);
    addDockWidget(Qt::LeftDockWidgetArea,_busNodesManagerDock);

    _canFrameListDock = new QDockWidget(tr("系統資訊"),this);
    _canFrameListDock->setObjectName("canFrameListDock");
    _canFrameListView = new BMS_CanFrameListView();
    _stackview = new BMSStackView();
    _canFrameListDock->setWidget(_stackview);
    addDockWidget(Qt::LeftDockWidgetArea,_canFrameListDock);
    tabifyDockWidget(_busNodesManagerDock,_canFrameListDock);

//    _dataLoggerDock = new QDockWidget(tr("Data logger"),this);
//    _dataLoggerDock->setObjectName("dataLoggerDock");
//    _dataLoggerWidget = new DataLoggerWidget();
//    _dataLoggerWidget->setTitle("Dockable data logger");
//    _dataLoggerWidget->chartView()->setRollingTimeMs(10000);
//    _dataLoggerWidget->chartView()->setRollingEnabled(true);
//    _dataLoggerDock->setWidget(_dataLoggerWidget);
//    addDockWidget(Qt::BottomDockWidgetArea,_dataLoggerDock);

    _busNodesManagerDock->raise();
}

void MainWindow::createWidgets()
{
//    _nodeScreens = new NodeScreensWidget();
    _nodeScreens = new BcuScreenWidget();

    _configScreens = new BMS_SystemConfigWidget();
    connect(this,&MainWindow::updateGpioState,_configScreens,&BMS_SystemConfigWidget::updateDigitalInputs);
    connect(_configScreens,&BMS_SystemConfigWidget::setDigitalOutput,this,&MainWindow::handleOutputToggle);
    connect(this,&MainWindow::updateAdcValue,_configScreens,&BMS_SystemConfigWidget::updateNTC);

    _tabWidget = new QTabWidget();

    //_tabWidget->addTab(_nodeScreens,"BCU");
    //_tabWidget->addTab(_configScreens,"CONFIG");
    _tabWidget->setCurrentIndex(0);

    _stackWidget = new QStackedWidget();
    _stackWidget->addWidget(_nodeScreens);
    _stackWidget->addWidget(_configScreens);
    _stackWidget->setCurrentIndex(0);

    //setCentralWidget(_nodeScreens);
    setCentralWidget(_stackWidget);
}

void MainWindow::createMenus()
{
    QAction *action;

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    action = new QAction(tr("&Stop all"),this);
    action->setIcon(QIcon(":/icons/img/icons8-cancel.png"));
    action->setStatusTip(tr("Stop all nodes in all buses"));
    action->setShortcut(QKeySequence::Cancel);
    fileMenu->addAction(action);
    connect(action,&QAction::triggered,CanOpen::instance(),&CanOpen::stopAll);

    action = new QAction(tr("E&xit"),this);
    action->setIcon(QIcon(":/icons/img/icons8-exit.png"));
    action->setStatusTip(tr("Exit BMS"));
    action->setShortcut(QKeySequence::Quit);
    fileMenu->addAction(action);
    connect(action,&QAction::triggered,this,&MainWindow::close);

    // bus
    QMenu *busMenu = menuBar()->addMenu(tr("&Bus"));
    busMenu->addAction(_busNodesManagerView->busManagerWidget()->actionTogleConnect());
    busMenu->addAction(_busNodesManagerView->busManagerWidget()->actionExplore());
    busMenu->addAction(_busNodesManagerView->busManagerWidget()->actionSyncOne());
    busMenu->addAction(_busNodesManagerView->busManagerWidget()->actionSyncStart());

    // node
    QMenu *nodeMenu = menuBar()->addMenu(tr("&Menu"));
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionPreop());
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionStart());
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionStop());
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionReset());
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionResetCom());
    nodeMenu->addSeparator();

    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionLoadEds());
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionReLoadEds());
    nodeMenu->addSeparator();

    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionRemoveNode());

}

void MainWindow::writeSettings()
{
    QSettings settings(QApplication::organizationName(),QApplication::applicationName());
    settings.beginGroup("MainWindow");
    settings.setValue("geometry",saveGeometry());
    settings.setValue("windowState",saveState());
    settings.endGroup();

    settings.beginGroup("BusNodeManager");
    _busNodesManagerView->saveState(settings);
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings(QApplication::organizationName(),QApplication::applicationName());
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();

    settings.beginGroup("BusNodeManager");
    _busNodesManagerView->restoreState(settings);
    settings.endGroup();
}

bool MainWindow::event(QEvent *event)
{
    if(event->type() == QEvent::Close){
        writeSettings();
        QApplication::quit();
    }
//    else if(event->type() == QEvent::win){
//        emit windowIdle();
//    }
    return QMainWindow::event(event);
}

void MainWindow::about()
{

}

void MainWindow::initSettings()
{
#ifdef Q_OS_UNIX
    // on-board peripheral handler
    GPIOHandler *h = new GPIOHandler(501,INPUT);
    h->setIndex(0);
    connect(h,&GPIOHandler::gpioChanged,this,&MainWindow::updateGpioState);
    _inputHandlers.append(h);
    h = new GPIOHandler(502,INPUT);
    h->setIndex(1);
    connect(h,&GPIOHandler::gpioChanged,this,&MainWindow::updateGpioState);
    _inputHandlers.append(h);
    h = new GPIOHandler(176,OUTPUT);
    h->setIndex(0x8000);
    connect(h,&GPIOHandler::gpioChanged,this,&MainWindow::updateGpioState);
    _outputHandlers.append(h);
    h = new GPIOHandler(500,OUTPUT);
    h->setIndex(0x8001);
    connect(h,&GPIOHandler::gpioChanged,this,&MainWindow::updateGpioState);
    _outputHandlers.append(h);

    NTCHandler *a = new NTCHandler("iio:device0",0);
    connect(a,&NTCHandler::updateValue,this,&MainWindow::updateAdcValue);
    _ntcHandlers.append(a);
    a = new NTCHandler("iio:device0",1);
    connect(a,&NTCHandler::updateValue,this,&MainWindow::updateAdcValue);
    _ntcHandlers.append(a);
#endif
}

void MainWindow::setFunction(int func)
{
    if(func == 5){
        if(_logger->isRunning()){
            _logger->stopLog();
        }
        else{
            _logger->startLog(GSettings::instance().bcuSection()->log_interval());
        }
    }
    else{
        Login::instance()->resetTimer();
        if(Login::instance()->isValid()){
            _stackWidget->setCurrentIndex(1);
            _configScreens->setActivePage(func);
        }
        else
        {
            Login::instance()->setModal(true);
            Login::instance()->showFullScreen();
            if(Login::instance()->exec() == QDialog::Accepted){
                if(Login::instance()->isValid()){
                    _stackWidget->setCurrentIndex(1);
                    _configScreens->setActivePage(func);
                }
                else{

                }
            }

        }

    }
}

void MainWindow::setActiveNode(Node *node)
{
//    while(_tabWidget->count() > 0){
//        _tabWidget->removeTab(0);
//    }
//    _tabWidget->addTab(_nodeScreens,"BCU");
    _stackWidget->setCurrentIndex(0);
}

void MainWindow::exitSuperUser()
{
    setActiveNode(nullptr);
}

void MainWindow::handleOutputToggle(int id, bool state)
{
#ifdef Q_OS_UNIX
    if(id < _outputHandlers.size()){
        _outputHandlers[id]->writeValue((state == 0)?0:1);
    }
#endif
}

void MainWindow::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug()<<Q_FUNC_INFO;
    ev->accept();
    Login::instance()->resetTimer();
}

void MainWindow::handleGpioValue(int id, int value)
{
#ifdef Q_OS_UNIX
    GPIOHandler *h = static_cast<GPIOHandler*>(sender());

    if(_inputHandlers[id] == h){

    }
    else if(_outputHandlers[id] == h){

    }
#endif
}

void MainWindow::handleAdcValue(int id, int value)
{

}

void MainWindow::scanBus()
{
}

