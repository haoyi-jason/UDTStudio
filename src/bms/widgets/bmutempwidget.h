#ifndef BMUTEMPWIDGET_H
#define BMUTEMPWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QLCDNumber>

#include "system/bmsstack.h"

class BmuTempWidget:public QWidget
{
    Q_OBJECT
public:
    BmuTempWidget(uint8_t ntc, QWidget *parent = nullptr);

    uint8_t ntc() const;

public slots:
    void setBCU(BCU *bcu);

private:
    uint8_t _ntc;
    BCU *_bcu;

    void createWidgets();

    QLCDNumber *_lcdNumber;
    QLabel *_label;
};

#endif // BMUTEMPWIDGET_H
