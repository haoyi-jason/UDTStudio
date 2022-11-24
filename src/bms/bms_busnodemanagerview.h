#ifndef BMS_BUSNODEMANAGERVIEW_H
#define BMS_BUSNODEMANAGERVIEW_H

#include <QWidget>

#include "busmanagerwidget.h"
#include "bms_busnodetreeview.h"
#include "bcu/bms_nodemanagerwidget.h"
#include "bms_busmanagerwidget.h"

//#include "nodemanagerwidget.h"
#include "bcu/bms_bcumanagerwidget.h"
#include "system/bms_systemmanagerwidget.h"

#include <QActionGroup>
#include <QThread>>

class BMS_Poller;


class BMS_BusNodesManagerView : public QWidget
{
    Q_OBJECT
public:
    explicit BMS_BusNodesManagerView(QWidget *parent = nullptr);
    BMS_BusNodesManagerView(CanOpen *canOpen, QWidget *parent = nullptr);

    CanOpen *canOpen() const;
    void setCanOpen(CanOpen *canOpen);

    CanOpenBus *currentBus() const;
    Node *currentNode() const;

    BMS_BusNodeTreeView *busNodeTreeView() const;
    BMS_NodeManagerWidget *nodeManagerWidget() const;
    BMS_BusManagerWidget *busManagerWidget() const;

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);
    void reloadEds(int id);

signals:
    void busSelected(CanOpenBus *currentBus);
    void nodeSelected(Node *currentNode);
    void functionSelected(int function); // switch node window or setting window

protected slots:
    void addBus(quint8 busId);
    void addBcu(CanOpenBus *bus, quint8 id);
    void removeBcu(CanOpenBus *bus, quint8 id);
    void setFunction(int func);
    void pollProc();
    void handlePollerDone();

public slots:
    void startBcu(int id);
    void stopBcu(int id);
    void preopBcu(int id);
    void startPoll(int interval);
    void stopPoll();

    void start();
    void stop();
    void single();

protected:
    void createWidgets();
    BMS_BusNodeTreeView *_busNodeTreeView;
    BMS_BusManagerWidget *_busManagerWidget;
    BMS_NodeManagerWidget *_nodeManagerWidget;

    CanOpen *_canOpen;

//    BMS_BCUManagerwidget *_bcuManagerWidget;
    QList<BMS_NodeManagerWidget*> _bcuWidgets;
    QList<BCU *> _bcus;
    QGroupBox *_bcuGroup;


    BMS_SystemManagerWidget *_sysManager;

    QAction *_actionSingle;
    QAction *_actionStart;
    QAction *_actionStop;

private:
    BMS_Poller *_poller;
    QActionGroup *_groupControl;
};

class BMS_Poller:public QThread
{
    Q_OBJECT
public:
    BMS_Poller(QObject *parent = nullptr);
    BMS_Poller(QList<BCU*> bcus, QObject *parent = nullptr);
    void stop();
    void setInterval(int interval);

protected:
    void run() override;
    QList<BCU *> _bcus;

signals:
    void started();
    void finished();

private:
    bool _stop;
    int _interval;
};

#endif // BUSNODEMANAGERVIEW_H
