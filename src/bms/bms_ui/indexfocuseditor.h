#ifndef INDEXFOCUSEDITOR_H
#define INDEXFOCUSEDITOR_H
#include <QWidget>
#include <QLineEdit>
#include "focusededitor.h"
#include "canopen/indexWidget/abstractindexwidget.h"

class IndexFocuseditor:public FocusedEditor, public AbstractIndexWidget
{
    Q_OBJECT
public:
    IndexFocuseditor(const NodeObjectId &objId =  NodeObjectId());

protected:
    void setTextEditValue(const QVariant &value);
    QVariant textEditValue() const;

    // abstractindexwidget interface
protected:
    void setDisplayValue(const QVariant &value, DisplayAttribute flats) override;
    bool isEditing() const override;
    void updateHint() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;


};

#endif // INDEXFOCUSEDITOR_H
