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

#include "bms_dataloggerwidget.h"

#include "bms_dataloggersingleton.h"

#include <QHBoxLayout>
#include <QSplitter>

BMS_DataLoggerWidget::BMS_DataLoggerWidget(QWidget *parent)
    : BMS_DataLoggerWidget(nullptr, Qt::Horizontal, parent)
{
}

BMS_DataLoggerWidget::BMS_DataLoggerWidget(DataLogger *dataLogger, Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
    , _dataLogger(dataLogger)
{
    _type = UserType;

    if (_dataLogger == nullptr)
    {
        _dataLogger = new DataLogger(this);
    }
    createWidgets(orientation);
    DataLoggerSingleton::addLogger(this);
}

BMS_DataLoggerWidget::~BMS_DataLoggerWidget()
{
    DataLoggerSingleton::removeLogger(this);
}

DataLogger *BMS_DataLoggerWidget::dataLogger() const
{
    return _dataLogger;
}

const QString &BMS_DataLoggerWidget::title() const
{
    return _title;
}

void BMS_DataLoggerWidget::setTitle(const QString &title)
{
    _title = title;
    setWindowTitle(title);
}

BMS_DataLoggerWidget::Type BMS_DataLoggerWidget::type() const
{
    return _type;
}

void BMS_DataLoggerWidget::setType(Type type)
{
    _type = type;
}

void BMS_DataLoggerWidget::createWidgets(Qt::Orientation orientation)
{
    QLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);

    QSplitter *splitter = new QSplitter();
    splitter->setStyleSheet("QSplitter {background: #19232D;}");

    QWidget *widgetLogger = new QWidget();
    QVBoxLayout *layoutLogger = new QVBoxLayout();
    _chartView = new BMS_DataLoggerChartsWidget(_dataLogger);

    _dataLoggerManagerWidget = new BMS_DataLoggerManagerWidget(_dataLogger);
    _dataLoggerManagerWidget->setChartWidget(_chartView);

    splitter->setOrientation(orientation);
    switch (orientation)
    {
        case Qt::Horizontal:
            splitter->addWidget(_dataLoggerManagerWidget);
            splitter->addWidget(widgetLogger);
            splitter->setSizes({70, 130});
            layoutLogger->setContentsMargins(2, 2, 2, 2);
            _dataLoggerManagerWidget->layout()->setContentsMargins(2, 2, 2, 2);
            break;

        case Qt::Vertical:
            splitter->addWidget(widgetLogger);
            splitter->addWidget(_dataLoggerManagerWidget);
            splitter->setSizes({200, 50});
            layoutLogger->setContentsMargins(0, 0, 0, 4);
            _dataLoggerManagerWidget->layout()->setContentsMargins(0, 4, 0, 0);
            break;
    }
    layoutLogger->addWidget(_chartView);
    widgetLogger->setLayout(layoutLogger);

    layout->addWidget(splitter);
    setLayout(layout);
}

BMS_DataLoggerChartsWidget *BMS_DataLoggerWidget::chartView() const
{
    return _chartView;
}

BMS_DataLoggerManagerWidget *BMS_DataLoggerWidget::managerWidget() const
{
    return _dataLoggerManagerWidget;
}
