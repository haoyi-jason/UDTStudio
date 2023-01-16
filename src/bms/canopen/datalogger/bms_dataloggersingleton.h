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

#ifndef BMS_DATALOGGERSINGLETON_H
#define BMS_DATALOGGERSINGLETON_H


#include <QObject>

#include "canopen/datalogger/bms_dataloggerchartswidget.h"

#include <QList>

class BMS_DataLoggerSingleton : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(BMS_DataLoggerSingleton)
public:
    static const QList<DataLoggerWidget *> &dataLoggerWidgets();
    static void addLogger(DataLoggerWidget *logger);
    static void removeLogger(DataLoggerWidget *logger);

    static QMenu *loggersMenu();
    static QMenu *createAddToLoggerMenu(const NodeObjectId &objId);
    static QMenu *createAddToLoggerMenu(const QList<NodeObjectId> &objIds);

    static inline BMS_DataLoggerSingleton *instance()
    {
        if (BMS_DataLoggerSingleton::_instance == nullptr)
        {
            BMS_DataLoggerSingleton::_instance = new BMS_DataLoggerSingleton();
        }
        return BMS_DataLoggerSingleton::_instance;
    }

    static inline void release()
    {
        delete BMS_DataLoggerSingleton::_instance;
    }

public slots:
    void stopAll();

signals:
    void listChanged();

protected:
    DataLoggerSingleton();
    ~DataLoggerSingleton() override;
    QList<BMS_DataLoggerWidget *> _dataLoggerWidgets;

    QMenu *_loggersMenu;
    int _loggerWindowCount;

    static BMS_DataLoggerSingleton *_instance;

    static void showWidgetRecursive(QWidget *widget);

    BMS_DataLoggerWidget *createNewLoggerWindow();

protected slots:
    void updateLoggersMenu();
};

#define dataLoggers() (BMS_DataLoggerSingleton::instance())

#endif  // DATALOGGERSINGLETON_H
