#include "nodescreensimulator.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

NodeScreenSimulator::NodeScreenSimulator()
{
    createWidgets();
}

void NodeScreenSimulator::createWidgets()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QPushButton *btn;

    QGridLayout *gl = new QGridLayout();
    gl->setSpacing(20);
    _cboNodes = new QComboBox();
    connect(_cboNodes,qOverload<int>(&QComboBox::currentIndexChanged),this,&NodeScreenSimulator::handleNodeSelection);
    _cbSimulate = new QCheckBox("模擬模式");
    connect(_cbSimulate,&QCheckBox::clicked,this,&NodeScreenSimulator::handleSetSimulation);
    //gl->addWidget(new QLabel("BCU編號"),0,0);
    //gl->addWidget(_cboNodes,0,1);
    gl->addWidget(_cbSimulate,0,0);

    _cboPackNumber = new QComboBox();
    gl->addWidget(new QLabel("Pack #"),1,0);
    gl->addWidget(_cboPackNumber,1,1);

    _cboCellNumber = new QComboBox();
    gl->addWidget(new QLabel("Cell #"),2,0);
    gl->addWidget(_cboCellNumber,2,1);

    _edCv = new FocusedEditor();
    gl->addWidget(_edCv,2,2);
    btn = new QPushButton("設定");
    btn->setProperty("ID",0);
    connect(btn,&QPushButton::clicked,this,&NodeScreenSimulator::setValue);
    gl->addWidget(btn,2,3);


    _cboNtcNumber = new QComboBox();
    gl->addWidget(new QLabel("NTC #"),3,0);
    gl->addWidget(_cboNtcNumber,3,1);
    _edCt = new FocusedEditor();
    gl->addWidget(_edCt,3,2);
    btn = new QPushButton("設定");
    btn->setProperty("ID",1);
    connect(btn,&QPushButton::clicked,this,&NodeScreenSimulator::setValue);
    gl->addWidget(btn,3,3);


    gl->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Fixed),0,4);

    mainLayout->addItem(gl);
    mainLayout->addItem(new QSpacerItem(0,0,QSizePolicy::Fixed,QSizePolicy::Expanding));

    setLayout(mainLayout);
}

void NodeScreenSimulator::handleNodeSelection(int index)
{

}

void NodeScreenSimulator::handleSetSimulation(bool set)
{
    // use od to set node into simulation mode
    // bmsstack class should check this od to know self mode

}

void NodeScreenSimulator::setValue()
{
    QPushButton *btn = static_cast<QPushButton*>(sender());
    int btnid = btn->property("ID").toInt();
    int pack = _cboPackNumber->currentIndex();
    int cell = _cboCellNumber->currentIndex();
    int ntc = _cboNtcNumber->currentIndex();
    // query index->subindex->setvalue
    //NodeSubIndex index;
    switch(btnid){
    case 0: // set cell voltage
        node()->nodeOd()->subIndex(0x2100+pack,cell+0x0a)->setValue(_edCv->text());
        break;
    case 1: // set ntc temp
        node()->nodeOd()->subIndex(0x2100+pack,ntc+0x1a)->setValue(_edCt->text());
        break;
    }
}

QString NodeScreenSimulator::title() const
{
    return QString("Simulator");
}

void NodeScreenSimulator::setNodeInternal(Node *node, uint8_t axis)
{
    if(node != nullptr){
        int packs = node->nodeOd()->value(0x2001,0x01).toInt();
        int cells = node->nodeOd()->value(0x2001,0x02).toInt();
        int ntcs = node->nodeOd()->value(0x2001,0x03).toInt();
        _cboPackNumber->clear();
        _cboCellNumber->clear();
        _cboNtcNumber->clear();
        for(int i=0;i<packs;i++){
            _cboPackNumber->addItem(QString("Pack# %1").arg(i+1));
        }
        for(int i=0;i<cells;i++){
            _cboCellNumber->addItem(QString("Cell# %1").arg(i+1));
        }
        for(int i=0;i<ntcs;i++){
            _cboNtcNumber->addItem(QString("Ntc# %1").arg(i+1));
        }
    }
}

void NodeScreenSimulator::addNode(Node *node)
{

}

void NodeScreenSimulator::setBCU(BCU *bcu)
{

}
