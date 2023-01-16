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

#ifndef BMS_DATALOGGERMANAGERWIDGET_H
#define BMS_DATALOGGERMANAGERWIDGET_H


#include <QWidget>

#include <QSpinBox>
#include <QToolBar>

#include "bms_dataloggerchartswidget.h"
#include "bms_dataloggertreeview.h"

class BMS_DataLoggerManagerWidget : public QWidget
{
    Q_OBJECT
public:
    BMS_DataLoggerManagerWidget(DataLogger *logger, QWidget *parent = nullptr);

    BMS_DataLoggerChartsWidget *chartWidget() const;
    void setChartWidget(BMS_DataLoggerChartsWidget *chartWidget);

    QAction *startStopAction() const;

protected slots:
    void toggleStartLogger(bool start);
    void setLogTimerMs(int ms);

    void setUseOpenGL(bool useOpenGL);
    void setViewCross(bool viewCross);

    void setRollingEnabled(bool enabled);
    void setRollingTimeMs(int ms);

    void takeScreenShot();
    void exportAllCSVData();

protected:
    BMS_DataLogger *_logger;
    BMS_DataLoggerChartsWidget *_chartWidget;

    void createWidgets();
    QToolBar *_toolBar;
    BMS_DataLoggerTreeView *_dataLoggerTreeView;

    QSpinBox *_logTimerSpinBox;
    QAction *_startStopAction;
    QAction *_openGLAction;
    QAction *_crossAction;
    QAction *_rollAction;
    QSpinBox *_rollingTimeSpinBox;
    QAction *_exportCSVAction;
    QAction *_screenShotAction;
};

#endif  // DATALOGGERMANAGERWIDGET_H
