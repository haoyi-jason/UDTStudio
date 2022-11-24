#ifndef BCUMANAGERWIDGET_H
#define BCUMANAGERWIDGET_H

#include <QWidget>

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>

#include "node.h"

class BCUManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BCUManagerWidget(QWidget *parent = nullptr);
    BCUManagerWidget(Node *node, QWidget *parent = nullptr);

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

public slots:
    void setNode(Node *node);
    void updateData();

    void preop();
    void start();
    void stop();
    void resetCom();
    void resetNode();

    void removeNode();
    void updateNodeFirmware();

    void loadEds(const QString &edsFileName = QString());
    void reloadEds();

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
    QGroupBox *_groupBox;

    QLineEdit *_nodeNameEdit;

    QAction *_actionLoadEds;
    QAction *_actionReLoadEds;

    QAction *_actionRemoveNode;

    QAction *_actionUpdateFirmware;

    Node *_node;


signals:

};

#endif // BCUMANAGERWIDGET_H
