#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSortFilterProxyModel>
#include <QStackedWidget>

#include "canopenbus.h"
#include "can/canFrameListView/canframelistview.h"
#include "canopen/busnodesmanagerview.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "screen/nodescreenswidget.h"

#include "widgets/bms_busnodemanagerview.h"
#include "canFrameListView/bms_canframelistview.h"

#include "screen/bcuscreenwidget.h"
#include "system/bms_systemconfigwidget.h"

#include "system/bms_alarmcriteria.h"
#include "system/bms_logger.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void exportCfgFile();
    void exportDCF();
    void about();

    void setFunction(int func);
    void setActiveNode(Node *node);
    void exitSuperUser();

protected:
    QDockWidget *_dockWidget;

    void createDocks();
    QDockWidget *_busNodesManagerDock;
//    BusNodesManagerView *_busNodesManagerView;
    BMS_BusNodesManagerView *_busNodesManagerView;

    QDockWidget *_canFrameListDock;
    BMS_CanFrameListView *_canFrameListView;
    QDockWidget *_dataLoggerDock;
    DataLoggerWidget *_dataLoggerWidget;

    void createWidgets();
//    NodeScreensWidget *_nodeScreens;
    BcuScreenWidget *_nodeScreens;

//    QWidget *_configScreens;
    BMS_SystemConfigWidget *_configScreens;



public:
    bool event(QEvent *event) override;
    void createMenus();
    void writeSettings();
    void readSettings();
    void initSettings();

signals:
    void windowIdle();

private:
//    Ui::MainWindow *ui;
    QTabWidget *_tabWidget;
    QStackedWidget *_stackWidget;

    QList<SetResetPair*> _cvwarning;
    QList<SetResetPair*> _cvalarm;
    QList<SetResetPair*> _ctwarning;
    QList<SetResetPair*> _ctalarm;
    QList<SetResetPair*> _socwarning;
    QList<SetResetPair*> _socalarm;

    BMS_Logger *_logger;
};

#endif // MAINWINDOW_H
