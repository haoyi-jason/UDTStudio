/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#ifndef DCFPARSER_H
#define DCFPARSER_H

#include "od_global.h"

#include <QString>
#include <QMap>
#include <QSettings>
#include <QVariant>

#include "parser.h"
#include "model/od.h"

class OD_EXPORT DcfParser : public Parser
{
public:
    DcfParser();

    OD *parse(const QString &path) const;

private:
    DataStorage readData(const QSettings &dcf) const;
    void readFileInfo(OD *od, const QSettings &dcf) const;
    uint8_t readPdoMapping(const QSettings &dcf) const;
    QVariant readLowLimit(const QSettings &dcf) const;
    QVariant readHighLimit(const QSettings &dcf) const;

};

#endif // DCFPARSER_H
