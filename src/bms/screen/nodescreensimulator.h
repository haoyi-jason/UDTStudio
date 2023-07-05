#ifndef NODESCREENSIMULATOR_H
#define NODESCREENSIMULATOR_H

#include "../lib/udtgui/screen/nodescreen.h"
#include "canopen/nodeod/nodeodwidget.h"
#include "system/bmsstack.h"
#include "bms_ui/focusededitor.h"

class AbstractIndexWidget;
class QLabel;
class IndexLabel;
class IndexComboBox;
class QGroupBox;
class QCheckBox;

class NodeScreenSimulator:public NodeScreen
{
    Q_OBJECT
public:
    NodeScreenSimulator();

public slots:
    void addNode(Node *node);
    void setBCU(BCU *bcu);

protected:
    void createWidgets();

private:

private slots:
    void handleNodeSelection(int index);
    void handleSetSimulation(bool set);
    void setValue();
public:
    QString title() const override;
    void setNodeInternal(Node *node, uint8_t axis) override;

private:
    BCU *_bcu;
    QList<Node*> _nodes;

    QComboBox *_cboNodes;
    QComboBox *_cboPackNumber;
    QComboBox *_cboCellNumber;
    QComboBox *_cboNtcNumber;

    FocusedEditor *_edCv,*_edCt;
    QCheckBox *_cbSimulate;
};

#endif // NODESCREENSIMULATOR_H
