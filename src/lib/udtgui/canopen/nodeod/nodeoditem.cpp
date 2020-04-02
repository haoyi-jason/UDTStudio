/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/

#include "nodeoditem.h"

#include "nodeoditemmodel.h"

NodeOdItem::NodeOdItem(NodeOd *od, NodeOdItem *parent)
{
    _type = TOD;
    _od = od;
    _index = nullptr;
    _subIndex = nullptr;
    _parent = parent;
    createChildren();
}

NodeOdItem::NodeOdItem(NodeIndex *index, NodeOdItem *parent)
{
    _type = TIndex;
    _od = nullptr;
    _index = index;
    _subIndex = nullptr;
    _parent = parent;
    createChildren();
}

NodeOdItem::NodeOdItem(NodeSubIndex *subIndex, NodeOdItem *parent)
{
    _type = TSubIndex;
    _od = nullptr;
    _index = nullptr;
    _subIndex = subIndex;
    _parent = parent;
}

NodeOdItem::~NodeOdItem()
{
    qDeleteAll(_children);
}

NodeOdItem::Type NodeOdItem::type() const
{
    return _type;
}

NodeOd *NodeOdItem::od() const
{
    return _od;
}

NodeIndex *NodeOdItem::index() const
{
    return _index;
}

NodeSubIndex *NodeOdItem::subIndex() const
{
    return _subIndex;
}

int NodeOdItem::rowCount() const
{
    switch (_type)
    {
    case NodeOdItem::TOD:
        return _od->indexCount();

    case NodeOdItem::TIndex:
        if (_index->objectType() == NodeIndex::VAR)
        {
            return 0;
        }
        else
        {
            return _index->subIndexes().count();
        }

    default:
        return 0;
    }
}

QVariant NodeOdItem::data(int column, int role) const
{
    switch (_type)
    {
    case NodeOdItem::TOD:
        break;

    case NodeOdItem::TIndex:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case NodeOdItemModel::OdIndex:
                return QVariant(QLatin1String("0x") + QString::number(_index->index(), 16).toUpper());

            case NodeOdItemModel::Name:
                return QVariant(_index->name());

            case NodeOdItemModel::Type:
                if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1)
                {
                    return QVariant(NodeSubIndex::dataTypeStr(_index->subIndex(0)->dataType()));
                }
                else
                {
                    return QVariant(NodeIndex::objectTypeStr(_index->objectType()));
                }

            case NodeOdItemModel::Value:
                if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1)
                {
                    return _index->subIndex(0)->value();
                }
                else
                {
                    return QVariant(QString("%1 items").arg(_index->subIndexesCount()));
                }
            default:
                return QVariant();
            }
        }
        break;

    case NodeOdItem::TSubIndex:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case NodeOdItemModel::OdIndex:
                return QVariant(QLatin1String("0x") + QString::number(_subIndex->subIndex(), 16).toUpper());

            case NodeOdItemModel::Name:
                return QVariant(_subIndex->name());

            case NodeOdItemModel::Type:
                return QVariant(NodeSubIndex::dataTypeStr(_subIndex->dataType()));

            case NodeOdItemModel::Value:
                return _subIndex->value();

            default:
                return QVariant();
            }
        }
        break;
    }
    return QVariant();
}

bool NodeOdItem::setData(int column, const QVariant &value, int role)
{
    // TODO IMPLEMENT ME to set value
    return false;
}

NodeOdItem *NodeOdItem::parent() const
{
    return _parent;
}

NodeOdItem *NodeOdItem::child(int id) const
{
    NodeOdItem *child;
    if (id < 0 || id >= _children.count())
    {
        return nullptr;
    }
    child = _children.at(id);
    return child;
}

int NodeOdItem::row() const
{
    if (!_parent)
    {
        return 0;
    }
    return _parent->_children.indexOf(const_cast<NodeOdItem *>(this));
}

const QList<NodeOdItem *> &NodeOdItem::children() const
{
    return _children;
}

void NodeOdItem::createChildren()
{
    switch (_type)
    {
    case NodeOdItem::TOD:
        for (NodeIndex *index : _od->indexes())
        {
            _children.append(new NodeOdItem(index, this));
        }
        break;

    case NodeOdItem::TIndex:
        for (NodeSubIndex *subIndex : _index->subIndexes())
        {
            _children.append(new NodeOdItem(subIndex, this));
        }
        break;

    default:
        break;
    }
}