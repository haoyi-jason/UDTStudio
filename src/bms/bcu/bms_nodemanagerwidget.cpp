#include "bms_nodemanagerwidget.h"

#include <QActionGroup>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>

#include "canopenbus.h"
#include "indexdb.h"
#include "services/services.h"

#include "canopen/bootloaderWidget/bootloaderwidget.h"

BMS_NodeManagerWidget::BMS_NodeManagerWidget(QWidget *parent)
    :BMS_NodeManagerWidget(nullptr,parent)
{

}

BMS_NodeManagerWidget::BMS_NodeManagerWidget(Node *node, QWidget *parent)
    : QWidget(parent)
{
    _node = node;
    createWidgets();
    setNode(node);
}

Node *BMS_NodeManagerWidget::node() const
{
    return _node;
}

void BMS_NodeManagerWidget::setBCU(BCU *bcu)
{
    _bcu = bcu;
    if(_bcu != nullptr){
        setNode(_bcu->node());
    }
    else{
        setNode(nullptr);
    }
}

void BMS_NodeManagerWidget::setNode(Node *node)
{
    //if(node == nullptr) return;

    if(node != _node){
        if(_node != nullptr)
        {
            disconnect(_node,&Node::statusChanged,this,&BMS_NodeManagerWidget::updateData);
            //disconnect(_node,&Node::stateChanged);
        }
    }
    _node = node;
    //_bcu = static_cast<BCU*>(node);

    if(_node != nullptr){
        connect(_node,&Node::statusChanged,this,&BMS_NodeManagerWidget::updateData);

        //connect(_bcu,&BCU::threadActive,this,&BMS_NodeManagerWidget::updateThreadState);

        //connect(_bcu,&BCU::statusChanged,this,&BMS_NodeManagerWidget::nodeStatusChanged);
    }

    _groupBox->setEnabled(_node != nullptr);
    _groupNmt->setEnabled(_node != nullptr);

    //_actionStartPoll->setEnabled(_bcu->isConfigReady());

    _actionLoadEds->setEnabled(_node != nullptr);
    _actionLoadEds->setEnabled(_node != nullptr);

    _actionRemoveNode->setEnabled(_node != nullptr);
    _actionUpdateFirmware->setEnabled(_node != nullptr);


    updateData();
}

void BMS_NodeManagerWidget::updateData()
{
    if(_node != nullptr){
        _nodeNameEdit->setText(_node->name());

        _groupNmt->blockSignals(true);
        for(QAction *action:_groupNmt->actions()){
            action->setChecked(false);
        }
        int idAction = _node->status() - Node::PREOP;
        if(idAction < 0){
            idAction = 0;
        }
        QAction *actionStatus = _groupNmt->actions()[static_cast<int>(idAction)];
        actionStatus->setCheckable(true);
        actionStatus->setChecked(true);
        _groupNmt->blockSignals(false);
        QString title = QString("%1 ID=%2").arg(_node->name()).arg(_node->nodeId());
        _groupBox->setTitle(title);
        //connect(_groupBox,&QGroupBox::clicked,this,&BMS_NodeManagerWidget::select);
        //_statusLabel->setText(_bcu->statusStr());
    }
    else{
        _nodeNameEdit->setText("");
    }
}

void BMS_NodeManagerWidget::updateThreadState(bool state)
{
    if(state){
        _actionStartPoll->setToolTip("Stop activity");
    }
    else{
        _actionStartPoll->setToolTip("Start activity");
    }
}

void BMS_NodeManagerWidget::pollNode()
{
    _bcu->startPoll();
}

void BMS_NodeManagerWidget::preop()
{
    if(_node != nullptr){
        _node->sendPreop();
    }
}

void BMS_NodeManagerWidget::start()
{
    if(_node != nullptr){
        _node->sendStart();
    }
}

void BMS_NodeManagerWidget::stop()
{
    if(_node != nullptr){
        _node->sendStop();
    }
}

void BMS_NodeManagerWidget::resetCom()
{
    if(_bcu != nullptr){
        _bcu->reConfig();
        updateData();
    }
}

void BMS_NodeManagerWidget::resetNode()
{
    if(_node != nullptr){
        _node->sendResetNode();
        _bcu->resetError();
        updateData();
    }
}

