#include "bmsstackview.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include "system/login.h"

BMSStackView::BMSStackView(QWidget *parent)
    :BMSStackView(nullptr,parent)
{

}

BMSStackView::BMSStackView(CanOpen *canopen, QWidget *parent)
    :_canopen(canopen)
    ,QWidget(parent)
{
    _stackManager = nullptr;
    _activeBcu = nullptr;
    createWidget();
}

CanOpen *BMSStackView::canOpen() const
{
    return _canopen;
}

void BMSStackView::setCanopen(CanOpen *canOpen)
{
    if(canOpen == nullptr) return;

    if(_canopen != nullptr){
        // todo:
    }

    _canopen = canOpen;
    _logger = nullptr;
    _currentStack = -1;
    _stackManager = nullptr;
}

CanOpenBus *BMSStackView::currentBus() const
{
    return nullptr;
}

void BMSStackView::setLogger(BMS_Logger *logger)
{
    _logger = logger;
}

void BMSStackView::createWidget()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QGridLayout *gl = nullptr;
    QHBoxLayout *hl = nullptr;
    QVBoxLayout *vl;
    QGroupBox *gb;
    QPushButton *btn;

    gb = new QGroupBox("系統訊息");
    vl = new QVBoxLayout();
    hl = new QHBoxLayout();
    gl = new QGridLayout();

    btn = new QPushButton("上一簇");
    connect(btn,&QPushButton::clicked,this,&BMSStackView::handleStackSwitch);
    btn->setProperty("ID",0);
    //hl->addWidget(btn);
    gl->addWidget(btn,0,0);
    btn = new QPushButton("下一簇");
    connect(btn,&QPushButton::clicked,this,&BMSStackView::handleStackSwitch);
    btn->setProperty("ID",1);
    gl->addWidget(btn,0,1);
//    hl->addWidget(btn);
    _btnStartsStop = new QPushButton("啟動");
    _btnStartsStop->setCheckable(true);
    connect(_btnStartsStop,&QPushButton::clicked,this,&BMSStackView::handleStackSwitch);
    _btnStartsStop->setProperty("ID",2);
    gl->addWidget(_btnStartsStop,1,0);

    btn = new QPushButton("進階設定");
    connect(btn,&QPushButton::clicked,this,&BMSStackView::handleStackSwitch);
    btn->setProperty("ID",3);
    gl->addWidget(btn,1,1);
    vl->addItem(gl);

    FocusedEditor *editor;
    _gbConfig = new QGroupBox("BCU設定");
    _gbConfig->setVisible(false);
    gl = new QGridLayout();
    editor = new FocusedEditor();
    _editList.append(editor);
    //editor->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    editor->setFixedWidth(200);
    gl->addWidget(new QLabel("電池數量"),0,0);
    gl->addWidget(editor,0,1);
    // 2001.02
    editor = new FocusedEditor();
    _editList.append(editor);
    //editor->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    editor->setFixedWidth(200);
    gl->addWidget(new QLabel("電芯數量"),1,0);
    gl->addWidget(editor,1,1);
    //2001.03
    editor = new FocusedEditor();
    _editList.append(editor);
    //editor->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    editor->setFixedWidth(200);
    gl->addWidget(new QLabel("溫度感測器數量"),2,0);
    gl->addWidget(editor,2,1);
    // save button
    hl = new QHBoxLayout();
    btn = new QPushButton("儲存");
    connect(btn,&QPushButton::clicked,this,&BMSStackView::handleStackSwitch);
    btn->setProperty("ID",4);
    hl->addWidget(btn);
    btn = new QPushButton("回復預設值");
    connect(btn,&QPushButton::clicked,this,&BMSStackView::handleStackSwitch);
    btn->setProperty("ID",5);
    hl->addWidget(btn);
    gl->addItem(hl,3,0,1,2);

    btn = new QPushButton("重新啟動");
    connect(btn,&QPushButton::clicked,this,&BMSStackView::handleStackSwitch);
    btn->setProperty("ID",6);
    hl->addWidget(btn);

    gl->addItem(hl,3,0,1,2);
    _gbConfig->setLayout(gl);
    vl->addWidget(_gbConfig);

    _hwInfo = new QLabel("硬體資訊");
    vl->addWidget(_hwInfo);

    _stackInfo = new QLabel();
    connect(_stackInfo,&QLabel::linkActivated,this,&BMSStackView::handleHyperlink);
    vl->addWidget(_stackInfo);
    vl->addItem(new QSpacerItem(0,0,QSizePolicy::Fixed,QSizePolicy::Expanding));

    QFont f = _stackInfo->font();
    f.setPixelSize(18);
    _stackInfo->setFont(f);

    QString text;
    text += QString("第[%1/%2]簇<br>").arg(1).arg(1);
    text += QString("總電壓(V):%1<br>").arg(100);
    text += QString("總電流(A):%1<br>").arg(50);
    text += QString("運行狀態:%1<br>").arg(colorText("充電","blue"));
    text += QString("最高電芯電壓:%1,[%2]<br>").arg(1).arg(linkText(QString::number(3),"max_cv"));
    text += QString("最低電芯電壓:%1,[%2]<br>").arg(2).arg(linkText(QString::number(4),"min_cv"));
    text += QString("最高溫:%1,[%2]<br>").arg(1).arg(linkText(QString::number(3),"max_ct"));
    text += QString("最低溫:%1,[%2]<br>").arg(2).arg(linkText(QString::number(4),"min_ct"));
    text += QString("電芯壓差:%1<br>").arg(100);
    //_stackInfo->setText("TEST"+linkText("test","test_key"));
    _stackInfo->setText(text);


    gb->setLayout(vl);
    mainLayout->addWidget(gb);

    gb = new QGroupBox("警報狀態");
    vl = new QVBoxLayout();
    vl->setContentsMargins(0,0,0,0);

    for(int i=0;i<alarm_map.size();i++){
        QLabel *l = new QLabel(alarm_map.at(i));
        l->setMinimumWidth(200);
        l->setMinimumHeight(24);
        l->setAlignment(Qt::AlignHCenter);
        l->setFont(f);
        l->setStyleSheet(style1);
        l->setFrameStyle(QFrame::Panel | QFrame::Sunken);
        l->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        vl->addWidget(l,0,Qt::AlignHCenter);
        _alarmLabels.append(l);
        //vl->setAlignment(l,Qt::AlignHCenter);

    }

    btn = new QPushButton("清除報警");
    connect(btn,&QPushButton::clicked,this,&BMSStackView::handleClearAlarm);
    vl->addWidget(btn);
    gb->setLayout(vl);
    mainLayout->addWidget(gb);




    setLayout(mainLayout);
}

