#include "bcumanagerwidget.h"

#include <QActionGroup>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>

#include "canopenbus.h"
#include "indexdb.h"
#include "services/services.h"


BCUManagerWidget::BCUManagerWidget(QWidget *parent)
    :BCUManagerWidget(nullptr, parent)
{

}

BCUManagerWidget::BCUManagerWidget(Node *node, QWidget *parent)
    :QWidget(parent)
{
    _node = nullptr;
    createWidgets();
    setNode(node);
}

Node *BCUManagerWidget::node() const
{
    return _node;
}

void BCUManagerWidget::setNode(Node *node)
{
    if(node != _node){
        if(_node != nullptr){
            disconnect(_node,&Node::statusChanged,this,&BCUManagerWidget::updateData);
        }
    }

    _node = node;
    if(_node != nullptr){
        connect(_node,&Node::statusChanged,this,&BCUManagerWidget::updateData);
    }

    _groupBox->setEnabled(_node != nullptr);
    _groupNmt->setEnabled(_node != nullptr);

    _actionLoadEds->setEnabled(_node != nullptr);
    _actionReLoadEds->setEnabled(_node != nullptr);

    _actionRemoveNode->setEnabled(_node != nullptr);
    _actionUpdateFirmware->setEnabled(_node != nullptr);

    updateData();
}

void BCUManagerWidget::updateData()
{
    if (_node != nullptr)
    {
        _nodeNameEdit->setText(_node->name());

        // status button update
        _groupNmt->blockSignals(true);
        for (QAction *action : _groupNmt->actions())
        {
            action->setChecked(false);
        }
        int idAction = _node->status() - Node::PREOP;
        if (idAction < 0)
        {
            idAction = 0;
        }
        QAction *actionStatus = _groupNmt->actions()[static_cast<int>(idAction)];
        actionStatus->setCheckable(true);
        actionStatus->setChecked(true);
        _groupNmt->blockSignals(false);
    }
    else
    {
        _nodeNameEdit->setText("");
    }
}
void BCUManagerWidget::preop()
{
    if (_node != nullptr)
    {
        _node->sendPreop();
    }
}

void BCUManagerWidget::start()
{
    if (_node != nullptr)
    {
        _node->sendStart();
    }
}

void BCUManagerWidget::stop()
{
    if (_node != nullptr)
    {
        _node->sendStop();
    }
}

void BCUManagerWidget::resetCom()
{
    if (_node != nullptr)
    {
        _node->sendResetComm();
        updateData();
    }
}

void BCUManagerWidget::resetNode()
{
    if (_node != nullptr)
    {
        _node->sendResetNode();
        updateData();
    }
}

void BCUManagerWidget::removeNode()
{
    if (_node != nullptr)
    {
        Node *node = _node;
        setNode(nullptr);
        node->bus()->removeNode(node);
    }
}

void BCUManagerWidget::updateNodeFirmware()
{
    if (_node != nullptr)
    {
//        BootloaderWidget bootloaderWidget(_node, this);
//        bootloaderWidget.exec();
    }
}

void BCUManagerWidget::loadEds(const QString &edsFileName)
{
    if (_node != nullptr)
    {
        QString fileName = edsFileName;
        if (fileName.isEmpty())
        {
            fileName = QFileDialog::getOpenFileName(this, tr("Choose eds file"), QString(), tr("Eds file (*.eds)"));
            if (fileName.isEmpty())
            {
                return;
            }
        }
        _node->loadEds(fileName);
        updateData();
    }
}

void BCUManagerWidget::reloadEds()
{
    if (_node != nullptr)
    {
        _node->loadEds(_node->edsFileName());
    }
}

void BCUManagerWidget::setNodeName()
{
    if (_node != nullptr)
    {
        _node->setName(_nodeNameEdit->text());
    }
}

void BCUManagerWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    _groupBox = new QGroupBox(tr("Node"));
    QFormLayout *layoutGroupBox = new QFormLayout();
    layoutGroupBox->setSpacing(2);
    layoutGroupBox->setContentsMargins(2, 2, 2, 2);

    // toolbar nmt
    _toolBar = new QToolBar(tr("Node commands"));
    _toolBar->setIconSize(QSize(20, 20));

    _groupNmt = new QActionGroup(this);
    _groupNmt->setExclusive(true);

    _actionPreop = _groupNmt->addAction(tr("Pre operationnal"));
    _actionPreop->setCheckable(true);
    _actionPreop->setIcon(QIcon(":/icons/img/icons8-connection-status-on.png"));
    _actionPreop->setStatusTip(tr("Request node to go in preop mode"));
    connect(_actionPreop, &QAction::triggered, this, &BCUManagerWidget::preop);

    _actionStart = _groupNmt->addAction(tr("Start"));
    _actionStart->setCheckable(true);
    _actionStart->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _actionStart->setStatusTip(tr("Request node to go in started mode"));
    connect(_actionStart, &QAction::triggered, this, &BCUManagerWidget::start);

    _actionStop = _groupNmt->addAction(tr("Stop"));
    _actionStop->setCheckable(true);
    _actionStop->setIcon(QIcon(":/icons/img/icons8-stop.png"));
    _actionStop->setStatusTip(tr("Request node to go in stop mode"));
    connect(_actionStop, &QAction::triggered, this, &BCUManagerWidget::stop);

    _actionResetCom = _groupNmt->addAction(tr("Reset communication"));
    _actionResetCom->setCheckable(true);
    _actionResetCom->setIcon(QIcon(":/icons/img/icons8-process.png"));
    _actionResetCom->setStatusTip(tr("Request node to reset com. parameters"));
    connect(_actionResetCom, &QAction::triggered, this, &BCUManagerWidget::resetCom);

    _actionReset = _groupNmt->addAction(tr("Reset node"));
    _actionReset->setCheckable(true);
    _actionReset->setIcon(QIcon(":/icons/img/icons8-reset.png"));
    _actionReset->setStatusTip(tr("Request node to reset all values"));
    connect(_actionReset, &QAction::triggered, this, &BCUManagerWidget::resetNode);

    _toolBar->addActions(_groupNmt->actions());

    // Remove node
    _actionRemoveNode = new QAction(tr("Remove node"));
    _actionRemoveNode->setIcon(QIcon(":/icons/img/icons8-delete.png"));
    _actionRemoveNode->setStatusTip(tr("Remove the current node from the bus"));
    connect(_actionRemoveNode, &QAction::triggered, this, &BCUManagerWidget::removeNode);

    // Update firmware
    _actionUpdateFirmware = new QAction(tr("Update firmware"));
    _actionUpdateFirmware->setIcon(QIcon(":/icons/img/icons8-restore-page.png"));
    _actionUpdateFirmware->setStatusTip(tr("Launch a firmware update to the node"));
    connect(_actionUpdateFirmware, &QAction::triggered, this, &BCUManagerWidget::updateNodeFirmware);

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
    connect(_actionReLoadEds, &QAction::triggered, this, &BCUManagerWidget::reloadEds);

    layoutGroupBox->addRow(_toolBar);
    layoutGroupBox->addItem(new QSpacerItem(0, 2));

    _nodeNameEdit = new QLineEdit();
    layoutGroupBox->addRow(tr("Name:"), _nodeNameEdit);
    connect(_nodeNameEdit, &QLineEdit::returnPressed, this, &BCUManagerWidget::setNodeName);

    _groupBox->setLayout(layoutGroupBox);
    layout->addWidget(_groupBox);

    layout->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout(layout);
}

QAction *BCUManagerWidget::actionRemoveNode() const
{
    return _actionRemoveNode;
}

QAction *BCUManagerWidget::actionUpdateFirmware() const
{
    return _actionUpdateFirmware;
}

QAction *BCUManagerWidget::actionReLoadEds() const
{
    return _actionReLoadEds;
}

QAction *BCUManagerWidget::actionLoadEds() const
{
    return _actionLoadEds;
}

QAction *BCUManagerWidget::actionReset() const
{
    return _actionReset;
}

QAction *BCUManagerWidget::actionResetCom() const
{
    return _actionResetCom;
}

QAction *BCUManagerWidget::actionStop() const
{
    return _actionStop;
}

QAction *BCUManagerWidget::actionStart() const
{
    return _actionStart;
}

QAction *BCUManagerWidget::actionPreop() const
{
    return _actionPreop;
}
