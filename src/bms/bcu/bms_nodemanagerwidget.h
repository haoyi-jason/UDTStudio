#ifndef BMS_NODEMANAGERWIDGET_H
#define BMS_NODEMANAGERWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>
#include <QPushButton>

#include "node.h"
#include "system/bmsstack.h"
#include "../bms_ui/focusededitor.h"

class BMS_NodeManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BMS_NodeManagerWidget(QWidget *parent = nullptr);
    BMS_NodeManagerWidget(Node *node, QWidget *parent = nullptr);

    Node *node() const;

    QAction *actionPreop() const;
    QAction *actionStart() const;
    QAction *actionStop() const;
    QAction *actionResetCom() const;
    QAction *actionReset() const;

    QAction *actionLoadEds() const;
    QAction *actionReLoadEds() const;

    QAction *actionRemoveNode() const;
    QAction *actionUpdateFirmware() const;

    QAction *actionSelect() const;

public slots:
    void setNode(Node *node);
    void setBCU(BCU *bcu);
    void updateData();

    void preop();
    void start();
    void stop();
    void resetCom();
    void resetNode();
    void select();

    void removeNode();
    void updateNodeFirmware();

    void loadEds(const QString &edsFileName = QString());
    void reloadEds();

    void updateThreadState(bool state);
    void pollNode();

    void addNode(Node *node);

    void nodeStatusChanged(Node::Status status);

protected slots:
    void setNodeName();

protected:
    void createWidgets();
    QToolBar *_toolBar;
    QActionGroup *_groupNmt;
    QAction *_actionPreop;
    QAction *_actionStart;
    QAction *_actionStop;
    QAction *_actionResetCom;
    QAction *_actionReset;
    QAction *_actionSelect;
    QGroupBox *_groupBox;

    QLineEdit *_nodeNameEdit;

    QAction *_actionLoadEds;
    QAction *_actionReLoadEds;

    QAction *_actionRemoveNode;

    QAction *_actionUpdateFirmware;

    Node *_node;
    BCU *_bcu;

    QAction *_actionStartPoll;

    QList<BCU*> _bcus;
    QList<QPushButton *> _bcuButtons;
    QGroupBox *_bcuGroup;
    QLabel *_statusLabel;

    FocusedEditor *_feInterval;
signals:
    void nodeSelected(BCU *bcu);
};

#endif // BMS_NODEMANAGERWIDGET_H
