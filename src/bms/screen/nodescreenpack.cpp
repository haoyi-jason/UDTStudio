#include "nodescreenpack.h"

#include <QLayout>
#include <QScrollArea>
#include <QSpacerItem>

#include <QGridLayout>
#include <QFormLayout>
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexlcdnumber.h"

#include <QSettings>
#include <QCoreApplication>
#include <QTextCodec>

#include <QDebug>

NodeScreenPack::NodeScreenPack(QWidget *parent)
    :NodeScreen(parent)
{
    _bmuIndex = 0;
    createWidgets();

    emit SetBmuIndex(_bmuIndex);

    QFont f = font();
    f.setPointSize(14);
    setFont(f);

}

void NodeScreenPack::createWidgets()
{
    //_bmuWidget = new BmuWidget(12,5,this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(2,2,2,2);

//    QToolBar *toolBar = new QToolBar("BMU Control");
//    toolBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
//    toolBar->setMinimumHeight(60);
//    toolBar->layout()->setSizeConstraint(QLayout::SetMinimumSize);
//    //toolBar->setIconSize(QSize(80,80));
//    toolBar->setFixedHeight(80);
//    layout->addWidget(toolBar);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

//    QWidget *widget = new QWidget(this);

    QWidget *widget = new QWidget();
    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(0,0,0,0);

    QFormLayout *infoLayout = new QFormLayout();
    infoLayout->setContentsMargins(5,5,5,5);
    //IndexLabel *indexLabel;
    //NodeObjectId obj;

    infoLayout->setHorizontalSpacing(10);
    infoLayout->setVerticalSpacing(10);

    _socLabel = new IndexLabel(NodeObjectId(0x2002,0x1));
    infoLayout->addRow("SOC",_socLabel);
    _sohLabel = new IndexLabel(NodeObjectId(0x2002,0x2));
    infoLayout->addRow("SOH",_sohLabel);
    _pvLabel = new IndexLabel(NodeObjectId(0x2002,0x3));
    infoLayout->addRow("Stack Voltage",_pvLabel);
    _paLabel = new IndexLabel(NodeObjectId(0x2002,0x4));
    infoLayout->addRow("Current(A)",_paLabel);

    vlayout->addItem(infoLayout);
//    _stateWidget = new BcuStateWidget();
//    vlayout->addWidget(_stateWidget);


    //layout->addWidget(_bmuWidget);
    //vlayout->addWidget(toolBar);
    // set to maximum supported count and hide in "setInternal"
    QGridLayout *_cellLayout = new QGridLayout();
    _cellLayout->setContentsMargins(5,5,5,5);

//    _cellLayout->addWidget(new QLabel("Pack#"),0,0);
//    _cellLayout->addWidget(new QLabel("Voltage"),0,1);
//    for(int i=0;i<12;i++){
//        _cellLayout->addWidget(new QLabel(QString("Cell#%1").arg(i+1)),0,i+2);
//    }
//    for(int i=0;i<5;i++){
//        _cellLayout->addWidget(new QLabel(QString("NTC#%1").arg(i+1)),0,i+2+12);
//    }

//    _lcdNumbers.clear();
//    IndexLCDNumber *lcdNumber;
//    NodeObjectId objId;
//    for(int i=0;i<30;i++){
//        objId = NodeObjectId(0x2100 + i,0x01);
//        registerObjId(objId);
//        lcdNumber = new IndexLCDNumber();
////        lcdNumber->setNode(node);
//        lcdNumber->setObjId(objId);
//        lcdNumber->setUnit("V");
//        lcdNumber->setScale(0.1);
//        lcdNumber->setDisplayHint(IndexLCDNumber::DisplayFloat);
//        _lcdNumbers.append(lcdNumber);
//        _cellLayout->addWidget(new QLabel(QString("#%1").arg(i+1)),i+1,0);
//        _cellLayout->addWidget(lcdNumber,i+1,1);

//        for(int j=0;j<12;j++){
//            objId = NodeObjectId(0x2100 + i,j+0xa);
//            registerObjId(objId);
//            lcdNumber = new IndexLCDNumber();
////            lcdNumber->setNode(node);
//            lcdNumber->setObjId(objId);
//            lcdNumber->setUnit("mV");
//            lcdNumber->setScale(1);
//            lcdNumber->setDisplayHint(IndexLCDNumber::DisplayDirectValue);
//            _lcdNumbers.append(lcdNumber);
//            _cellLayout->addWidget(lcdNumber,i+1,j+2);

//        }
//        for(int j=0;j<5;j++){
//            objId = NodeObjectId(0x2100 + i,j+0x18);
//            registerObjId(objId);
//            lcdNumber = new IndexLCDNumber();
////            lcdNumber->setNode(node);
//            lcdNumber->setObjId(objId);
//            lcdNumber->setUnit(QChar(0x2103));
//            lcdNumber->setScale(0.1);
//            lcdNumber->setDisplayHint(IndexLCDNumber::DisplayDirectValue);
//            _lcdNumbers.append(lcdNumber);
//            _cellLayout->addWidget(lcdNumber,i+1,j+2+12);
//        }
//    }

    _cellGroup = new QGroupBox();
    _cellGroup->setLayout(_cellLayout);
    vlayout->addWidget(_cellGroup);

    // add cell widgets

//    for(int i=0;i<30;i++){
//        BmuCellInputWidgets *w = new BmuCellInputWidgets(i,12,5);
//        vlayout->addWidget(w);
//        _bmuCellInputWidgets.append(w);
//        w->setCellRange(3000,3700);
//        w->setNtcRange(20,50);
//    }
    vlayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Fixed,QSizePolicy::Expanding));


    widget->setLayout(vlayout);
    scrollArea->setWidget(widget);
    layout->addWidget(scrollArea);

