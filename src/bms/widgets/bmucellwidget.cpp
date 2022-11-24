#include "bmucellwidget.h"

#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLayout>

BmuCellWidget::BmuCellWidget(uint8_t cell, QWidget *parent)
    :QWidget(parent)
{
    _cell = cell;
    createWidgets();

}

uint8_t BmuCellWidget::cell() const{
    return _cell;
}

void BmuCellWidget::readAllObject()
{

}

void BmuCellWidget::setBCU(BCU *bcu)
{
    _bcu = bcu;
}

void BmuCellWidget::setSettings(bool checked)
{

}

void BmuCellWidget::createWidgets()
{
//    QVBoxLayout *channelLayout = new QVBoxLayout();
//    channelLayout->setContentsMargins(0,0,0,0);

//    channelLayout->addItem(QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Expanding));

    QLabel *channelLabel = new QLabel(QString("%1").arg(QString::number(_cell,10)),this);
    channelLabel->setStyleSheet("font: bold 14px; font-size: 20px;");
    channelLabel->setGeometry(0,0,100,25);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    layout->addWidget(channelLabel);

    _lcdNumber = new QLCDNumber();
    _lcdNumber->setStyleSheet("margin-right:0;padding-right:0;");

    _label = new QLabel();
    _label->setStyleSheet("margin-left:0;padding-left:0; font: italic;");

    layout->addWidget(_lcdNumber);
    layout->addWidget(_label);

    setLayout(layout);

}
