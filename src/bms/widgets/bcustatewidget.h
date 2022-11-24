#ifndef BCUSTATEWIDGET_H
#define BCUSTATEWIDGET_H

#include <QWidget>

#include "node.h"
#include "nodeobjectid.h"
#include "nodeodsubscriber.h"

#include <QLabel>
#include "bmsstack.h"

class NodeObjectId;
class IndexLabel;

class BcuStateWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    explicit BcuStateWidget(QWidget *parent = nullptr);

signals:

public slots:
    void setBCU(BCU *bcu);

private:
    BCU *_bcu;
    QList<NodeObjectId> _objectIds;
    QList<IndexLabel*> _labels;
    IndexLabel *_socLabel;
    IndexLabel *_sohLabel;
    IndexLabel *_pvLabel;
    IndexLabel *_paLabel;

    void createWidgets();

protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
};

#endif // BCUSTATEWIDGET_H
