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

#include "nodeobjectid.h"

NodeObjectId::NodeObjectId(quint8 busId, quint8 nodeId, quint16 index, quint8 subIndex, QMetaType::Type dataType)
    : busId(busId), nodeId(nodeId), index(index), subIndex(subIndex), dataType(dataType)
{
}

NodeObjectId::NodeObjectId(quint16 index, quint8 subIndex, QMetaType::Type dataType)
    : busId(0xFF), nodeId(0xFF), index(index), subIndex(subIndex), dataType(dataType)
{
}

NodeObjectId::NodeObjectId(const NodeObjectId &other)
{
    busId = other.busId;
    nodeId = other.nodeId;
    index = other.index;
    subIndex = other.subIndex;
    dataType = other.dataType;
}

quint64 NodeObjectId::key() const
{
    quint64 key = 0;
    key += static_cast<quint64>(busId) << 24;
    key += static_cast<quint64>(nodeId) << 16;
    key += static_cast<quint64>(index) << 8;
    key += static_cast<quint64>(subIndex);
    return key;
}

bool NodeObjectId::isValid() const
{
    if (busId == 0xFF && nodeId == 0xFF)
    {
        return true;
    }
    return false;
}

bool NodeObjectId::isNodeIndependant() const
{
    if (busId == 0xFF && nodeId == 0xFF)
    {
        return true;
    }
    return false;
}

bool NodeObjectId::isABus() const
{
    if (busId != 0xFF && nodeId == 0xFF && index == 0xFFFF && subIndex == 0xFF)
    {
        return true;
    }
    return false;
}

bool NodeObjectId::isANode() const
{
    if (busId != 0xFF && nodeId != 0xFF && index == 0xFFFF && subIndex == 0xFF)
    {
        return true;
    }
    return false;
}

bool NodeObjectId::isAnIndex() const
{
    if (index != 0xFFFF && subIndex == 0xFF)
    {
        return true;
    }
    return false;
}

bool NodeObjectId::isASubIndex() const
{
    if (index != 0xFFFF && subIndex != 0xFF)
    {
        return true;
    }
    return false;
}

NodeObjectId &NodeObjectId::operator=(const NodeObjectId &other)
{
    busId = other.busId;
    nodeId = other.nodeId;
    index = other.index;
    subIndex = other.subIndex;
    dataType = other.dataType;
    return *this;
}

bool operator==(const NodeObjectId &a, const NodeObjectId &b)
{
    return (a.busId == b.busId
           && a.nodeId == b.nodeId
           && a.index == b.index
           && a.subIndex == b.subIndex);
}
