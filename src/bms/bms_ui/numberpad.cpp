#include "numberpad.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QSize>

Button::Button(const QString &text, QWidget *parent)
    :QToolButton(parent)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    setText(text);
}

QSize Button::sizeHint() const
{
    QSize size = QToolButton::sizeHint();

    size.rheight() += 20;
    size.rwidth() = qMax(size.width(),size.height());

    return size;
}



NumberPad::NumberPad(QWidget *parent)
    :QDialog(parent)
{
    _checkInput = false;
    _passChar = false;
    _isNumber = true;
    _maxInput = INT_MAX;
    _minInput = INT_MIN;

    _inBox = new QLineEdit();
    _inBox->setReadOnly(true);
    _inBox->setAlignment(Qt::AlignRight);
    _inBox->setMaxLength(16);

    QFont f = _inBox->font();
    f.setPointSize(f.pointSize() + 16);
    f.setBold(true);
    _inBox->setFont(f);

    for(int i=0;i<10;i++){
        Button *btn =new Button(QString::number(i));
        connect(btn,&Button::clicked,this,&NumberPad::padClicked);
        _buttons.append(btn);
    }

    Button *dot = new Button(".");
    connect(dot,&Button::clicked,this,&NumberPad::padClicked);
    Button *ok = new Button("OK");
    connect(ok,&Button::clicked,this,&NumberPad::padClicked);
    Button *can = new Button("CAN");
    connect(can,&Button::clicked,this,&NumberPad::padClicked);
//    Button *clr = new Button("CLR");
    Button *clr = new Button("+/-");
    connect(clr,&Button::clicked,this,&NumberPad::padClicked);
    Button *bck = new Button("<-");
    connect(bck,&Button::clicked,this,&NumberPad::padClicked);

    _hintText = new QLabel("MIN:0, MAX:9999");

    QGridLayout *ml = new QGridLayout;
    ml->setSizeConstraint(QLayout::SetFixedSize);
    ml->addWidget(_inBox,0,0,1,3);
    ml->addWidget(_hintText,1,0,1,3);
    ml->addWidget(bck,2,0);
    ml->addWidget(clr,2,1);
    ml->addWidget(can,2,2);

    for(int i=1;i<_buttons.size();i++){
        int r = ((9-i)/3)+3;
        int c = ((i-1)%3);
        ml->addWidget(_buttons[i],r,c);
    }
    ml->addWidget(_buttons[0],6,0);
    ml->addWidget(dot,6,1);
    ml->addWidget(ok,6,2);

    setLayout(ml);
    setWindowTitle("Input");

}

void NumberPad::padClicked()
{
    Button *btn = (Button*)sender();
    QString str = _inBox->text();
    QString key = btn->text();
    switch(key_map.value(key)){
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        if(_inBox->selectedText().size()){
            str = btn->text();
        }
        else{
            str += btn->text();
        }
        if(_isNumber){
            int v = str.toInt();
            if((v <= _maxInput) || (v >= _minInput )){
                _inBox->setText(str);
            }
        }
        else{
            _inBox->setText(str);
        }
        break;
    case 10:
        str += btn->text();
        break;
    case 11:
        if(_checkInput){
            int v = _inBox->text().toInt();
            if((v <= _maxInput) && (v >= _minInput)){
                _resultString = _inBox->text();
                emit result(_inBox->text());
                emit accept();
            }
            else{

            }
        }
        else{
            _resultString = _inBox->text();
            emit result(_inBox->text());
            emit accept();
        }
        close();
        break;
    case 12:
        emit reject();
        close();
        break;
    case 13:
        str = str.left(str.size()-1);
        break;
    case 14:
        if(str.left(1)=="-"){
            str = str.right(str.size()-1);
        }
        else{
            str = "-"+str;
        }
        break;
    default:break;
    }

    if(_passChar){
        int n = str.length();
        QString b("*");
        b.repeated(n);
        _inBox->setText(b);
    }
    else{
        _inBox->setText(str);
    }
}

void NumberPad::setText(QString &content)
{
    _inBox->setText(content);
    _inBox->selectAll();
}

void NumberPad::setMin(int value)
{
    _minInput = value;
}

void NumberPad::setMax(int value)
{
    _maxInput = value;
}

void NumberPad::passwordMode(bool on)
{
    _passChar = on;
}

void NumberPad::validInput(bool on)
{
    _checkInput = on;
}

QString NumberPad::result()
{
    return _resultString;
}













