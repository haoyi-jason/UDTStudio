#include "bcuscreenwidget.h"

#include "../../lib/udtgui/screen/nodescreenod.h"
#include "../../lib/udtgui/screen/nodescreenhome.h"
#include "../../lib/udtgui/screen/nodescreenpdo.h"
#include "../../lib/udtgui/screen/nodescreensynchro.h"
#include "../../lib/udtgui/screen/nodescreenuio.h"
#include "../../lib/udtgui/screen/nodescreenuioled.h"
#include "../../lib/udtgui/screen/nodescreenumcmotor.h"



#include "screen/nodescreenbcu.h"
#include "screen/nodescreenpack.h"
#include "screen/nodescreensimulator.h"

#include <QApplication>
#include <QHBoxLayout>

#include <QDebug>

BcuScreenWidget::BcuScreenWidget(QWidget *parent)
    :NodeScreensWidget(parent)
{
    _activeNode = nullptr;
    _bcu = nullptr;

    //createWidgets();

    //setStyleSheet("QTabWidget::tab-bar {left: 5px;}");
}

Node *BcuScreenWidget::activeNode() const
{
    return _activeNode;
}

void BcuScreenWidget::setActiveBcu(BCU *bcu)
{
    if(bcu != nullptr){
        _bcu = bcu;
        setActiveNode(_bcu->node());
        emit bcuSelected(bcu);
    }
}

void BcuScreenWidget::setActiveNode(Node *node)
{
    qDebug()<<Q_FUNC_INFO<< ((node == nullptr)?"NULL":node->name());
    int currentIndex = _tabWidget->currentIndex();

    // remove all screens from QTabWidget
    while (_tabWidget->count() > 0)
    {
        _tabWidget->removeTab(0);
    }

    _activeNode = node;
    //_activeBcu = static_cast<BCU*>(node);

    if (node != nullptr)
    {
        addNode(node);

        // add all screens from nodeScreens to QTabWidget and set node
        NodeScreens nodeScreens = _nodesMap.value(_activeNode);
        for (NodeScreen *screen : qAsConst(nodeScreens.screens))
        {
            _tabWidget->addTab(screen, screen->icon(), " " + screen->title() + " ");
        }

//        if(currentIndex<0 && _tabWidget->count() > 0){
//            currentIndex = 0;
//        }
        _tabWidget->setCurrentIndex(currentIndex);
        //_tabWidget->setVisible(true);
    }

//    qDebug()<<"Active Node:"<<_activeNode;
//    qDebug()<<"Tabwidget Visible? "<<_tabWidget->isVisible();
//    _tabWidget->setVisible(true);
//    qDebug()<<"Tabwidget Visible? "<<_tabWidget->isVisible();
}

void BcuScreenWidget::setOneShot()
{
//    if(_bcu == nullptr) return;

    //_bcu->accessVoltage(0,0);
}

void BcuScreenWidget::setActiveTab(int id)
{
    _tabWidget->setCurrentIndex(id);
}

void BcuScreenWidget::setActiveTab(const QString &name)
{
    for (int tabIndex = 0; tabIndex < _tabWidget->count(); tabIndex++)
    {
        const QString &tabName = _tabWidget->tabText(tabIndex).trimmed();
        if (tabName.compare(name, Qt::CaseInsensitive) == 0)
        {
            _tabWidget->setCurrentIndex(tabIndex);
            return;
        }
    }
}


void BcuScreenWidget::addNode(Node *node)
{
    QMap<Node *, NodeScreens>::const_iterator nodeIt = _nodesMap.constFind(node);
    if (nodeIt != _nodesMap.constEnd())
    {
        return;
    }

    // add generic screens to the NodeScreensStruct
    NodeScreens nodeScreens;
    nodeScreens.node = node;

    NodeScreen *screen;

//    screen = new NodeScreenBCU();
//    // this signal not used
//    //connect(this,&BcuScreenWidget::bcuSelected,static_cast<NodeScreenBCU*>(screen),&NodeScreenBCU::setBCU);
//    screen->setNode(node);
//    screen->setScreenWidget(this);
//    nodeScreens.screens.append(screen);

    QApplication::processEvents();
    screen = new NodeScreenPack();
    connect(this,&BcuScreenWidget::bcuSelected,static_cast<NodeScreenPack*>(screen),&NodeScreenPack::setBcu);
    screen->setNode(node);
    screen->setScreenWidget(this);
    nodeScreens.screens.append(screen);
    //connect((NodeScreenPack*)screen,&NodeScreenPack::OneShot,this,&NodeScreenPack::setOneShot);
//    QApplication::processEvents();
//    screen = new NodeScreenHome();
//    screen->setNode(node);
//    screen->setScreenWidget(this);
//    nodeScreens.screens.append(screen);

    QApplication::processEvents();
    screen = new NodeScreenOD();
    screen->setNode(node);
    screen->setScreenWidget(this);
    nodeScreens.screens.append(screen);

    QApplication::processEvents();
    screen = new NodeScreenPDO();
    screen->setNode(node);
    screen->setScreenWidget(this);
    nodeScreens.screens.append(screen);

    // add specific screens node
    QApplication::processEvents();
    screen = new NodeScreenSimulator();
    screen->setNode(node);
    screen->setScreenWidget(this);
    connect(this,&BcuScreenWidget::bcuSelected,static_cast<NodeScreenSimulator*>(screen),&NodeScreenSimulator::setBcu);
    nodeScreens.screens.append(screen);

    _nodesMap.insert(node, nodeScreens);

}

//void BcuScreenWidget::createWidgets()
//{
//    QLayout *layout = new QVBoxLayout();
//    layout->setContentsMargins(2, 2, 2, 2);


//    //_tabWidget = new QTabWidget();
//    //QLabel *lb = new QLabel("Test");
//    //lb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
//    //layout->addWidget(lb);
//    layout->addWidget(_tabWidget);

//    setLayout(layout);
//}

