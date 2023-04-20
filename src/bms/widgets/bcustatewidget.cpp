#include "bcustatewidget.h"
#include <QFormLayout>
#include <QLabel>
#include "canopen/indexWidget/indexlabel.h"

BcuStateWidget::BcuStateWidget(QWidget *parent) : QWidget(parent)
{
    createWidgets();
}

void BcuStateWidget::setBCU(BCU *bcu)
{
    if(bcu == nullptr){
        return;
    }

    _bcu = bcu;

    NodeObjectId obj;

    obj = NodeObjectId(0x2002,0x01);
    registerObjId(obj);
    _socLabel->setObjId(obj);
    _socLabel->setNode(_bcu->node());

    obj = NodeObjectId(0x2002,0x02);
    registerObjId(obj);
    _sohLabel->setObjId(obj);
    _sohLabel->setNode(_bcu->node());

    obj = NodeObjectId(0x2002,0x03);
    registerObjId(obj);
    _pvLabel->setObjId(obj);
    _pvLabel->setNode(_bcu->node());

    obj = NodeObjectId(0x2002,0x04);
    registerObjId(obj);
    _paLabel->setObjId(obj);
    _paLabel->setNode(_bcu->node());

    setNodeInterrest(_bcu->node());
}

void BcuStateWidget::createWidgets()
{
    QFormLayout *layout = new QFormLayout();
    layout->setContentsMargins(5,5,5,5);

    _socLabel = new IndexLabel();
    layout->addRow("SOC:",_socLabel);

    _sohLabel = new IndexLabel();
    layout->addRow("SOH:",_sohLabel);

    _pvLabel = new IndexLabel();
    layout->addRow("Pack Volt(V):",_pvLabel);

    _paLabel = new IndexLabel();
    layout->addRow("Pack Current(A):",_paLabel);

    setLayout(layout);
}
void BcuStateWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if((flags & NodeOd::FlagsRequest::Error) != 0){
        return;
    }
}
