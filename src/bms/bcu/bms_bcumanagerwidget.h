#ifndef BMS_BCUMANAGERWIDGET_H
#define BMS_BCUMANAGERWIDGET_H

#include <QWidget>

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QToolBar>
#include <QPushButton>
#include "canopenbus.h"
#include "bmsstack.h"
#include "canopen.h"

class BMS_BCUManagerwidget : public QWidget
{
    Q_OBJECT
public:
    explicit BMS_BCUManagerwidget(QWidget *parent = nullptr);
    BMS_BCUManagerwidget(BCU *bcu, QWidget *parent = nullptr);

    BCU *bcu(uint8_t id) const;

    QAction *actionPreop() const;
    QAction *actionStart() const;
    QAction *actionStop() const;
    QAction *actionReset() const;

    void addBus(quint8 busId);
    void removeBcu(BCU *bcu);

signals:
    void nodeSelected(Node *node);

public slots:
//    void setBcu(BCU *bcu);
//    void updateData();

//    void preop();
//    void start();
//    void stop();
//    void reset();


protected slots:
    void setSyncTimer(int i);
    void setBusName();
    void updateBusData();
    void setCurrentBcu();

protected:
    CanOpen *_canOpen;
    CanOpenBus *_bus;

    void createWidgets();

    QGroupBox *_groupBox;
    QAction *_actionTogleConnect;
    QAction *_actionTogleSync;

    QLineEdit *_bcuIDEdit;
    QPushButton *_btnSetId;

    QList<BCU*> _bcus;
    QList<QPushButton *> _bcuButtons;
    QGroupBox *_bcuGroup;
    BCU *_currentBcu;
};

#endif // BMS_BCUMANAGERWIDGET_H
