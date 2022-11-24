#ifndef PDOMAPPINGPAINTER_H
#define PDOMAPPINGPAINTER_H

#include <QPainter>
#include "services/pdo.h"

class PDOMappingPainter : public QPainter
{
public:
    PDOMappingPainter(QWidget *widget);

    static int bitFromX(const QRect &rect, double x);

    void drawDragPos(const QRect &rect, double pos);

    void drawListMapping(const QRect &rect,
                         const QList<NodeObjectId> &nodeListMapping,
                         const QList<QString> &nodeListName = QList<QString>(),
                         const QList<QColor> &nodeListColor = QList<QColor>(),
                         bool enabled = true);

    void drawMapping(const QRect &objRect, const NodeObjectId &nodeObjectId, const QString &nodeName, const QColor &nodeColor);

    static int objIdAtPos(const QRect &rect, const QList<NodeObjectId> &nodeListMapping, const QPoint &pos);

protected:
    QWidget *_widget;
};

#endif // PDOMAPPINGPAINTER_H
