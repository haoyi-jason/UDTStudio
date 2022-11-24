#ifndef NODESCREENBCU_H
#define NODESCREENBCU_H

#include "../lib/udtgui/screen/nodescreen.h"
#include "canopen/nodeod/nodeodwidget.h"
#include "bmsstack.h"

class AbstractIndexWidget;
class QLabel;
class IndexLabel;
class IndexComboBox;


class NodeScreenBCU : public NodeScreen
{
    Q_OBJECT
public:
    NodeScreenBCU();

public:
    void resetHardware();

protected:
    void createWidgets();
    QWidget *createSumaryWidget();
    QLabel *_summaryIconLabel;
    QLabel *_summaryProfileLabel;

    QWidget *createInfoWidget();

    QWidget *createStatusWidget();


    QList<AbstractIndexWidget *> _indexWidgets;
    void updateInfos(BCU *bcu);

public:
    QString title() const override;
    void setNodeInternal(Node *node, uint8_t axis) override;

};

#endif // NODESCREENBCU_H
