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
    f.setPointSize(12);
    setFont(f);

    _configLoaded = false;
    _odError = false;
    _bcu = nullptr;
}

BCU* NodeScreenPack::bcu() const
{
    return _bcu;
}

void NodeScreenPack::setBcu(BCU *bcu)
{
    if(bcu == nullptr) return;
    _bcu = bcu;
    connect(_bcu,&BCU::configReady,this,&NodeScreenPack::BCUConfigReady);
    setNodeInternal(_bcu->node(),0);
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

    _infoGroup = new QGroupBox("電池資訊");
//    QGridLayout *infoLayout = new QGridLayout();
//    infoLayout->setContentsMargins(5,5,5,5);
//    infoLayout->setHorizontalSpacing(10);
//    infoLayout->setVerticalSpacing(10);

//    _pvLabel = new IndexLCDNumber(NodeObjectId(0x2002,0x3));
//    _pvLabel->setScale(0.1);
//    _pvLabel->setUnit("V");
//    infoLayout->addWidget(new QLabel("總電壓"),0,0);
//    infoLayout->addWidget(_pvLabel,0,1);
//    _paLabel = new IndexLCDNumber(NodeObjectId(0x2002,0x4));
//    _paLabel->setScale(0.1);
//    _paLabel->setUnit("A");

//    infoLayout->addWidget(new QLabel("總電流"),0,2);
//    infoLayout->addWidget(_paLabel,0,3);

//    _socLabel = new IndexLCDNumber(NodeObjectId(0x2002,0x1));
//    _socLabel->setUnit("%");
//    infoLayout->addWidget(new QLabel("SOC"),1,0);
//    infoLayout->addWidget(_socLabel,1,1);
//    _sohLabel = new IndexLCDNumber(NodeObjectId(0x2002,0x2));
//    _sohLabel->setUnit("%");
//    infoLayout->addWidget(new QLabel("SOH"),1,2);
//    infoLayout->addWidget(_sohLabel,1,3);

    _bcuInfo = new QLabel("系統狀態:\n\n");
//    infoLayout->addWidget(_bcuInfo,0,4,2,1);

    QHBoxLayout *hlayout = new QHBoxLayout();
    //hlayout->addItem(hl);
    hlayout->addWidget(_bcuInfo);
//    hlayout->addWidget(_bcuInfo);
    hlayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Fixed));

    _infoGroup->setLayout(hlayout);

    vlayout->addWidget(_infoGroup);


    QGridLayout *_cellLayout = new QGridLayout();
    _cellLayout->setContentsMargins(1,5,1,5);

    QVBoxLayout *vl = new QVBoxLayout();
    _cellInfo = new QLabel();
    _cellInfo2 = new QTextBrowser();
    //_cellInfo2->setStyleSheet(css2);
    //_cellInfo->setTextFormat(Qt::RichText);
    vl->addWidget(_cellInfo);

    _cellGroup = new QGroupBox();
    _cellGroup->setLayout(vl);
    vlayout->addWidget(_cellGroup);

    vlayout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Fixed,QSizePolicy::Expanding));


    widget->setLayout(vlayout);
    scrollArea->setWidget(widget);
    layout->addWidget(scrollArea);

    setLayout(layout);
}

QString NodeScreenPack::title() const
{
    return QString(tr("BMU State"));
}

void NodeScreenPack::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(axis);
    //qDebug()<<Q_FUNC_INFO;
    setNodeInterrest(node);
    //_bcu = static_cast<BCU*>(node);
    _node = node;
    if(_node != nullptr){
        //connect(_bcu,&BCU::configReady,this,&NodeScreenPack::BCUConfigReady);
        NodeObjectId objId;
        objId = NodeObjectId(0x2001,0x01);
        _packs = _node->nodeOd()->value(objId).toInt()-1;
        registerObjId(objId);
        objId = NodeObjectId(0x2001,0x02);
        _cells = _node->nodeOd()->value(objId).toInt();
        registerObjId(objId);
        objId = NodeObjectId(0x2001,0x03);
        _ntcs = _node->nodeOd()->value(objId).toInt();
        registerObjId(objId);

        registerObjId(NodeObjectId(0x2011,0x01));
        refreshContent();
        //connect(_bcu,&BCU::updateState,this, &NodeScreenPack::updateBCUInfo);
    }
}

