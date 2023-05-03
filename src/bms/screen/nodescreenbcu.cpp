#include "nodescreenbcu.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>

#include "canopen/indexWidget/indexcombobox.h"
#include "canopen/indexWidget/indexlabel.h"
#include "screen/nodescreenswidget.h"

#include "bootloader/bootloader.h"

#include "canopen/bootloaderWidget/bootloaderwidget.h"

#include "widgets/bmuwidget.h"
#include "system/login.h"
#include <QDebug>
#include "bms_ui/indexfocuseditor.h"

NodeScreenBCU::NodeScreenBCU()
{
    createWidgets();
}

void NodeScreenBCU::resetHardware()
{
    _node->bootloader()->resetProgram();
}

void NodeScreenBCU::createWidgets()
{
    QLayout *glayout = new QVBoxLayout();
    glayout->setContentsMargins(2,2,2,2);

    QLayout *toolBarLayout = new QVBoxLayout();
    toolBarLayout->setContentsMargins(2,2,2,2);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    QWidget *widget = new QWidget(this);
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(2,2,2,2);
    layout->setSpacing(0);

    layout->addWidget(createSumaryWidget());
    layout->addWidget(createControlWidget());
//    layout->addWidget(createInfoWidget());
//    layout->addWidget(createStatusWidget());

    //layout->addWidget(new BmuWidget());

    widget->setLayout(layout);
    scrollArea->setWidget(widget);
    glayout->addWidget(scrollArea);

    setLayout(glayout);
}
QWidget *NodeScreenBCU::createSumaryWidget()
{
    IndexFocuseditor *editor;
    _groupBCU = new QGroupBox("BCU 資訊");
    QVBoxLayout *hlayout = new QVBoxLayout();

    _summaryIconLabel = new QLabel();
    _summaryIconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    _summaryIconLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);
    hlayout->addWidget(_summaryIconLabel);

    QFormLayout *sumaryLayout = new QFormLayout();
    sumaryLayout->setHorizontalSpacing(10);
    sumaryLayout->setVerticalSpacing(10);

    IndexLabel *indexLabel;
    indexLabel = new IndexLabel(NodeObjectId(0x1008, 0));
    sumaryLayout->addRow(tr("裝置名稱:"), indexLabel);
    _indexWidgets.append(indexLabel);

//    _summaryProfileLabel = new QLabel();
//    sumaryLayout->addRow(tr("Profile:"), _summaryProfileLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x1009, 0));
    sumaryLayout->addRow(tr("硬體版本:"), indexLabel);
    _indexWidgets.append(indexLabel);

//    indexLabel = new IndexLabel(NodeObjectId(0x2001, 0));
//    sumaryLayout->addRow(tr("Manufacture date:"), indexLabel);
//    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x1018, 4));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayHexa);
    sumaryLayout->addRow(tr("序號:"), indexLabel);
    _indexWidgets.append(indexLabel);

//    indexLabel = new IndexLabel(NodeObjectId(0x100A, 0));
//    sumaryLayout->addRow(tr("Software version:"), indexLabel);
//    _indexWidgets.append(indexLabel);

//    indexLabel = new IndexLabel(NodeObjectId(0x2003, 0));
//    sumaryLayout->addRow(tr("Software build:"), indexLabel);
//    _indexWidgets.append(indexLabel);

    //indexLabel = new IndexLabel(NodeObjectId(0x2001, 1));
    //indexLabel->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
//    indexLabel->setScale(1.0);
//    indexLabel->setUnit(" ");
    editor = new IndexFocuseditor(NodeObjectId(0x2001,1));
    //editor->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    //editor->setObjId(NodeObjectId(0x2001,1));
    sumaryLayout->addRow(tr("本串電池數量:"), editor);
    //_indexWidgets.append(indexLabel);
    editor->setFixedWidth(200);
    _indexWidgets.append(editor);

    indexLabel = new IndexLabel(NodeObjectId(0x2001, 2));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    indexLabel->setScale(1.0);
    indexLabel->setUnit(" ");
    editor = new IndexFocuseditor(NodeObjectId(0x2001,2));
    editor->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    editor->setFixedWidth(200);
    sumaryLayout->addRow(tr("每顆電池電芯數:"), editor);
    _indexWidgets.append(indexLabel);
    _indexWidgets.append(editor);

    indexLabel = new IndexLabel(NodeObjectId(0x2001, 3));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    indexLabel->setScale(1.0);
    indexLabel->setUnit(" ");
    editor = new IndexFocuseditor(NodeObjectId(0x2001,3));
    editor->setFixedWidth(200);
    editor->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    sumaryLayout->addRow(tr("每顆電池溫度感測器數量:"), editor);
    _indexWidgets.append(indexLabel);
    _indexWidgets.append(editor);

    hlayout->addItem(sumaryLayout);

    QVBoxLayout *buttonlayout = new QVBoxLayout();
    buttonlayout->setSpacing(3);
    QPushButton *btn;

//    btn = new QPushButton(tr("啟動BCU"));
//    btn->setFixedWidth(200);
//    connect(btn,&QPushButton::released,this,[=](){startBCU();});
//    buttonlayout->addWidget(btn);

    btn = new QPushButton(tr("設定模式"));
    btn->setFixedWidth(200);
    connect(btn,&QPushButton::released,this,[=](){setCanState();});
    buttonlayout->addWidget(btn);

    btn = new QPushButton(tr("儲存參數"));
    btn->setFixedWidth(200);
    connect(btn,&QPushButton::released,this,[=](){saveParam();});
    buttonlayout->addWidget(btn);

    btn = new QPushButton(tr("回復預設值"));
    btn->setFixedWidth(200);
    connect(btn,&QPushButton::released,this,[=](){loadDefault();});
    buttonlayout->addWidget(btn);
    //    QPushButton *updateFirmwareButton = new QPushButton(tr("&Update firmware"));
