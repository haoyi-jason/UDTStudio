#ifndef BMUCELLWIDGET_H
#define BMUCELLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QLCDNumber>

#include "bmsstack.h"
class BmuCellWidget:public QWidget
{
    Q_OBJECT
public:
    BmuCellWidget(uint8_t cell, QWidget *parent = nullptr);

    uint8_t cell() const;

    void readAllObject();

public slots:
    void setBCU(BCU *bcu);
    void setSettings(bool checked);

private:
    uint8_t _cell;
    BCU *_bcu;

    void createWidgets();
    QStackedWidget *_stackedWidget;

    QLCDNumber *_lcdNumber;
    QLabel *_label;
};

#endif // BMUCELLWIDGET_H