void NodeScreenPack::refreshContent()
{
    return;
    int packs =  _packs;
    int cells =  _cells;
    int ntcs =  _ntcs;

    if(packs > 30) return;
    if(cells > 20) return;
    if(ntcs > 20) return;

    if(_packs==0 || _cells == 0 || _ntcs == 0) return;
    //qDebug()<<Q_FUNC_INFO<<QString(" Pack:%1, CELLS:%2, NTCs:%3").arg(packs).arg(cells).arg(ntcs);

    int count = 0;
    _socLabel->setNode(_node);
    _sohLabel->setNode(_node);
    _pvLabel->setNode(_node);
    _paLabel->setNode(_node);

    _lcdNumbers.clear();
    QGridLayout *_cellLayout = (QGridLayout*)_cellGroup->layout();

    QLayoutItem *item;
    while((item = _cellLayout->takeAt(0)) != nullptr){
        if(item->widget()){
            delete item->widget();
        }
        delete item;
    }

    _cellLayout->addWidget(new QLabel("Pack#"),0,0);
    _cellLayout->addWidget(new QLabel("Voltage"),0,1);
    for(int i=0;i<cells;i++){
        _cellLayout->addWidget(new QLabel(QString("C#%1(V)").arg(i+1)),0,i+2);
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
            objId = NodeObjectId(0x2100 + i,j+0x1a);
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
        for(uint8_t j=2;j<10;j++){
            objId = NodeObjectId(0x2100 + i,j );
            registerObjId(objId);
        }
    }

    //_cellLayout->addItem(new QSpacerItem(0,0,QSizePolicy::Fixed,QSizePolicy::Expanding));
    _cellGroup->setLayout(_cellLayout);
    foreach (IndexLCDNumber *v, _lcdNumbers) {
        v->setNode(static_cast<Node*>(_node));
        //v->show();
    }


    loadCriteria();

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

void NodeScreenPack::updateCellData()
{
    if(_bcu == nullptr) return;

    QString text = _header;
    quint16 v;
    double vd;
    for(int i=0;i<_bcu->nofPacks();i++){
        text += "<tr>";
        text += cellText(QString("#%1").arg(i+1),css_normal);
        v = static_cast<quint16>(node()->nodeOd()->value(0x2100+i,1).toInt());
        text += cellText(QString::number(double(v/10.),'f',1),css_normal);
        for(int j=0;j<_bcu->nofCellsPerPack();j++){
            v = static_cast<quint16>(node()->nodeOd()->value(0x2100+i,j+0x0a).toInt());
            text += cellText(node()->nodeOd()->value(0x2100+i,j+0x0a).toString(),v<2000?css_normal:css_warning);
        }
        for(int j=0;j<_bcu->nofNtcsPerPack();j++){
            vd = (node()->nodeOd()->value(0x2100+i,j+0x1a).toDouble()/10);
            text += cellText(QString("%1").arg(vd,4,'f',1),vd<20?css_normal:css_alarm);
        }
        text += "</tr>";
    }
    text += "</table>";
    _cellInfo->setText(text);


    // update stack state
    updateBCUInfo();
}

QString NodeScreenPack::colorText(QString text, QString color)
{
    return QString("<font color = %1>%2</font>").arg(color).arg(text);
}

QString NodeScreenPack::cellText(QString text,QString style)
{
    return QString("<td %2>%1</td>").arg(text).arg(style);
}

void NodeScreenPack::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    //qDebug()<<Q_FUNC_INFO;
    if((flags & NodeOd::FlagsRequest::Error) != 0){
        // todo: handle error here
        _odError = true;
        return;
    }
    int pack = objId.index() - 0x2100;
    if(objId.index() == 0x2001){
        //qDebug()<<Q_FUNC_INFO;
//        _bcu->configReceived(objId.index(),objId.subIndex());

        switch(objId.subIndex()){
        case 0x01:
            _packs = _node->nodeOd()->value(objId).toInt()-1;
            break;
        case 0x02:
            _cells = _node->nodeOd()->value(objId).toInt();
            break;
        case 0x03:
            _ntcs = _node->nodeOd()->value(objId).toInt();
            //refreshContent();
            break;
        }
    }
    else if(objId.index() == 0x1018){

    }
    else if(objId.index() == 0x2011){
        quint8 cell = static_cast<quint8>(node()->nodeOd()->value(objId.index(),0x02).toInt());
        if(cell == 0xff){
            updateCellData();
        }
    }
//    else if((pack >= 0) && (pack < _packs)){
//        quint16 mask = (quint16)(_node->nodeOd()->value(objId).toInt());
//        int base = pack *(_cells + _ntcs + 1) + 1;
//        switch(objId.subIndex()){
//        case 0x02:
//            //qDebug()<<Q_FUNC_INFO << "Read Balance mask";
//            for(int i=0;i<_cells;i++){
//                _lcdNumbers[base+i]->setBalancing((mask & (1 << i)) != 0);
//            }
//            break;
//        case 0x03:
//            //qDebug()<<Q_FUNC_INFO << "Read open wire mask";
//            for(int i=0;i<_cells;i++){
//                _lcdNumbers[base+i]->setOpenWire((mask & (1 << i)) != 0);
//            }
//            break;
//        }

//    }
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

void NodeScreenPack::BCUConfigReady()
{
      //qDebug()<<Q_FUNC_INFO;
      //refreshContent();
    QString text;

//    text += "<style> \
//             .s1 {color:red} \
//             .s2 {color:blue} \
//            ";


    text += "<table width=100%;><tr>";
    text += cellText("簇",css_header);
    text += cellText("電壓(V)",css_header);

    for(int i=0;i<_bcu->nofCellsPerPack();i++){
        text += cellText(QString("C#%1").arg(i+1),css_header);
    }
    for(int i=0;i<_bcu->nofNtcsPerPack();i++){
        text += cellText(QString("T#%1").arg(i+1),css_header);
    }
    text += "</tr>";

    _header = text;
    text += "</table>";
    _cellInfo->setText(text);

    // add _bcuInfo
}

void NodeScreenPack::updateBCUInfo()
{
    //qDebug()<<Q_FUNC_INFO;
    QString text;
    double value;
    text += "<table width=100%;>";
    text += "<tr>";
    text += cellText("總壓(V)",css_header);
    value = node()->nodeOd()->value(0x2002,0x03).toDouble()/10;
    text += cellText(QString::number(value,'f',1),css_normal);
    text += cellText("總電流(A)",css_header);
    value = node()->nodeOd()->value(0x2002,0x04).toDouble()/10;
    text += cellText(QString::number(value,'f',1),css_normal);
    text += "</tr>";

    text += "<tr>";
    text += cellText("SOC(%)",css_header);
    value = node()->nodeOd()->value(0x2002,0x01).toDouble()/10;
    text += cellText(QString::number(value,'f',1),css_normal);
    text += cellText("SOH(%)",css_header);
    value = node()->nodeOd()->value(0x2002,0x02).toDouble()/10;
    text += cellText(QString::number(value,'f',1),css_normal);
    text += "</tr>";

    text += "</table>";
    _bcuInfo->setText(text);
}
