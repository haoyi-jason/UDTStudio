#ifndef BMS_BUSNODETREEVIEW_H
#define BMS_BUSNODETREEVIEW_H

#include <QTreeView>
#include "bmsstackmodel.h"

#include <QAction>
#include <QList>
#include <QSettings>
#include <QSortFilterProxyModel>

#include "canopen.h"

class BMS_BusNodeTreeView : public QTreeView
{
    Q_OBJECT
public:
    BMS_BusNodeTreeView(QWidget *parent = nullptr);
    BMS_BusNodeTreeView(CanOpen *canOpen, QWidget *parent = nullptr);
    ~BMS_BusNodeTreeView() override;

    CanOpen *canOpen() const;
    void setCanOpen(CanOpen *canOpen);

    CanOpenBus *currentBus() const;
    Node *currentNode() const;

    void addBusAction(QAction *action);
    void addNodeAction(QAction *action);

    void saveState(QSettings &settings);
    void restoreState(QSettings &settings);

signals:
    void busSelected(CanOpenBus *currentBus);
    void nodeSelected(Node *currentNode);

protected slots:
    void updateSelection();
    void indexDbClick(const QModelIndex &index);
    void addBus(quint8 id);
    void addNode(CanOpenBus *bus, quint8 id);

protected:
    void createHeader();
    BMSStackModel *_stackModel;
    QSortFilterProxyModel *_sortFilterProxyModel;

    QList<QAction *> _busActions;
    QList<QAction *> _nodeActions;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

};

#endif // BMSSTACKTREEVIEW_H