void BMS_NodeManagerWidget::removeNode()
{
    if(_node != nullptr){
        Node *node = _node;
        setNode(nullptr);
        node->bus()->removeNode(node);
    }
}

void BMS_NodeManagerWidget::updateNodeFirmware()
{
    if(_node != nullptr){
    }
}

void BMS_NodeManagerWidget::loadEds(const QString &edsFileName)
{
    if(_node != nullptr){
        QString fileName = edsFileName;
        if(fileName.isEmpty()){
            fileName = QFileDialog::getOpenFileName(this,tr("Choose eds file"),QString(),tr("EDS file (*.eds)"));
            if(fileName.isEmpty()){
                return;
            }
        }
        _node->loadEds(fileName);
        updateData();
    }
}

void BMS_NodeManagerWidget::reloadEds()
{
    if(_node != nullptr){
        _node->loadEds(_node->edsFileName());
    }
}

void BMS_NodeManagerWidget::setNodeName()
{
    if(_node != nullptr){
        _node->setName(_nodeNameEdit->text());
    }
}

void BMS_NodeManagerWidget::select()
{
    //qDebug()<<Q_FUNC_INFO;
    _bcu->readConfig();

    //emit nodeSelected(_bcu->node());
}

void BMS_NodeManagerWidget::nodeStatusChanged(Node::Status status)
{
    if(_bcu != nullptr){
        _statusLabel->setText(_bcu->statusStr());
    }
}

void BMS_NodeManagerWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);

    _groupBox = new QGroupBox(tr("BCU##"));
    QFormLayout *layoutGroupBox = new QFormLayout();
    layoutGroupBox->setSpacing(2);
    layoutGroupBox->setContentsMargins(2,2,2,2);

    // toolbar nmt
    _toolBar = new QToolBar(tr("Node commands"));
//    _toolBar->setIconSize(QSize(20, 20));

    _groupNmt = new QActionGroup(this);
    _groupNmt->setExclusive(true);

    _actionSelect = _toolBar->addAction(tr("SEL"));
    _actionSelect->setCheckable(false);
    _actionSelect->setIcon(QIcon(":/icons/img/icons8-system-information.png"));
    connect(_actionSelect,&QAction::triggered,this,&BMS_NodeManagerWidget::select);

    _actionPreop = _groupNmt->addAction(tr("Pre operationnal"));
    _actionPreop->setCheckable(true);
    _actionPreop->setIcon(QIcon(":/icons/img/icons8-connection-status-on.png"));
    _actionPreop->setStatusTip(tr("Request node to go in preop mode"));
    connect(_actionPreop, &QAction::triggered, this, &BMS_NodeManagerWidget::preop);

    _actionStart = _groupNmt->addAction(tr("Start"));
    _actionStart->setCheckable(true);
    _actionStart->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _actionStart->setStatusTip(tr("Request node to go in started mode"));
    connect(_actionStart, &QAction::triggered, this, &BMS_NodeManagerWidget::start);

    _actionStop = _groupNmt->addAction(tr("Stop"));
    _actionStop->setCheckable(true);
    _actionStop->setIcon(QIcon(":/icons/img/icons8-stop.png"));
    _actionStop->setStatusTip(tr("Request node to go in stop mode"));
    connect(_actionStop, &QAction::triggered, this, &BMS_NodeManagerWidget::stop);

//    _actionReset = _groupNmt->addAction(tr("Reset"));
//    _actionReset->setCheckable(true);
//    _actionReset->setIcon(QIcon(":/icons/img/icons8-sync.png"));
//    _actionReset->setStatusTip(tr("Reread bcu config"));
//    connect(_actionReset, &QAction::triggered, this, &BMS_NodeManagerWidget::resetCom);

    _actionResetCom = _groupNmt->addAction(tr("Reconfig "));
    _actionResetCom->setCheckable(true);
    _actionResetCom->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    _actionResetCom->setStatusTip(tr("Reread bcu config"));