//    updateFirmwareButton->setFixedWidth(200);
//    connect(updateFirmwareButton,
//            &QPushButton::released,
//            this,
//            [=]()
//            {
//                updateFirmware();
//            });
//    buttonlayout->addWidget(updateFirmwareButton);

//    QPushButton *resetHardwareButton = new QPushButton(tr("&Reset hardware"));
//    resetHardwareButton->setFixedWidth(200);
//    connect(resetHardwareButton,
//            &QPushButton::released,
//            this,
//            [=]()
//            {
//                resetHardware();
//            });
//    buttonlayout->addWidget(resetHardwareButton);

//    buttonlayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

    hlayout->addItem(buttonlayout);
    hlayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

    _groupBCU->setLayout(hlayout);
    _groupBCU->setEnabled(false);
    return _groupBCU;

}

QWidget *NodeScreenBCU::createControlWidget()
{
    QWidget *w = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout();

    QPushButton *btn;

    btn = new QPushButton(tr("更改設定"));
    btn->setFixedWidth(200);
    connect(btn,&QPushButton::released,this,[=](){logIn();});
    layout->addWidget(btn);

    btn = new QPushButton(tr("啟動BCU"));
    btn->setFixedWidth(200);
    connect(btn,&QPushButton::released,this,[=](){startBCU();});
    layout->addWidget(btn);

    w->setLayout(layout);
    return w;
}

QWidget *NodeScreenBCU::createInfoWidget()
{
    IndexLabel *indexLabel;
    QGroupBox *groupBox = new QGroupBox(tr("BCU Info"));
    QFormLayout *sumaryLayout = new QFormLayout();

    indexLabel = new IndexLabel(NodeObjectId(0x2000, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 100.0);
    indexLabel->setUnit(" V");
    sumaryLayout->addRow(tr("Board voltage:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2020, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 10.0);
    indexLabel->setUnit(" °C");
    sumaryLayout->addRow(tr("CPU temperature:"), indexLabel);
    _indexWidgets.append(indexLabel);

    groupBox->setLayout(sumaryLayout);
    return groupBox;
}

QWidget *NodeScreenBCU::createStatusWidget()
{

    IndexLabel *indexLabel;
    QGroupBox *groupBox = new QGroupBox(tr("Status"));
    QFormLayout *sumaryLayout = new QFormLayout();

    indexLabel = new IndexLabel(NodeObjectId(0x2000, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 100.0);
    indexLabel->setUnit(" V");
    sumaryLayout->addRow(tr("Board voltage:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2020, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 10.0);
    indexLabel->setUnit(" °C");
    sumaryLayout->addRow(tr("CPU temperature:"), indexLabel);
    _indexWidgets.append(indexLabel);

    groupBox->setLayout(sumaryLayout);
    return groupBox;
}

void NodeScreenBCU::updateInfos(BCU *bcu)
{

}

QString NodeScreenBCU::title() const
{
    return QString(tr("BCU資訊"));
}

void NodeScreenBCU::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(axis);

    if(node != nullptr){

        for(AbstractIndexWidget *indexWidget:qAsConst(_indexWidgets)){
            indexWidget->setNode(node);
        }
    }


//    for(AbstractIndexWidget *indexWidget:qAsConst(_indexEditors)){
//        indexWidget->setNode(node);
//    }

    //updateInfos((BCU*)node);

}

void NodeScreenBCU::startBCU()
{
    QPushButton *btn = static_cast<QPushButton*>(sender());
    TPDO *pdo = node()->tpdos().at(0);
    if(pdo->isEnabled()){
        pdo->setEnabled(false);
        btn->setText(tr("啟動BCU"));
        if(_bcu != nullptr){
            _bcu->stopPoll();
        }
    }
    else{
        pdo->setEnabled(true);
        btn->setText(tr("停止BCU"));
        if(_bcu != nullptr){
            _bcu->startPoll();
        }
    }
}

void NodeScreenBCU::setCanState()
{
    QPushButton *btn = static_cast<QPushButton*>(sender());
    if(node()->status() == Node::STARTED){
        node()->sendPreop();
        btn->setText(tr("運行模式"));
    }
    else if(node()->status() == Node::PREOP){
        node()->sendStart();
        btn->setText(tr("設定模式"));
    }
}

void NodeScreenBCU::saveParam()
{
    Node::StoreSegment segment = Node::StoreAll;
    node()->store(segment);
}

void NodeScreenBCU::loadDefault()
{
    Node::RestoreSegment segment = Node::RestoreFactoryAll;
    node()->restore(segment);
}

BCU *NodeScreenBCU::bcu() const
{
    return _bcu;
}

void NodeScreenBCU::setBCU(BCU *bcu)
{
    qDebug()<<Q_FUNC_INFO;
    _bcu = bcu;
    if(_bcu != nullptr){
        setNodeInternal(_bcu->node(),0);
    }
}

void NodeScreenBCU::logIn()
{
    QPushButton *btn = static_cast<QPushButton*>(sender());
    if(_groupBCU->isEnabled()){
        _groupBCU->setEnabled(false);
        btn->setText(tr("更改設定"));
    }
    else{
        if(Login::instance()->exec() == QDialog::Accepted){
            _groupBCU->setEnabled(true);
            btn->setText(tr("完成"));
        }
        else{
            _groupBCU->setEnabled(false);
            btn->setText(tr("更改設定"));
        }
    }
}
