#include "bmucellinputwidgets.h"
#include "canopen/indexWidget/indexlcdnumber.h"

#include <QFormLayout>
#include <QLabel>
#include <QTimer>

BmuCellInputWidgets::BmuCellInputWidgets(quint16 pack, quint8 cells, quint8 ntcs, QWidget *parent) : QWidget(parent)
{
    _pack = pack;
    _cells = cells;
    _ntcs = ntcs;
    _maxCells = cells;
    _maxNtcs = ntcs;
    createWidgets();

}

void BmuCellInputWidgets::readAllObject()
{
    // do not implement
}

const NodeObjectId &BmuCellInputWidgets::cellObjectId() const
{

}

void BmuCellInputWidgets::setBCU(BCU *bcu)
{
    if(bcu == nullptr){
        return;
    }

    _bcu = bcu;
    _objectIds.clear();
    _cells = _bcu->node()->nodeOd()->value(0x2001,0x02).toInt();
    _ntcs = _bcu->node()->nodeOd()->value(0x2001,0x03).toInt();

    if(_cells > _maxCells){
        _cells = _maxCells;
    }

    if(_ntcs > _maxNtcs){
        _ntcs = _maxNtcs;
    }
//    NodeObjectId obj = NodeObjectId();
//    NodeSubIndex sub = obj.
    foreach (auto v, _lcdNumbers) {
        v->hide();
    }
    NodeObjectId obj;
    quint16 index = 0x2100 + _pack;

    obj = NodeObjectId(index,0x01); // pack voltage
    _objectIds.append(obj);
    registerObjId(_objectIds.last());
    _lcdNumbers[0]->setObjId(obj);
    _lcdNumbers[0]->setNode(_bcu->node());
    _lcdNumbers[0]->show();

    for(quint8 i=0; i < _cells; i++){
        obj = NodeObjectId(index,i+0xa);
        _objectIds.append(obj);
        registerObjId(_objectIds.last());
        _lcdNumbers[i+1]->setObjId(obj);
        _lcdNumbers[i+1]->setNode(_bcu->node());
        _lcdNumbers[i+1]->show();
    }
    for(quint8 i=0; i < _ntcs; i++){
        obj = NodeObjectId(index,i+0x18);
        _objectIds.append(obj);
        registerObjId(_objectIds.last());
        _lcdNumbers[i+1+_maxCells]->setObjId(obj);
        _lcdNumbers[i+1+_maxCells]->setNode(_bcu->node());
        _lcdNumbers[i+1+_maxCells]->show();
    }

//    for(int i=0;i<_objectIds.size();i++)
//    {
//        _lcdNumbers[i]->setObjId(_objectIds[i]);
//        _lcdNumbers[i]->setNode( static_cast<Node*>(_bcu) );

//        _lcdNumbers[i]->show();
//        if((i > _cells) && (i < 12 )){
//            _lcdNumbers[i]->hide();
//        }
//        if( i > (12 + _ntcs)){
//            _lcdNumbers[i]->hide();
//        }
//    }


    setNodeInterrest(_bcu->node());



}

void BmuCellInputWidgets::createWidgets()
{
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0,0,0,0);

    _label = new QLabel();
    _label->setText(QString("   # %1").arg(_pack+1));
    hlayout->addWidget(_label);

    IndexLCDNumber *lcd;
    lcd = new IndexLCDNumber();
    lcd->setUnit("V");
    lcd->setScale(0.1);
    lcd->setDisplayHint(IndexLCDNumber::DisplayFloat);
    _lcdNumbers.append(lcd);

    for(int i=0;i<_maxCells;i++){
        lcd = new IndexLCDNumber();
        lcd->setUnit("mV");
        lcd->setScale(1.0);
        lcd->setDisplayHint(IndexLCDNumber::DisplayDirectValue);
        _lcdNumbers.append(lcd);
    }
    for(int i=0;i<_maxNtcs;i++){
        lcd = new IndexLCDNumber();
        lcd->setUnit(QChar(0x2103));
        lcd->setScale(0.1);
        lcd->setDisplayHint(IndexLCDNumber::DisplayFloat);
        _lcdNumbers.append(lcd);
    }

    for(int i=0;i<_lcdNumbers.size();i++){
        hlayout->addWidget(_lcdNumbers[i]);
    }

    setLayout(hlayout);

}

void BmuCellInputWidgets::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if((flags & NodeOd::FlagsRequest::Error) != 0){
        return;
    }

//    foreach (NodeObjectId obj, _objectIds) {
//        if(obj == objId){
//            int index = _objectIds.indexOf(obj);
//            int value = _bcu->nodeOd()->value(obj).toInt();

//        }
//    }
}

void BmuCellInputWidgets::setCellRange(qint16 low, qint16 high)
{
    for(int i=0;i<12;i++){
        _lcdNumbers[i+1]->setRangeValue(low, high);
    }
}

void BmuCellInputWidgets::setNtcRange(qint16 low, qint16 high)
{
    for(int i=0;i<5;i++){
        _lcdNumbers[i+13]->setRangeValue(low, high);
    }
}

void BmuCellInputWidgets::setBalanceMask(uint16_t mask)
{
    for(uint8_t i=0;i<_lcdNumbers.count();i++)
    {
        _lcdNumbers[i]->setBalancing((mask & (1 << i)) != 0 );
    }
}

void BmuCellInputWidgets::setOpenWireMask(uint16_t mask)
{
    for(uint8_t i=0;i<_lcdNumbers.count();i++)
    {
        _lcdNumbers[i]->setOpenWire((mask & (1 << i)) !=0);
    }
}
