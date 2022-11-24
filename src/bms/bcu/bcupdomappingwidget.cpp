#include "bcupdomappingwidget.h"

#include <QVBoxLayout>

#include "node.h"
#include "services/rpdo.h"
#include "services/tpdo.h"

BCUPDOMappingWidget::BCUPDOMappingWidget(QWidget *parent)
    :QScrollArea(parent)
{
    setWidgetResizable(true);
    createWidgets();
}


Node *BCUPDOMappingWidget::node() const
{
    return _node;
}

void BCUPDOMappingWidget::setNode(Node *node)
{
    _node = node;
    if (node != nullptr)
    {
        for (int rpdo = 0; rpdo < 4; rpdo++)
        {
            if (node->rpdos().count() >= rpdo)
            {
                _rpdoMappingWidgets[rpdo]->setPdo(node->rpdos()[rpdo]);
            }
            else
            {
                _rpdoMappingWidgets[rpdo]->setPdo(nullptr);
            }
        }
        for (int tpdo = 0; tpdo < 4; tpdo++)
        {
            if (node->tpdos().count() >= tpdo)
            {
                _tpdoMappingWidgets[tpdo]->setPdo(node->tpdos()[tpdo]);
            }
            else
            {
                _tpdoMappingWidgets[tpdo]->setPdo(nullptr);
            }
        }
    }
}

void BCUPDOMappingWidget::readAllMapping()
{
    if (_node == nullptr)
    {
        return;
    }

    for (RPDO *rpdo : _node->rpdos())
    {
        rpdo->readMapping();
    }
    for (TPDO *tpdo : _node->tpdos())
    {
        tpdo->readMapping();
    }
}

void BCUPDOMappingWidget::clearAllMapping()
{
    if (_node == nullptr)
    {
        return;
    }

    for (RPDO *rpdo : _node->rpdos())
    {
        rpdo->writeMapping(QList<NodeObjectId>());
    }
    for (TPDO *tpdo : _node->tpdos())
    {
        tpdo->writeMapping(QList<NodeObjectId>());
    }
}

void BCUPDOMappingWidget::createWidgets()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(2, 2, 0, 0);

    _toolBar = new QToolBar(tr("PDO commands"));
    _toolBar->setIconSize(QSize(20, 20));

    // read all action
    _actionReadMappings = _toolBar->addAction(tr("Read all"));
    _actionReadMappings->setIcon(QIcon(":/icons/img/icons8-update.png"));
    _actionReadMappings->setShortcut(QKeySequence("Ctrl+R"));
    _actionReadMappings->setStatusTip(tr("Read all PDO mapping from device"));
    connect(_actionReadMappings, &QAction::triggered, this, &BCUPDOMappingWidget::readAllMapping);

    // read all action
    _actionClearMappings = _toolBar->addAction(tr("Clear all"));
    _actionClearMappings->setIcon(QIcon(":/icons/img/icons8-broom.png"));
    _actionClearMappings->setStatusTip(tr("Clear all PDO mappings"));
    connect(_actionClearMappings, &QAction::triggered, this, &BCUPDOMappingWidget::clearAllMapping);

    layout->addWidget(_toolBar);

    for (int rpdo = 0; rpdo < 4; rpdo++)
    {
        PDOMappingWidget *mappingWidget = new PDOMappingWidget();
        _rpdoMappingWidgets.append(mappingWidget);
        layout->addWidget(mappingWidget);
    }

    for (int tpdo = 0; tpdo < 4; tpdo++)
    {
        PDOMappingWidget *mappingWidget = new PDOMappingWidget();
        _tpdoMappingWidgets.append(mappingWidget);
        layout->addWidget(mappingWidget);
    }

    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

    widget->setLayout(layout);
    setWidget(widget);
}
