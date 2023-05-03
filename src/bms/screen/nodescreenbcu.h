#ifndef NODESCREENBCU_H
#define NODESCREENBCU_H

#include "../lib/udtgui/screen/nodescreen.h"
#include "canopen/nodeod/nodeodwidget.h"
#include "system/bmsstack.h"

class AbstractIndexWidget;
class QLabel;
class IndexLabel;
class IndexComboBox;
class QGroupBox;
class IndexFocuseditor;

class NodeScreenBCU : public NodeScreen
{
    Q_OBJECT
public:
    NodeScreenBCU();

public:
    void resetHardware();
    BCU *bcu() const;

public slots:
    void setBCU(BCU *bcu);

protected:
    void createWidgets();
    QWidget *createSumaryWidget();
    QLabel *_summaryIconLabel;
    QLabel *_summaryProfileLabel;

    QWidget *createInfoWidget();

    QWidget *createStatusWidget();


    QList<AbstractIndexWidget *> _indexWidgets;
    void updateInfos(BCU *bcu);
private:
    QWidget *createControlWidget();

private slots:
    void startBCU();
    void setCanState();
    void saveParam();
    void loadDefault();
    void logIn();
public:
    QString title() const override;
    void setNodeInternal(Node *node, uint8_t axis) override;


private:
    BCU *_bcu;
    QGroupBox *_groupBCU;
   // QList<IndexFocuseditor*> _indexEditors;
};

#endif // NODESCREENBCU_H
