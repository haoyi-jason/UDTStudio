#include "focusededitor.h"
#include <QEvent>
#include <QDebug>
#include <QMouseEvent>

FocusedEditor::FocusedEditor(QWidget *parent)
    :QLineEdit(parent)
//    :FocusedEditor(INT_MIN,INT_MAX,parent)
{
    setAlignment(Qt::AlignRight);
    _max = INT_MAX;
    _min = INT_MIN;
}

//FocusedEditor::FocusedEditor(int min, int max, QWidget *parent)
//    :QLineEdit(parent)
//{

//}

void FocusedEditor::setPasswordMode(bool on)
{
    setEchoMode(on?QLineEdit::Password:QLineEdit::Normal);
}

bool FocusedEditor::event(QEvent *e)
{
//    qDebug()<<Q_FUNC_INFO;
    QLineEdit::event(e);
    if(e->type() == QEvent::MouseButtonRelease){
        //emit focused(true);
        focusInternal();
    }
}

void FocusedEditor::focusInternal()
{
    NumberPad *nPad = new NumberPad(this);

    QString str = text();
    nPad->setText(str);
    nPad->setMax(_max);
    nPad->setMin(_min);
    nPad->passwordMode(echoMode()==QLineEdit::Password);
    //nPad->move(nPad->x() + (nPad->width()-width())/2, nPad->y()+(nPad->height() - height())/2);
    //connect(nPad,&NumberPad::accepted,this,&FocusedEditor::accepted);
    //nPad->showFullScreen();
    if(nPad->exec() == QDialog::Accepted){
        setText(nPad->result());
        textEdited();
        emit edited();
    }
}

void FocusedEditor::textEdited()
{

}

void FocusedEditor::accepted()
{

}

void FocusedEditor::rejected()
{

}

void FocusedEditor::mousePressEvent(QMouseEvent *e)
{
    e->accept();
}

void FocusedEditor::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
}