//    _prevPackAction = toolBar->addAction(tr("Prev"));
//    _nextPackAction = toolBar->addAction(tr("Next"));
//    _onShotAction = toolBar->addAction(tr("One Shot"));
//    _edIndex = new QLineEdit();
//    _edIndex->setText("0x2000");
//    _edSubIndex = new QLineEdit();
//    _edSubIndex->setText("0x01");
//    toolBar->addWidget(_edIndex);
//    toolBar->addWidget(_edSubIndex);
//    _setIndexValue = toolBar->addAction("Set");
//    connect(_setIndexValue,&QAction::triggered,this,&NodeScreenPack::toggleSetIndex);

//    _prevPackAction->setText(tr("<-"));
//    _nextPackAction->setText(tr("->"));

//    connect(_prevPackAction,&QAction::triggered,this,&NodeScreenPack::toggleBMUNavagate);
//    connect(_nextPackAction,&QAction::triggered,this,&NodeScreenPack::toggleBMUNavagate);
//    connect(_onShotAction,&QAction::triggered,this,&NodeScreenPack::toggleOneShot);
    setLayout(layout);
}

QString NodeScreenPack::title() const
{
    return QString(tr("BMU State"));
}

void NodeScreenPack::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(axis);
    //setNodeInterrest(node);
    BCU *bcu = static_cast<BCU*>(node);
    int packs =  _node->nodeOd()->value(0x2001,0x01).toInt();
    int cells =  _node->nodeOd()->value(0x2001,0x02).toInt();
    int ntcs =  _node->nodeOd()->value(0x2001,0x03).toInt();
    int count = 0;

    _socLabel->setNode(node);
    _sohLabel->setNode(node);
    _pvLabel->setNode(node);
    _paLabel->setNode(node);

    _lcdNumbers.clear();
    QGridLayout *_cellLayout = (QGridLayout*)_cellGroup->layout();


    _cellLayout->addWidget(new QLabel("Pack#"),0,0);
    _cellLayout->addWidget(new QLabel("Voltage"),0,1);
    for(int i=0;i<cells;i++){
        _cellLayout->addWidget(new QLabel(QString("C#%1(mV)").arg(i+1)),0,i+2);
    }
    for(int i=0;i<ntcs;i++){
        _cellLayout->addWidget(new QLabel(QString("T#%1-%2").arg(i+1).arg(QChar(0x2103))),0,i+2+12);
    }

    IndexLCDNumber *lcdNumber;
    NodeObjectId objId;
    for(int i=0;i<packs;i++){
        objId = NodeObjectId(0x2100 + i,0x01);
        registerObjId(objId);
        lcdNumber = new IndexLCDNumber();
        lcdNumber->setProperty("ID",1);
  //      lcdNumber->setNode(node);
        lcdNumber->setObjId(objId);
        //lcdNumber->setUnit("V");
        lcdNumber->setScale(0.1);
        lcdNumber->setDisplayHint(IndexLCDNumber::DisplayFloat);
        _lcdNumbers.append(lcdNumber);
        _cellLayout->addWidget(new QLabel(QString("#%1").arg(i+1)),i+1,0);
        _cellLayout->addWidget(lcdNumber,i+1,1);
        for(int j=0;j<cells;j++){
            objId = NodeObjectId(0x2100 + i,j+0xa);
            registerObjId(objId);
            lcdNumber = new IndexLCDNumber();
            lcdNumber->setProperty("ID",2);
  //          lcdNumber->setNode(node);
            lcdNumber->setObjId(objId);
            //lcdNumber->setUnit("mV");
            lcdNumber->setScale(0.001);
            lcdNumber->setDisplayHint(IndexLCDNumber::DisplayFloat);
            _lcdNumbers.append(lcdNumber);
            _cellLayout->addWidget(lcdNumber,i+1,j+2);

        }
        for(int j=0;j<ntcs;j++){
            objId = NodeObjectId(0x2100 + i,j+0x18);
            registerObjId(objId);
            lcdNumber = new IndexLCDNumber();
            lcdNumber->setProperty("ID",3);
//            lcdNumber->setNode(node);
            lcdNumber->setObjId(objId);
            //lcdNumber->setUnit(QChar(0x2103));
            lcdNumber->setScale(0.1);
            lcdNumber->setDisplayHint(IndexLCDNumber::DisplayFloat);
            _lcdNumbers.append(lcdNumber);
            _cellLayout->addWidget(lcdNumber,i+1,j+2+12);
        }
    }

    //_cellLayout->addItem(new QSpacerItem(0,0,QSizePolicy::Fixed,QSizePolicy::Expanding));
    _cellGroup->setLayout(_cellLayout);
    foreach (IndexLCDNumber *v, _lcdNumbers) {
        v->setNode(static_cast<Node*>(bcu));
        //v->show();
    }

    loadCriteria();
