#ifndef BMUCELLINPUTWIDGETS_H
#define BMUCELLINPUTWIDGETS_H

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

class BmuCellInputWidgets : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    explicit BmuCellInputWidgets(quint16 pack, quint8 cells, quint8 ntcs, QWidget *parent = nullptr);

    void readAllObject();

    const NodeObjectId &cellObjectId() const;

public slots:
    void setBCU(BCU *bcu);
    void setCellRange(qint16 low, qint16 high);
    void setNtcRange(qint16 low, qint16 high);


private:
    BCU *_bcu;
    quint8 _cells;
    quint8 _ntcs;
    quint16 _pack;
    IndexLCDNumber *_cellLcdNumber;
    QList<NodeObjectId> _objectIds;
    QList<IndexLCDNumber*> _lcdNumbers;
    QLabel *_label;

    quint8 _maxCells;
    quint8 _maxNtcs;

    void createWidgets();

protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
};

#endif // BMUCELLINPUTWIDGET_H
