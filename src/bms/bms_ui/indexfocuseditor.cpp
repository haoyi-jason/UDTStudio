#include "indexfocuseditor.h"

IndexFocuseditor::IndexFocuseditor(const NodeObjectId &objId)
{
    setObjId(objId);

    _widget = this;
}

void IndexFocuseditor::setTextEditValue(const QVariant &value)
{

}

QVariant IndexFocuseditor::textEditValue() const
{

}

void IndexFocuseditor::setDisplayValue(const QVariant &value, DisplayAttribute flats)
{
    if (flags == DisplayAttribute::Error)
    {
        QFont mfont = font();
        mfont.setItalic(true);
        lineEdit()->setFont(mfont);
    }
    else
    {
        QFont mfont = font();
        mfont.setItalic(false);
        lineEdit()->setFont(mfont);
    }
    setTextEditValue(value);
}

bool IndexFocuseditor::isEditing() const
{
    return lineEdit()->hasFocus();
}

void IndexFocuseditor::updateHint()
{

}

void IndexFocuseditor::keyPressEvent(QKeyEvent *event)
{

}

void IndexFocuseditor::focusOutEvent(QFocusEvent *event)
{

}

void IndexFocuseditor::mousePressEvent(QMouseEvent *event)
{

}

void IndexFocuseditor::mouseMoveEvent(QMouseEvent *event)
{

}

void IndexFocuseditor::contextMenuEvent(QContextMenuEvent *event)
{

}
