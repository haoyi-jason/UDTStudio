#include "indexfocuseditor.h"
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QRegularExpression>
#include <QDebug>

IndexFocuseditor::IndexFocuseditor(const NodeObjectId &objId)
    :AbstractIndexWidget(objId)
{
    setObjId(objId);

    _widget = this;
}

void IndexFocuseditor::setTextEditValue(const QVariant &value)
{
    this->setText(value.toString());
}

void IndexFocuseditor::textEdited()
{
    qDebug()<<Q_FUNC_INFO<<" Request write:"<<text();
    requestWriteValue(text());
}

QVariant IndexFocuseditor::textEditValue() const
{

}

void IndexFocuseditor::setDisplayValue(const QVariant &value, DisplayAttribute flags)
{
    if (flags == DisplayAttribute::Error)
    {
        QFont mfont = font();
        mfont.setItalic(true);
        setFont(mfont);

    }
    else
    {
        QFont mfont = font();
        mfont.setItalic(false);
        setFont(mfont);
    }
    setTextEditValue(value);
}

bool IndexFocuseditor::isEditing() const
{
    return hasFocus();
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
