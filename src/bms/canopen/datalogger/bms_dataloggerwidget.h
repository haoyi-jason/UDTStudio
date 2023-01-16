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

#ifndef BMS_DATALOGGERWIDGET_H
#define BMS_DATALOGGERWIDGET_H


#include <QWidget>

#include "datalogger/datalogger.h"
#include "bms_dataloggerchartswidget.h"
#include "bms_dataloggermanagerwidget.h"

class BMS_DataLoggerWidget : public QWidget
{
    Q_OBJECT
public:
    BMS_DataLoggerWidget(QWidget *parent = nullptr);
    BMS_DataLoggerWidget(DataLogger *dataLogger, Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = nullptr);
    ~BMS_DataLoggerWidget() override;

    DataLogger *dataLogger() const;

    const QString &title() const;
    void setTitle(const QString &title);

    enum Type
    {
        UserType,
        DockType,
        InternalType
    };
    Type type() const;
    void setType(Type type);

    BMS_DataLoggerManagerWidget *managerWidget() const;
    BMS_DataLoggerChartsWidget *chartView() const;

protected:
    DataLogger *_dataLogger;
    QString _title;
    Type _type;

    void createWidgets(Qt::Orientation orientation);
    BMS_DataLoggerManagerWidget *_dataLoggerManagerWidget;
    BMS_DataLoggerChartsWidget *_chartView;
};

#endif  // DATALOGGERWIDGET_H
