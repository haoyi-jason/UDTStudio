/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
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

#include "modepp.h"
#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

enum ControlWordIP : quint16
{
    CW_PP_NewSetPoint = 0x10,
    CW_PP_ChangeSetImmediately = 0x20,
    CW_PP_AbsRel = 0x40,
    CW_PP_ChangeOnSetPoint = 0x200,
    CW_Mask = 0x270
};

ModePp::ModePp(NodeProfile402 *nodeProfile402)
    : Mode(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_SET_POINT, _axisId);
    _targetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    _mode = NodeProfile402::OperationMode::PP;
    // TODO : redesign the process for default value witg setCwDefaultflag()
    _cmdControlWordFlag = CW_PP_NewSetPoint;
}

void ModePp::newSetPoint(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_PP_NewSetPoint;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_PP_NewSetPoint);
    }

    quint16 cw = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _node->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModePp::isNewSetPoint()
{
    return (_cmdControlWordFlag & CW_PP_NewSetPoint) >> 4;
}

void ModePp::setChangeSetImmediately(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_PP_ChangeSetImmediately;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_PP_ChangeSetImmediately);
    }
    quint16 cw = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _node->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModePp::isChangeSetImmediately()
{
    return (_cmdControlWordFlag & CW_PP_ChangeSetImmediately) >> 5;
}

bool ModePp::isAbsRel()
{
    return (_cmdControlWordFlag & CW_PP_AbsRel) >> 6;
}

void ModePp::setChangeOnSetPoint(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_PP_ChangeOnSetPoint;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_PP_ChangeOnSetPoint);
    }
    quint16 cw = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _node->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModePp::isChangeOnSetPoint()
{
    return (_cmdControlWordFlag & CW_PP_ChangeOnSetPoint) >> 9;
}

void ModePp::setAbsRel(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_PP_AbsRel;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_PP_AbsRel);
    }
    quint16 cw = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _node->writeObject(_controlWordObjectId, QVariant(cw));
}

void ModePp::setTarget(qint32 target)
{
    _node->writeObject(_targetObjectId, QVariant(target));
}

quint16 ModePp::getSpecificCwFlag()
{
    return _cmdControlWordFlag & CW_Mask;
}

void ModePp::setCwDefaultflag()
{
    _cmdControlWordFlag = CW_PP_NewSetPoint;
    quint16 cw = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _node->writeObject(_controlWordObjectId, QVariant(cw));
}

void ModePp::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((objId == _controlWordObjectId) && _nodeProfile402->actualMode() == _mode)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
            _cmdControlWordFlag = controlWord & CW_Mask;

            emit changeSetImmediatelyEvent((_cmdControlWordFlag & CW_PP_NewSetPoint) >> 4);
            emit changeSetImmediatelyEvent((_cmdControlWordFlag & CW_PP_ChangeSetImmediately) >> 5);
            emit absRelEvent((_cmdControlWordFlag & CW_PP_AbsRel) >> 6);
            emit changeOnSetPointEvent((_cmdControlWordFlag & CW_PP_ChangeOnSetPoint) >> 9);
        }
    }
}
