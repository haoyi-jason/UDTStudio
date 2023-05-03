#ifndef NODESCREENPACK_H
#define NODESCREENPACK_H


#include "screen/nodescreen.h"
#include "widgets/bmuwidget.h"
#include <QTableWidget>
#include "widgets/bmucellinputwidgets.h"
#include "QLineEdit"
#include "widgets/bcustatewidget.h"

#include "nodeodsubscriber.h"
#include <QGroupBox>

class NodeObjectId;
class IndexLCDNumber;
class QGridLayout;

class NodeScreenPack : public NodeScreen,public NodeOdSubscriber
{
    Q_OBJECT
public:
    NodeScreenPack(QWidget *parent = nullptr);


protected:
    BmuWidget *_bmuWidget;

    QAction *_nextPackAction;
    QAction *_prevPackAction;
    QAction *_onShotAction;
    QAction *_setIndexValue;

    void createWidgets();

private slots:
    void toggleBMUNavagate();
    void toggleOneShot();
    void refreshContent();

signals:
    void SetBmuIndex(int);
    void OneShot();

public slots:
    void loadCriteria();
    void BCUConfigReady();
    void updateBCUInfo();

public:
    QString title() const;
    void setNodeInternal(Node *node, uint8_t axis);
private:
    int _bmuIndex;
    //QList<BmuCellInputWidgets*> _bmuCellInputWidgets;
    BcuStateWidget *_stateWidget;
    IndexLCDNumber *_socLabel;
    IndexLCDNumber *_sohLabel;
    IndexLCDNumber *_pvLabel;
    IndexLCDNumber *_paLabel;

    QList<IndexLCDNumber*> _lcdNumbers;
    QList<NodeObjectId> _objectIds;

    QGroupBox *_cellGroup;
    bool _configLoaded;

    BCU *_bcu;

    QGroupBox *_infoGroup;
    QLabel *_bcuInfo;

    bool _odError;

    int _packs;
    int _cells;
    int _ntcs;


protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
//    QLineEdit *_edIndex;
//    QLineEdit *_edSubIndex;
};

#endif // NODESCREENPACK_H