//    _actionResetCom->setStatusTip(tr("Request node to reset com. parameters"));
    connect(_actionResetCom, &QAction::triggered, this, &BMS_NodeManagerWidget::resetCom);

    _actionReset = _groupNmt->addAction(tr("Reset node"));
    _actionReset->setCheckable(true);
    _actionReset->setIcon(QIcon(":/icons/img/icons8-reset.png"));
    _actionReset->setStatusTip(tr("Request node to reset all values"));
    connect(_actionReset, &QAction::triggered, this, &BMS_NodeManagerWidget::resetNode);

    _actionStartPoll = _toolBar->addAction(tr("Poll"));
    _actionStartPoll->setStatusTip(tr("Poll BCU"));
    connect(_actionStartPoll,&QAction::triggered,this,&BMS_NodeManagerWidget::pollNode);

    _toolBar->addActions(_groupNmt->actions());

    _statusLabel = new QLabel("Status");
    //_toolBar->addWidget(_statusLabel);

    // Remove node
    _actionRemoveNode = new QAction(tr("Remove node"));
    _actionRemoveNode->setIcon(QIcon(":/icons/img/icons8-delete.png"));
    _actionRemoveNode->setStatusTip(tr("Remove the current node from the bus"));
    connect(_actionRemoveNode, &QAction::triggered, this, &BMS_NodeManagerWidget::removeNode);

    // Update firmware
    _actionUpdateFirmware = new QAction(tr("Update firmware"));
    _actionUpdateFirmware->setIcon(QIcon(":/icons/img/icons8-restore-page.png"));
    _actionUpdateFirmware->setStatusTip(tr("Launch a firmware update to the node"));
    connect(_actionUpdateFirmware, &QAction::triggered, this, &BMS_NodeManagerWidget::updateNodeFirmware);

    // EDS actions
    _actionLoadEds = new QAction(tr("Load eds"));
    _actionLoadEds->setIcon(QIcon(":/icons/img/icons8-import-file.png"));
    _actionLoadEds->setStatusTip(tr("Load an eds file as object dictionary description"));
    connect(_actionLoadEds,
            &QAction::triggered,
            this,
            [=]()
            {
                loadEds();
            });

    _actionReLoadEds = new QAction(tr("Reload eds"));
    _actionReLoadEds->setIcon(QIcon(":/icons/img/icons8-restore-page.png"));
    _actionReLoadEds->setStatusTip(tr("Reload the current eds file"));
    connect(_actionReLoadEds, &QAction::triggered, this, &BMS_NodeManagerWidget::reloadEds);

    layoutGroupBox->addRow(_toolBar);
    layoutGroupBox->addItem(new QSpacerItem(0, 2));

    _nodeNameEdit = new QLineEdit();
//    layoutGroupBox->addRow(tr("Name:"), _nodeNameEdit);
    connect(_nodeNameEdit, &QLineEdit::returnPressed, this, &BMS_NodeManagerWidget::setNodeName);

    _groupBox->setLayout(layoutGroupBox);
    layout->addWidget(_groupBox);

    layout->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout(layout);

}

QAction *BMS_NodeManagerWidget::actionRemoveNode() const
{
    return _actionRemoveNode;
}

QAction *BMS_NodeManagerWidget::actionUpdateFirmware() const
{
    return _actionUpdateFirmware;
}

QAction *BMS_NodeManagerWidget::actionReLoadEds() const
{
    return _actionReLoadEds;
}

QAction *BMS_NodeManagerWidget::actionLoadEds() const
{
    return _actionLoadEds;
}

QAction *BMS_NodeManagerWidget::actionReset() const
{
    return _actionReset;
}

QAction *BMS_NodeManagerWidget::actionResetCom() const
{
    return _actionResetCom;
}

QAction *BMS_NodeManagerWidget::actionStop() const
{
    return _actionStop;
}

QAction *BMS_NodeManagerWidget::actionStart() const
{
    return _actionStart;
}

QAction *BMS_NodeManagerWidget::actionPreop() const
{
    return _actionPreop;
}

QAction *BMS_NodeManagerWidget::actionSelect() const
{
    return _actionSelect;
}

void BMS_NodeManagerWidget::addNode(Node *node)
{
//    if(node == nullptr) return;

//    // find if node exist
//    BCU *bcu = static_cast<BCU*>(node);
//    bool found = false;
//    foreach (BCU *b, _bcus) {
//        if(b == bcu){
//            found = true;
//        }
//    }

//    if(!found){
//        _bcus.append(bcu);
//        updateData();
//    }
}