void BMSStackView::setStackManager(BMS_StackManager *manager)
{
    _stackManager = manager;
    if(_stackManager != nullptr){
        connect(_stackManager,&BMS_StackManager::activeBcuChannged,this,&BMSStackView::handleBcuChanged);
        connect(_stackManager,&BMS_StackManager::statusUpdated,this,&BMSStackView::updateView);
    }
}

BMS_StackManager *BMSStackView::stackManager() const
{
    return _stackManager;
}

void BMSStackView::handleStackSwitch()
{
    QPushButton *btn = static_cast<QPushButton*>(sender());
    int id = btn->property("ID").toInt();
    TPDO *pdo = nullptr;
    switch(id){
    case 0:
        if(_stackManager != nullptr)
            _stackManager->prevBcu();
        break;
    case 1:
        if(_stackManager != nullptr)
            _stackManager->nextBcu();
        break;
    case 2: //start/stop
        // change pdo enable by default,
        // use Control index to enable/disable cell data dump
        if(btn->isChecked()){
            btn->setText("停止");
            _stackManager->bcu()->node()->writeObject(0x2003,0x01,0x01);
        }
        else{
            btn->setText("啟動");
            _stackManager->bcu()->node()->writeObject(0x2003,0x01,0x00);
        }
//        pdo = _stackManager->bcu()->node()->tpdos().at(0);
//        qDebug()<<Q_FUNC_INFO<<pdo->isEnabled();
//        if(pdo->isEnabled()){
//            pdo->setEnabled(false);
//            btn->setText("啟動");
//        }
//        else{
//            pdo->setEnabled(true);
//            btn->setText("停止");
//        }
        break;
    case 3: // setting
        if(_gbConfig->isVisible()){
            _gbConfig->setVisible(false);
        }
        else{
            if(Login::instance()->exec() == QDialog::Accepted){
                if(_activeBcu != nullptr){
                    _editList[0]->setText(_activeBcu->node()->nodeOd()->value(0x2001,0x01).toString());
                    _editList[1]->setText(_activeBcu->node()->nodeOd()->value(0x2001,0x02).toString());
                    _editList[2]->setText(_activeBcu->node()->nodeOd()->value(0x2001,0x03).toString());
                    _gbConfig->setVisible(true);
                }
            }
        }
        break;
    case 4: // save
        if(_activeBcu != nullptr){
            for(quint8 i=0;i<0x03;i++){
                _activeBcu->node()->writeObject(0x2001,i+1,_editList[i]->text());
            }
        }
        foreach (PDO *p, _stackManager->bcu()->node()->tpdos()) {
            p->setEnabled(false);
        }
        _stackManager->bcu()->node()->sendPreop();
        QThread::msleep(100);
        _stackManager->bcu()->node()->store(Node::StoreAll);
        QThread::msleep(100);
        _stackManager->bcu()->node()->sendStart();
        foreach (PDO *p, _stackManager->bcu()->node()->tpdos()) {
            p->setEnabled(true);
        }
        break;
    case 5: // restore
        foreach (PDO *p, _stackManager->bcu()->node()->tpdos()) {
            p->setEnabled(false);
        }
        _stackManager->bcu()->node()->sendPreop();
        QThread::msleep(100);
        _stackManager->bcu()->node()->restore(Node::RestoreFactoryAll);
        QThread::msleep(100);
        _stackManager->bcu()->node()->sendStart();
        foreach (PDO *p, _stackManager->bcu()->node()->tpdos()) {
            p->setEnabled(true);
        }
        break;
    case 6: // restart bmu stack
        _stackManager->bcu()->node()->writeObject(0x2003,0x04,0xaa);
        break;
    }
}

