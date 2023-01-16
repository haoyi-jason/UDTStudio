/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/

#include "indexlcdnumber.h"

#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLCDNumber>
#include <QLabel>
#include <QMenu>
#include <QtGui>

IndexLCDNumber::IndexLCDNumber(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    setObjId(objId);

    _widget = this;
    _openWire = false;
    _balancing = false;

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(0);

    _lcdNumber = new QLCDNumber();
    _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;color:red; background-color:green");
    _lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    _label = new QLabel();
    _label->setStyleSheet("margin-left: 0; padding-left: 0;font: italic;");

    hlayout->addWidget(_lcdNumber);
    hlayout->addWidget(_label);

    setLayout(hlayout);

//    QPalette p = _lcdNumber->palette();
//    p.setColor(p.WindowText,QColor(255,0,0));
//    p.setColor(p.Background,QColor(0,255,0));
//    p.setColor(p.Light,QColor(255,0,0));
//    p.setColor(p.Dark,QColor(0,255,0));

//    _lcdNumber->setPalette(p);

//    setMinValue(900);
//    setMaxValue(1100);
//    setDisplayValue(1200, DisplayAttribute::Normal);
}

void IndexLCDNumber::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    requestReadValue();
}

void IndexLCDNumber::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    indexWidgetMouseClick(event);
}

void IndexLCDNumber::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    indexWidgetMouseMove(event);
}

void IndexLCDNumber::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

    menu->exec(event->globalPos());
    delete menu;

    event->accept();
}

void IndexLCDNumber::setDisplayValue(const QVariant &value, AbstractIndexWidget::DisplayAttribute flags)
{
    if (flags == DisplayAttribute::Error)
    {
        QFont mfont = font();
        mfont.setItalic(true);
        setFont(mfont);
    }
    else
    {
        QFont mfont = font();
        mfont.setItalic(false);
        setFont(mfont);
    }

    if (_hint == AbstractIndexWidget::DisplayFloat)
    {
        _lcdNumber->display(QString::number(value.toDouble(), 'f', 2));
    }
    else if (_hint == AbstractIndexWidget::DisplayDirectValue)
    {
        _lcdNumber->display(value.toInt());

    }
    else
    {
        return;
    }

    if (!_unit.isEmpty())
    {
        _label->setText(unit());
    }

    if(_openWire){
        switch(inBound(value)){
        case BoundTooLow:
            _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;color:blue;background-color:red;");
            break;
        case BoundTooHigh:
            _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;color:red;background-color:red;");
            break;
        default:
            _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;color:black;background-color:red;");
            break;
        }
    }
    else if(_balancing){
        switch(inBound(value)){
        case BoundTooLow:
            _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;color:blue;background-color:green;");
            break;
        case BoundTooHigh:
            _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;color:red;background-color:green;");
            break;
        default:
            _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;color:black;background-color:green;");
            break;
        }
    }
    else{

        switch(inBound(value)){
        case BoundTooLow:
            _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;color:blue;");
            break;
        case BoundTooHigh:
            _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;color:red;");
            break;
        default:
            _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;color:black;");
            break;
        }
    }
}

bool IndexLCDNumber::isEditing() const
{
    return false;
}

bool IndexLCDNumber::isOpenWire() const
{
    return _openWire;
}

bool IndexLCDNumber::isBalancing() const
{
    return _balancing;
}

void IndexLCDNumber::setOpenWire(bool state)
{
    _openWire = state;
}

void IndexLCDNumber::setBalancing(bool state)
{
    _balancing = state;
}

