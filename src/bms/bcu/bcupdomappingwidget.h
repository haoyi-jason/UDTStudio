#ifndef BCUPDOMAPPINGWIDGET_H
#define BCUPDOMAPPINGWIDGET_H

#include <QScrollArea>

#include "pdomappingwidget.h"

#include <QToolBar>

class BCUPDOMappingWidget:public QScrollArea
{
    Q_OBJECT
public:
    BCUPDOMappingWidget(QWidget *parent = nullptr);

    Node *node() const;

public slots:
    void setNode(Node *node);
    void readAllMapping();
    void clearAllMapping();

protected:
    void createWidgets();
    QList<PDOMappingWidget *> _tpdoMappingWidgets;
    QList<PDOMappingWidget *> _rpdoMappingWidgets;

    Node *_node;
    QToolBar *_toolBar;
    QAction *_actionReadMappings;
    QAction *_actionClearMappings;
};

#endif // BCUPDOMAPPINGWIDGET_H
