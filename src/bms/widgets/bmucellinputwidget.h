#ifndef BMUCELLINPUTWIDGET_H
#define BMUCELLINPUTWIDGET_H

#include <QWidget>

#include "node.h"
#include "nodeobjectid.h"
#include "nodeodsubscriber.h"

#include <QLabel>
#include <QProgressBar>
#include <QSlider>

#include "bmsstack.h"

class IndexCheckBox;
class NodeObjectId;
class IndexLCDNumber;

class BmuCellInputWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    explicit BmuCellInputWidget(uint8_t cell, QWidget *parent = nullptr);

    void readAllObject();

    const NodeObjectId &cellObjectId() const;

public slots:
    void setBCU(BCU *bcu);

private:
    BCU *_bcu;
    uint8_t cell;
    IndexLCDNumber *_cellLcdNumber;
    NodeObjectId _cellObjectId;
    QLabel *_label;

    void createWidgets();

protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
};

#endif // BMUCELLINPUTWIDGET_H
