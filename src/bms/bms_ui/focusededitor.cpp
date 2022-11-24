#include "focusededitor.h"
#include <QEvent>
#include <QDebug>

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
    NumberPad *nPad = new NumberPad();
    QString str = text();
    nPad->setText(str);
    nPad->setMax(_max);
    nPad->setMin(_min);

    //connect(nPad,&NumberPad::accepted,this,&FocusedEditor::accepted);
    if(nPad->exec() == QDialog::Accepted){
        setText(nPad->result());
    }
}

void FocusedEditor::accepted()
{

}

void FocusedEditor::rejected()
{

}
