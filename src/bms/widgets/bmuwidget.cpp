#include "bmuwidget.h"

/* widget to display bmu state at right of the window
 *
 */

#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QScrollArea>

#include "bmucellwidget.h"
#include "bmutempwidget.h"

BmuWidget::BmuWidget(QWidget *parent)
    :BmuWidget(12,4,parent)
{

}

BmuWidget::BmuWidget(uint8_t cellCount, uint8_t ntcCount, QWidget *parent)
    :QWidget(parent)
    ,_cellCount(cellCount)
    ,_ntcCount(ntcCount)
{
    createWidgets();
}

BCU *BmuWidget::bcu() const
{
    return _bcu;
}

BatteryPack *BmuWidget::pack() const
{
    return _pack;
}

void BmuWidget::setSettings(bool checked)
{

}

void BmuWidget::readAllObject()
{

}

void BmuWidget::readCellVoltage()
{

}

void BmuWidget::readTemperature()
{

}

void BmuWidget::setBCU(BCU *bcu)
{
    if(bcu == nullptr) return;

    _bcu = bcu;
}

void BmuWidget::setPack(BatteryPack *pack)
{
    if(pack == nullptr) return;

    _pack = pack;

    // todo : refres data
}

void BmuWidget::createWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    QLabel *cellLabel = new QLabel("Cell");
    QLabel *voltLabel = new QLabel("Voltage");
    QLabel *balanceLabel = new QLabel("Balance");

    QHBoxLayout *column = new QHBoxLayout();
    column->setContentsMargins(0,0,0,0);
    column->addWidget(cellLabel);
    column->addWidget(voltLabel);
    column->addWidget(balanceLabel);

    layout->addItem(column);

    QWidget *cellWidget = new QWidget(this);
    QVBoxLayout *cellLayout = new QVBoxLayout(cellWidget);
    cellLayout->setContentsMargins(0,0,0,0);

    for(uint8_t i=0;i<_cellCount;i++){
        QFrame *frame = new QFrame();
        frame->setFrameStyle(QFrame::HLine);
        frame->setFrameShadow(QFrame::Sunken);
        cellLayout->addWidget(frame);
        _bmuCellWidgets.append(new BmuCellWidget(i,this));
        cellLayout->addWidget(_bmuCellWidgets.at(i));
    }

    QScrollArea *scrollArea = new QScrollArea();
    //scrollArea->setLayout(slayout);
    scrollArea->setWidget(cellWidget);
    scrollArea->setWidgetResizable(true);
    layout->addWidget(scrollArea);


    QHBoxLayout *column2 = new QHBoxLayout();
    QLabel *ntcLabel = new QLabel("NTC");
    QLabel *tempLabel = new QLabel("Temperature");
    QLabel *stateLabel = new QLabel("State");
    column2->setContentsMargins(0,0,0,0);
    column2->addWidget(ntcLabel);
    column2->addWidget(tempLabel);
    column2->addWidget(stateLabel);

    layout->addItem(column2);

    QWidget *ntcWidget = new QWidget(this);
    QVBoxLayout *ntcLayout = new QVBoxLayout(ntcWidget);
    ntcLayout->setContentsMargins(0,0,0,0);
    for(uint8_t i=0;i<_ntcCount;i++){
        QFrame *frame = new QFrame();
        frame->setFrameStyle(QFrame::HLine);
        frame->setFrameShadow(QFrame::Sunken);
        ntcLayout->addWidget(frame);
        _bmuTempWidgets.append(new BmuTempWidget(i,this));
        ntcLayout->addWidget(_bmuTempWidgets.at(i));
    }
    QScrollArea *scrollAreaNtc = new QScrollArea();
    //scrollArea->setLayout(slayout);
    scrollAreaNtc->setWidget(ntcWidget);
    scrollAreaNtc->setWidgetResizable(true);

//    QVBoxLayout *slayout = new QVBoxLayout();
//    slayout->setContentsMargins(0,0,0,0);
//    slayout->addWidget(scrollArea);
//    slayout->addWidget(scrollAreaNtc);


    layout->addWidget(scrollAreaNtc);
    setLayout(layout);
}
