/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2021 UniSwarm
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

#include "bms_dataloggersingleton.h"

#include <QMenu>
#include <QStackedWidget>
#include <QTabWidget>

BMS_DataLoggerSingleton *BMS_DataLoggerSingleton::_instance = nullptr;

const QList<BMS_DataLoggerWidget *> &BMS_DataLoggerSingleton::dataLoggerWidgets()
{
    return instance()->_dataLoggerWidgets;
}

void BMS_DataLoggerSingleton::addLogger(BMS_DataLoggerWidget *logger)
{
    instance()->_dataLoggerWidgets.append(logger);
    emit _instance->listChanged();
}

void BMS_DataLoggerSingleton::removeLogger(BMS_DataLoggerWidget *logger)
{
    if (instance()->_dataLoggerWidgets.removeOne(logger))
    {
        emit _instance->listChanged();
    }
}

QMenu *BMS_DataLoggerSingleton::loggersMenu()
{
    return instance()->_loggersMenu;
}

QMenu *BMS_DataLoggerSingleton::createAddToLoggerMenu(const NodeObjectId &objId)
{
    return instance()->createAddToLoggerMenu(QList<NodeObjectId>() << objId);
}

QMenu *BMS_DataLoggerSingleton::createAddToLoggerMenu(const QList<NodeObjectId> &objIds)
{
    QMenu *menu = new QMenu(tr("Add to logger..."));
    BMS_DataLoggerSingleton *instance = BMS_DataLoggerSingleton::instance();

    for (BMS_DataLoggerWidget *loggerWidget : qAsConst(instance->_dataLoggerWidgets))
    {
        QAction *addAction = nullptr;
        switch (loggerWidget->type())
        {
            case BMS_DataLoggerWidget::UserType:
            case BMS_DataLoggerWidget::DockType:
                addAction = menu->addAction(loggerWidget->title());
                break;

            case BMS_DataLoggerWidget::InternalType:
                break;
        }
        if (addAction != nullptr)
        {
            connect(addAction,
                    &QAction::triggered,
                    instance,
                    [=]()
                    {
                        loggerWidget->dataLogger()->addData(objIds);
                        showWidgetRecursive(loggerWidget);
                    });
        }
    }

    menu->addSeparator();
    QAction *addNewAction = menu->addAction(tr("Add to new logger"));
    connect(addNewAction,
            &QAction::triggered,
            instance,
            [=]()
            {
                BMS_DataLoggerWidget *newLogger = instance->createNewLoggerWindow();
                newLogger->dataLogger()->addData(objIds);
            });

    return menu;
}

BMS_DataLoggerSingleton::BMS_DataLoggerSingleton()
{
    _loggersMenu = new QMenu(tr("Data loggers"));
    connect(_loggersMenu, &QMenu::aboutToShow, this, &DataLoggerSingleton::updateLoggersMenu);
    _loggerWindowCount = 0;
}

BMS_DataLoggerSingleton::~BMS_DataLoggerSingleton()
{
    delete _loggersMenu;
}

void BMS_DataLoggerSingleton::showWidgetRecursive(QWidget *widget)
{
    if (widget->parentWidget())
    {
        BMS_DataLoggerSingleton::showWidgetRecursive(widget->parentWidget());
        QStackedWidget *stackWidget = qobject_cast<QStackedWidget *>(widget->parentWidget());
        if (stackWidget != nullptr)
        {
            QTabWidget *tabWidget = qobject_cast<QTabWidget *>(stackWidget->parentWidget());
            if (tabWidget)
            {
                tabWidget->setCurrentWidget(widget);
            }
        }
    }
    widget->raise();
    widget->show();
    widget->activateWindow();
}

void BMS_DataLoggerSingleton::updateLoggersMenu()
{
    _loggersMenu->clear();

    QAction *newLoggerAction = _loggersMenu->addAction(tr("Create a new logger window"));
    connect(newLoggerAction, &QAction::triggered, this, &BMS_DataLoggerSingleton::createNewLoggerWindow);

    QAction *stopAllAction = _loggersMenu->addAction(tr("Stop all"));
    connect(stopAllAction, &QAction::triggered, this, &BMS_DataLoggerSingleton::stopAll);

    _loggersMenu->addSection(tr("Loggers list"));
    for (BMS_DataLoggerWidget *loggerWidget : qAsConst(_dataLoggerWidgets))
    {
        QString actionText;
        QAction *showAction = nullptr;
        switch (loggerWidget->type())
        {
            case BMS_DataLoggerWidget::UserType:
            case BMS_DataLoggerWidget::DockType:
                actionText = loggerWidget->title();
                if (loggerWidget->dataLogger()->isStarted())
                {
                    actionText.append(tr(" (Running)"));
                }
                showAction = _loggersMenu->addAction(actionText);
                // action
                break;

            case BMS_DataLoggerWidget::InternalType:
                if (loggerWidget->dataLogger()->isStarted())
                {
                    showAction = _loggersMenu->addAction(loggerWidget->title() + tr(" (Running)"));
                    // action
                }
                break;
        }
        if (showAction != nullptr)
        {
            connect(showAction,
                    &QAction::triggered,
                    this,
                    [=]()
                    {
                        showWidgetRecursive(loggerWidget);
                    });
        }
    }
}

void BMS_DataLoggerSingleton::stopAll()
{
    for (BMS_DataLoggerWidget *logger : qAsConst(_dataLoggerWidgets))
    {
        logger->dataLogger()->stop();
    }
}

BMS_DataLoggerWidget *BMS_DataLoggerSingleton::createNewLoggerWindow()
{
    BMS_DataLogger *dataLogger = new BMS_DataLogger();
    BMS_DataLoggerWidget *dataLoggerWidget = new BMS_DataLoggerWidget(dataLogger);
    dataLoggerWidget->setTitle(tr("Logger window %1").arg(_loggerWindowCount++));

    dataLoggerWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(dataLoggerWidget, &QObject::destroyed, dataLogger, &DataLogger::deleteLater);

    dataLoggerWidget->show();
    dataLoggerWidget->raise();
    dataLoggerWidget->activateWindow();

    return dataLoggerWidget;
}
