#ifndef BMSSTACKMODEL_H
#define BMSSTACKMODEL_H

#include <QAbstractItemModel>
#include "bmsstack.h"
#include "canopen.h"

class BMSStackModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    BMSStackModel(QObject *parent = nullptr);
    BMSStackModel(CanOpen *canOpen, QObject *parent = nullptr);
    ~BMSStackModel() override;

    CanOpen *canOpen() const;
    void setCanOpen(CanOpen *canOpen);

    CanOpenBus *bus(const QModelIndex &index) const;
    Node *node(const QModelIndex &index) const;

    enum Column{
        StackId,
        Name,
        Status,
        ColumnCount
    };

protected:
    void prepareAddBus(quint8 busId);
    void addBus(quint8 busId);
    void removeBus(quint8 busId);
    void updateBus(CanOpenBus *bus, quint8 column);

    void prepareAddNode(CanOpenBus *bus, quint8 nodeId);
    void addNode(CanOpenBus *bus, quint8 nodeId);
    void removeNode(CanOpenBus *bus, quint8 nodeId);
    void updateNode(Node *node, quint8 column);

public:
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    CanOpen *_canOpen;
};

#endif // BMSSTACKMODEL_H