//    _stateWidget->setBCU(bcu);
//    foreach (BmuCellInputWidgets *b, _bmuCellInputWidgets) {
//        b->setBCU(bcu);
//        if(count < packs){
//            b->show();
//        }
//        else{
//            b->hide();
//        }
//        count++;
//    }



}

void NodeScreenPack::refreshContent()
{

}


void NodeScreenPack::toggleBMUNavagate()
{
    QAction *act = (QAction*)sender();

    int index = _bmuIndex;

    if(act->text() == "->"){
        index++;
    }
    else if(act->text() == "<-"){
        index--;
    }

    if(index > 12) index = 0;
    if(index < 0) index = 11;

    _bmuIndex = index;
    emit SetBmuIndex(_bmuIndex);
}

void NodeScreenPack::toggleOneShot()
{
    emit OneShot();
}

void NodeScreenPack::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if((flags & NodeOd::FlagsRequest::Error) != 0){
        return;
    }
}

void NodeScreenPack::loadCriteria()
{
    QString path = QCoreApplication::applicationDirPath();
    path  += "//config.ini";
    QString keySearch;

    QSettings *progSetting = new QSettings(path, QSettings::IniFormat);
    progSetting->setIniCodec(QTextCodec::codecForName("UTF-8"));
    int sz = progSetting->beginReadArray("ALARM_SETTINGS");
    QStringList sl,sl2;

    // cell voltage warning
    progSetting->setArrayIndex(0);
    sl = progSetting->value("HIGH").toString().split(",");
    sl2 = progSetting->value("LOW").toString().split(",");
    foreach (IndexLCDNumber *v, _lcdNumbers) {
        if(v->property("ID").toInt() == 2){
            //qDebug()<<Q_FUNC_INFO<< "Confit Cell widgets";
            v->setRangeValue(sl2[0],sl[0]);
        }
    }

    // temperature warning
    progSetting->setArrayIndex(2);
    sl = progSetting->value("HIGH").toString().split(",");
    sl2 = progSetting->value("LOW").toString().split(",");
    foreach (IndexLCDNumber *v, _lcdNumbers) {
        if(v->property("ID").toInt() == 3){
            //qDebug()<<Q_FUNC_INFO<< "Confit NTC widgets "<<sl[0]<<sl2[0];
            v->setRangeValue(sl2[0],sl[0]);
        }
    }

    progSetting->endArray();
}
