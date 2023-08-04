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
#include <QTextBrowser>

class NodeObjectId;
class IndexLCDNumber;
class QGridLayout;

const QString css ="table{ \
        table-layout:fixed;\
        width:100%; \
        border:10px solid red; \
        } \
        td{ \
        border:1px solid blue; \
        overflow:hidden; \
        white-space:nowwrap; \
        text-overflow:ellipsis; \
        }";
const QString css2 = " \
                    <style> \
                    table,th,td { \
                        border: 1px solid red; \
                        border-collapse: collapse; \
                    } \
                    </style> \
                     ";
const QString css_normal =  "style='text-align:right; padding:5px;width:60px;font-size:16px;font-weight:bold;color:#ffffff;background:#888888'";
const QString css_warning = "style='text-align:right; padding:5px;font-size:16px;font-weight:bold;color:yellow;background:blue'";
const QString css_alarm =   "style='text-align:right; padding:5px;font-size:16px;font-weight:bold;color:#ffffff;background:red'";
const QString css_header =  "style='text-align:right; padding:5px;font-size:16px;font-weight:bold;color:#222222;background:#BBBBBB'";
const QString css_hwarning ="style='text-align:right; padding:5px;font-size:16px;font-weight:bold;color:#222222;background:yellow'";
const QString css_halarm =  "style='text-align:right; padding:5px;font-size:16px;font-weight:bold;color:#ffffff;background:red'";
const QString css_lwarning ="style='text-align:right; padding:5px;font-size:16px;font-weight:bold;color:yellow;background:green'";
const QString css_lalarm =  "style='text-align:right; padding:5px;font-size:16px;font-weight:bold;color:#ffffff;background:blue'";

class NodeScreenPack : public NodeScreen,public NodeOdSubscriber
{
    Q_OBJECT
public:
    NodeScreenPack(QWidget *parent = nullptr);
    BCU *bcu() const;

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
    void setBcu(BCU *bcu);
    void updateCellData();

public:
    QString title() const;
    void setNodeInternal(Node *node, uint8_t axis);
private:
    QString colorText(QString text, QString color);
    QString cellText(QString text, QString style="style = 'color:black'");
    void UpdateHeader(BCU *bcu);
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
    QLabel *_cellInfo;
    QTextBrowser *_cellInfo2;

    bool _odError;

    int _packs;
    int _cells;
    int _ntcs;

    QString _header;


protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
//    QLineEdit *_edIndex;
//    QLineEdit *_edSubIndex;
};

#endif // NODESCREENPACK_H