void BMSStackView::updateView()
{
    if(_stackManager == nullptr) return;

    //BCU *bcu = _stackManager->bcu();
    QString text = QString("第[%1/%2]簇<br>").arg(_stackManager->currentBcuId()+1).arg(_stackManager->totalBcus());
    text += QString("總電壓(V):%1<br>").arg(_stackManager->packVoltage());
    text += QString("總電流(A):%1<br>").arg(_stackManager->packCurrent());
    text += QString("運行狀態:%1<br>").arg(_stackManager->packCurrent()<-1?colorText("充電","blue"):colorText("放電","red"));
    text += QString("最高電芯電壓:%1,[%2]<br>").arg(_stackManager->maxCV()).arg(linkText2(QString::number(_stackManager->maxCvPID()),QString::number(_stackManager->maxCvPos()),"max_cv"));
    text += QString("最低電芯電壓:%1,[%2]<br>").arg(_stackManager->minCV()).arg(linkText2(QString::number(_stackManager->minCvPID()),QString::number(_stackManager->minCvPos()),"min_cv"));
    text += QString("最高溫:%1,[%2]<br>").arg(_stackManager->maxCT()).arg(linkText2(QString::number(_stackManager->maxCtPID()),QString::number(_stackManager->maxCtPos()),"max_ct"));
    text += QString("最低溫:%1,[%2]<br>").arg(_stackManager->minCT()).arg(linkText2(QString::number(_stackManager->minCtPID()),QString::number(_stackManager->minCtPos()),"min_ct"));
    text += QString("電芯壓差:%1<br>").arg(_stackManager->cvDiff());

    _stackInfo->setText(text);
}

QString BMSStackView::colorText(QString text, QString color)
{
    return QString("<font color = %1>%2</font>").arg(color).arg(text);
}

QString BMSStackView::linkText(QString text, QString key)
{
    return QString("<a href = %1>%2</a>").arg(key).arg(text);
}

QString BMSStackView::linkText2(QString text1,QString text2, QString key)
{
    return QString("<a href = %1>%2-%3</a>").arg(key).arg(text1).arg(text2);
}

void BMSStackView::handleBcuChanged(BCU *bcu)
{
    if(bcu != nullptr){
        _activeBcu = bcu;
        updateView();
        if(_stackManager->bcu()->isConfigReady()){
            quint8 sta = static_cast<quint8>(_stackManager->bcu()->node()->nodeOd()->value(0x2003,0x01).toInt());
            if(sta != 0){
                _btnStartsStop->setChecked(true);
                _btnStartsStop->setText("停止");
            }
            else{
                _btnStartsStop->setChecked(false);
                _btnStartsStop->setText("啟動");
            }


        }
        QString text;
        text = QString::number(_stackManager->bcu()->node()->nodeOd()->value(0x2001,0x01).toInt());
        _editList[0]->setText(text);
        text = QString::number(_stackManager->bcu()->node()->nodeOd()->value(0x2001,0x02).toInt());
        _editList[1]->setText(text);
        text = QString::number(_stackManager->bcu()->node()->nodeOd()->value(0x2001,0x03).toInt());
        _editList[2]->setText(text);
//        setNodeInterrest(bcu->node());
//        foreach (IndexFocuseditor *editor, _editList) {
//            editor->setNode(bcu->node());
//        }
    }
}

void BMSStackView::handleHyperlink(QString link)
{
    qDebug()<<Q_FUNC_INFO<<link;
}

void BMSStackView::handleClearAlarm()
{
    if(_stackManager != nullptr){
        _stackManager->clearAlarm();
    }
}

void BMSStackView::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{

}
