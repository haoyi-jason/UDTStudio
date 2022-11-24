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

#ifndef BMS_CANFRAMEVIEW_H
#define BMS_CANFRAMEVIEW_H


#include "busdriver/qcanbusframe.h"
#include <QAction>
#include <QTableView>

#include "bms_canframemodel.h"

class BMS_CanFrameListView : public QTableView
{
    Q_OBJECT
public:
    BMS_CanFrameListView(QWidget *parent = nullptr);
    ~BMS_CanFrameListView() override;

    CanOpenBus *bus() const;
    void setBus(CanOpenBus *bus);

    QAction *clearAction() const;
    QAction *copyAction() const;

public slots:
    void appendCanFrame(const QCanBusFrame &frame);
    void clear();
    void copy();

protected slots:
    void updateSelect(const QItemSelection &selected, const QItemSelection &deselected);

protected:
    BMS_CanFrameModel *_canModel;

    // Actions
    void createActions();
    QAction *_clearAction;
    QAction *_copyAction;

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif  // CANFRAMEVIEW_H
