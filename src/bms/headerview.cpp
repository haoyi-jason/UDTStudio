#include "headerview.h"
#include <QContextMenuEvent>
#include <QMenu>


HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent)
    :QHeaderView(orientation,parent)
{


}


void HeaderView::addMandatorySection(int section)
{
    _mandatorySection.insert(section);
}

void HeaderView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;

    for(int col=0;col < model()->columnCount();col++){
        QAction *action = menu.addAction(model()->headerData(col,orientation()).toString());
        action->setCheckable(true);
        action->setChecked(!isSectionHidden(col));

        if(_mandatorySection.contains(col)){
            action->setDisabled(true);
        }

        connect(action, &QAction::triggered,this,[=](){
            if(isSectionHidden(col)){
                showSection(col);
            }
            else
            {
                hideSection(col);
            }
        });
    }

    menu.exec(event->globalPos());
}
