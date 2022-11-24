#ifndef BUSMANAGERWIDGET_H
#define BUSMANAGERWIDGET_H

#include <QWidget>

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QToolBar>

#include "canopenbus.h"

class BusManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BusManagerWidget(QWidget *parent = nullptr);
    BusManagerWidget(CanOpenBus *bus, QWidget *parent = nullptr);

    CanOpenBus *bus() const;

    QAction *actionTogleConnect() const;
    QAction *actionExplore() const;
    QAction *actionSyncOne() const;
    QAction *actionSyncStart() const;

public slots:
    void setBus(CanOpenBus *bus);
    void togleConnect();
    void exploreBus();
    void sendSyncOne();
    void toggleSync(bool start);

protected slots:
    void setSyncTimer(int i);
    void setBusName();
    void updateBusData();

protected:
    CanOpenBus *_bus;
    void createWidgets();

    QGroupBox *_groupBox;
    QToolBar *_toolBar;
    QLineEdit *_busNameEdit;
    QSpinBox *_syncTimerSpinBox;

    QAction *_actionTogleConnect;
    QAction *_actionExplore;
    QAction *_actionSyncOne;
    QAction *_actionSyncStart;

signals:

};

#endif // BUSMANAGERWIDGET_H
