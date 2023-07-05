#ifndef BCUSCREENWIDGET_H
#define BCUSCREENWIDGET_H

#include "../../lib/udtgui/screen/nodescreenswidget.h"

#include <QWidget>
#include "../../lib/udtgui/screen/nodescreen.h"
#include <QTableWidget>

#include "system/bmsstack.h"
#include <QLabel>

class BcuScreenWidget : public NodeScreensWidget
{
    Q_OBJECT
public:
    BcuScreenWidget(QWidget *parent = nullptr);

    Node *activeNode() const;

public slots:
    void setActiveBcu(BCU *bcu);
    void setActiveNode(Node *node);
    void setActiveTab(int id);
    void setActiveTab(const QString &name);

    void setOneShot();
//    void setBCU(BCU *bcu);

signals:
    void bcuSelected(BCU *bcu);

protected:
    BCU *_bcu;
    //Node *_activeNode;

    void addNode(Node *node);

    //void createWidgets();
    //QTabWidget *_tabWidget;

//    struct NodeScreens{
//        Node *node;
//        QList<NodeScreen*> screens;
//    };

    //QMap<Node*, NodeScreens> _nodesMap;

   // QLabel *_logoLabe;

};

#endif // BCUSCREENWIDGET_H
