#include "login.h"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDebug>
#include <QTimer>

Login *Login::_instance = nullptr;
QMutex Login::_mutex;
bool Login::_logIn = false;

Login::Login(QWidget *parent) : QDialog(parent)
{
    createWidgets();
    _timer = new QTimer();
    _timer->setSingleShot(true);
    connect(_timer,&QTimer::timeout,this,&Login::timeout);
    setWindowTitle("帳戶切換");
}

Login::~Login(){

}

void Login::createWidgets()
{
    hide();

    QHBoxLayout *layout = new QHBoxLayout();
    QFormLayout *flayout = new QFormLayout();

    _edPassword1 = new FocusedEditor();
    _edPassword2 = new FocusedEditor();

    _edPassword1->setPasswordMode(true);
    _edPassword2->setPasswordMode(true);

    flayout->addRow(tr("請輸入密碼"),_edPassword1);

    _lbChangePasswd = new QLabel("請再次輸入密碼");
    _lbChangePasswd->hide();
    _edPassword2->hide();
    flayout->addRow(_lbChangePasswd,_edPassword2);

//    flayout->addRow(tr("請確認密碼"),_edPassword2);
    QPushButton *btnOk = new QPushButton();
    connect(btnOk,&QPushButton::clicked,this,&Login::validate);
    QPushButton *btnCancel = new QPushButton();
    connect(btnCancel,&QPushButton::clicked,this,&Login::accept);
    QPushButton *btnModifyPasswd = new QPushButton();
    connect(btnModifyPasswd,&QPushButton::clicked,this,&Login::modifyPassword);

    btnOk->setText("確定");
    btnCancel->setText("取消");
    btnModifyPasswd->setText("變更密碼");
    flayout->addWidget(btnOk);
    flayout->addWidget(btnCancel);
    flayout->addWidget(btnModifyPasswd);

    layout->addStretch(0);
    layout->addLayout(flayout);
    layout->addStretch(0);

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addStretch(0);
    vlayout->addLayout(layout);
    vlayout->addStretch(0);
    setLayout(vlayout);


}

void Login::validate()
{
    //qDebug()<<_edPassword1->text();
    _logIn = true;
    _timer->start(10*1000);
    //_timer->singleShot(600*1000,this,&Login::timeout);
    accept();
}

void Login::modifyPassword()
{
    QPushButton *btn = (QPushButton*)sender();
    if(_edPassword2->isVisible()){
        _edPassword2->hide();
        _lbChangePasswd->hide();
        btn->setText("變更密碼");
    }
    else{
        _edPassword2->show();
        _lbChangePasswd->show();
        btn->setText("取消變更密碼");
        _edPassword1->setText("");
        _edPassword1->setFocus();
    }
    accept();
}

void Login::timeout()
{
//    qDebug()<<Q_FUNC_INFO;
    _logIn = false;
    emit expired();
}

void Login::showEvent(QShowEvent *e)
{
    _edPassword1->setText("");
    _edPassword2->setText("");
    //resize(320,240);
    setGeometry(100,100,320,240);
    //move(100,100);
    //showFullScreen();
    //showEvent(e);
}
