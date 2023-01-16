#ifndef PDOMAPPINGVIEW_H
#define PDOMAPPINGVIEW_H

#include <QWidget>

#include "nodeobjectid.h"

class PDO;
class PDOMappingPainter;

class PDOMappingView : public QWidget
{
    Q_OBJECT
public:
    explicit PDOMappingView(QWidget *parent = nullptr);

    PDO *pdo() const;
    const QList<NodeObjectId> &nodeListMapping() const;
    void setNodeListMapping(const QList<NodeObjectId> &nodeListMapping);

    const QList<QString> &nodeListName() const;
    void setNodeListName(const QList<QString> &nodeListName);

    const QList<QColor> &nodeListColor() const;
    void setNodeListColor(const QList<QColor> &nodeListColor);

    int objIdAtPos(const QPoint &pos);

public slots:
    void setPdo(PDO *pdo);

public:
    int heightForWidth(int width) const override;
    bool hasHeightForWidth() const override;
    QSize minimumSizeHint() const override;

protected slots:
    void updateMapping();
    void updateEnabled(bool enabled);

protected:
    PDO *_pdo;
    bool _enabled;

    QList<NodeObjectId> _nodeListMapping;
    QList<QString> _nodeListName;
    QList<QColor> _nodeListColor;

    int _dragBitPos;
    NodeObjectId _dragObjId;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;

    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

    // QObject interface
public:
    bool event(QEvent *event) override;

};

#endif // PDOMAPPINGVIEW_H
