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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(tr("BMS-GE"));
    statusBar()->setVisible(true);

    createDocks();
    createWidgets();
//    createMenus();
//    connect(_busNodesManagerView,&BMS_BusNodesManagerView::nodeSelected,_nodeScreens,&NodeScreensWidget::setActiveNode);
    connect(_busNodesManagerView,&BMS_BusNodesManagerView::nodeSelected,_nodeScreens,&BcuScreenWidget::setActiveNode);
    connect(_busNodesManagerView,&BMS_BusNodesManagerView::nodeSelected,this,&MainWindow::setActiveNode);
    connect(_busNodesManagerView,&BMS_BusNodesManagerView::functionSelected,this,&MainWindow::setFunction);

    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    QString sl_conn ;
    //QList<CanOpenBus*> busList;
    CanOpenBus *b = nullptr;
    //    b = new CanOpenBus(new CanBusVCI("COM10"));
   //     CanOpen::addBus(b);


    //if(progSetting->contains("INTERFACE")){
    int sz = progSetting->beginReadArray("INTERFACE");
    if(sz > 0){
        for(int i=0;i<sz;i++){
            progSetting->setArrayIndex(i);
            QStringList sl = progSetting->value("BUS").toString().split(",");
            if(sl.size() > 2){
                if(sl[0] == "VCI"){
                    b = new CanOpenBus(new CanBusVCI(sl[1]));
                    b->setBusName(sl[0]);
                    //busList.append(b);
                    CanOpen::addBus(b);

                }
            }
        }
        progSetting->endArray();
    }
    else{
        progSetting->beginWriteArray("INTERFACE");
        progSetting->setArrayIndex(0);
        progSetting->setValue("BUS","COM10,500000,2");
        progSetting->endArray();
        progSetting->sync();
    }



    CanOpenBus *bus = nullptr;
#ifdef Q_OS_UNIX

#endif
//    bus = new CanOpenBus(new CanBusDriver(""));
//    bus->setBusName("VBus eds");
//    CanOpen::addBus(bus);
    int id = 1;
    for(const QString &edsFile:qAsConst(OdDb::edsFiles())){
        //Node *node = new Node(id,QFileInfo(edsFile).completeBaseName(),edsFile);
        if(edsFile.contains("BCU")){
            //if(progSetting->contains("BCUS")){
            int sz = progSetting->beginReadArray("BCUS");
            if(sz > 0){
                for(int i=0;i<sz;i++){
                    progSetting->setArrayIndex(i);
                    int bid = progSetting->value("BUS_ID").toInt();
                    int nid = progSetting->value("NODE_ID").toInt();
                    BCU *node = new BCU(nid,QFileInfo(edsFile).completeBaseName(),edsFile);
                    if(bid < CanOpen::buses().count()){
                        //busList[bus_id]->addNode(node);
                        CanOpen::bus(bid)->addNode(node);
                    }
                }
                progSetting->endArray();
            }
//            for(int i=1;i<7;i++){
//                BCU *node = new BCU(i,QFileInfo(edsFile).completeBaseName(),edsFile);
//                b->addNode(node);
//            }

        }
        id++;
    }

    //resize(QApplication::screens().at(0)->size()*3/4);
    resize(1280,800);

    readSettings();

    _logger = new BMS_Logger();

    foreach (CanOpenBus *b, CanOpen::buses()) {
        foreach (Node *n, b->nodes()) {
            BCU *bcu = static_cast<BCU*>(n);
            bcu->alarmManager()->set_cell_voltage_criteria(_cvwarning,_cvalarm);
            bcu->alarmManager()->set_cell_temperature_criteria(_ctwarning,_ctalarm);
            bcu->alarmManager()->set_soc_criteria(_socwarning,_socalarm);
            _logger->addBCU(bcu);
        }
    }
    //

    // trigger "reloadEds"
    _busNodesManagerView->reloadEds(-1);
    //_busNodesManagerView->startBcu(-1);
    //_busNodesManagerView->startPoll(2000);
    _logger->startLog(10);
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

    _busNodesManagerDock = new QDockWidget(tr("Bus nodes"),this);
    _busNodesManagerDock->setObjectName("busNodesManagerDock");
    _busNodesManagerView = new BMS_BusNodesManagerView(CanOpen::instance());
    _busNodesManagerDock->setWidget(_busNodesManagerView);
    addDockWidget(Qt::LeftDockWidgetArea,_busNodesManagerDock);

    _canFrameListDock = new QDockWidget(tr("Can frames"),this);
    _canFrameListDock->setObjectName("canFrameListDock");
    _canFrameListView = new BMS_CanFrameListView();
    _canFrameListDock->setWidget(_canFrameListView);
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

    _tabWidget = new QTabWidget();

    _tabWidget->addTab(_nodeScreens,"BCU");
    _tabWidget->addTab(_configScreens,"CONFIG");
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

    // read alarm/warning settings

    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";
    QString keySearch;

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));

    int sz = progSetting->beginReadArray("ALARM_SETTINGS");
    QStringList sl;
    SetResetPair *sr;
    if(sz == 6){
        progSetting->setArrayIndex(0);
        sl = progSetting->value("HIGH").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_GT);
        _cvwarning.append(sr);
        sl = progSetting->value("LOW").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_LT);
        _cvwarning.append(sr);

        progSetting->setArrayIndex(1);
        sl = progSetting->value("HIGH").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_GT);
        _cvalarm.append(sr);
        sl = progSetting->value("LOW").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_LT);
        _cvalarm.append(sr);

        progSetting->setArrayIndex(2);
        sl = progSetting->value("HIGH").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_GT);
        _ctwarning.append(sr);
        sl = progSetting->value("LOW").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_LT);
        _ctwarning.append(sr);

        progSetting->setArrayIndex(3);
        sl = progSetting->value("HIGH").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_GT);
        _ctalarm.append(sr);
        sl = progSetting->value("LOW").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_LT);
        _ctalarm.append(sr);

        progSetting->setArrayIndex(4);
        sl = progSetting->value("HIGH").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_GT);
        _socwarning.append(sr);
        sl = progSetting->value("LOW").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_LT);
        _socwarning.append(sr);

        progSetting->setArrayIndex(5);
        sl = progSetting->value("HIGH").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_GT);
        _socalarm.append(sr);
        sl = progSetting->value("LOW").toString().split(",");
        sr = new SetResetPair(sl[0].toDouble(), sl[1].toDouble(),SetResetPair::Comparator::CMP_LT);
        _socalarm.append(sr);


    }


    progSetting->endArray();
}

bool MainWindow::event(QEvent *event)
{
    if(event->type() == QEvent::Close){
        writeSettings();
        QApplication::quit();
    }
    return QMainWindow::event(event);
}

void MainWindow::about()
{

}

void MainWindow::initSettings()
{
}

void MainWindow::setFunction(int func)
{
//    qDebug()<<Q_FUNC_INFO<<" Func="<<func;
//    while(_tabWidget->count() > 0){
//        _tabWidget->removeTab(0);
//    }
//    _tabWidget->addTab(_configScreens,"CONFIG");
    _stackWidget->setCurrentIndex(1);
    _configScreens->setActivePage(func);
}

void MainWindow::setActiveNode(Node *node)
{
//    while(_tabWidget->count() > 0){
//        _tabWidget->removeTab(0);
//    }
//    _tabWidget->addTab(_nodeScreens,"BCU");
    _stackWidget->setCurrentIndex(0);
}
