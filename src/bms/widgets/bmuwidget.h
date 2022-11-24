#ifndef BMUWIDGET_H
#define BMUWIDGET_H

#include <QWidget>
#include <QToolBar>

#include "bmsstack.h"



class BmuCellWidget;
class BmuTempWidget;

class BmuWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BmuWidget(QWidget *parent = nullptr);
    BmuWidget(uint8_t cellCount, uint8_t ntcCount, QWidget *parent = nullptr);

    BCU *bcu() const;
    BatteryPack *pack() const;

public slots:
    void setBCU(BCU *bcu);
    void setSettings(bool checked);
    void readAllObject();
    void readCellVoltage();
    void readTemperature();
    //void dtaLogger();
    void setPack(BatteryPack *pack);

signals:
    //void settings(bool checked);

private:
    quint8 _cellCount;
    quint8 _ntcCount;
    BCU *_bcu;
    BatteryPack *_pack;

    QList<BmuCellWidget*> _bmuCellWidgets;
    QList<BmuTempWidget*> _bmuTempWidgets;

    void createWidgets();

};

#endif // BMUWIDGET_H
