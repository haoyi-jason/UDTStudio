#ifndef BMS_BUSMANAGERWIDGET_H
#define BMS_BUSMANAGERWIDGET_H

#include <QWidget>

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QToolBar>
#include <QPushButton>
#include "canopenbus.h"


class BMS_BusManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BMS_BusManagerWidget(QWidget *parent = nullptr);
    BMS_BusManagerWidget(CanOpenBus *bus, QWidget *parent = nullptr);

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

    void setBusAddress();

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

    QLineEdit *_busAddressEdit;
    QPushButton *_btnSetBusAddress;
};

#endif // BMS_BUSMANAGERWIDGET_H
