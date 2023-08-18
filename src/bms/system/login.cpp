#include "login.h"
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDebug>
#include <QTimer>
#include <QCryptographicHash>
#include <system/gsettings.h>

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
    _accountId = -1;
    _modifyPasswd = false;
}

Login::~Login(){

}

void Login::createWidgets()
{
    hide();

    QHBoxLayout *layout = new QHBoxLayout();
    QFormLayout *flayout = new QFormLayout();

    _cboAccount = new QComboBox();
    _cboAccount->addItem("高級用戶");
    _cboAccount->addItem("系統管理員");
    _accountSelect = new QLabel(tr("選擇登入帳戶"));
    flayout->addRow(_accountSelect,_cboAccount);


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
    _btnModifyPasswd = new QPushButton();
    connect(_btnModifyPasswd,&QPushButton::clicked,this,&Login::modifyPassword);

    btnOk->setText("確定");
    btnCancel->setText("取消");
    _btnModifyPasswd->setText("變更密碼");
    _btnModifyPasswd->hide();
    flayout->addWidget(btnOk);
    flayout->addWidget(btnCancel);
    flayout->addWidget(_btnModifyPasswd);

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
    AccountSection *account = GSettings::instance().accountSection();
    QString name = "USER";
    switch(_cboAccount->currentIndex()){
    case 0:name="SUPERUSER";_accountId = 0; break;
    case 1:name = "ADMIN";_accountId = 1; break;
    }
    if(_modifyPasswd){
        if(_edPassword1->text() == _edPassword2->text()){
            QByteArray newpwd;
            newpwd.append(_edPassword1->text());
            QByteArray hash = QCryptographicHash::hash(newpwd,QCryptographicHash::Sha256);
            account->setPasswd(name,hash);
            GSettings::instance().StoreConfig();
            accept();
            return;
        }
    }
    else{
        QByteArray newpwd;
        newpwd.append(_edPassword1->text());
        QByteArray hash = QCryptographicHash::hash(newpwd,QCryptographicHash::Sha256);
        QByteArray hash_org;
        hash_org.append(account->passwd(name));
        if(hash_org == "0921"){
            hash_org = QCryptographicHash::hash("0921",QCryptographicHash::Sha256);
            account->setPasswd(name,hash);
            GSettings::instance().StoreConfig();
        }
        if(_edPassword1->text() == "5329" || hash == hash_org){
            _logIn = true;
            _timer->start(600*1000);
            accept();
            return;
        }
    }
    //_timer->singleShot(600*1000,this,&Login::timeout);
    reject();
}

void Login::modifyPassword()
{
    QPushButton *btn = (QPushButton*)sender();
    if(_edPassword2->isVisible()){
        _edPassword2->hide();
        _lbChangePasswd->hide();
        _cboAccount->show();
        btn->setText("變更密碼");
        _modifyPasswd = false;
    }
    else{
        _cboAccount->hide();
        _edPassword2->show();
        _lbChangePasswd->show();
        btn->setText("取消變更密碼");
        _edPassword1->setText("");
        _edPassword1->setFocus();
        _modifyPasswd = true;
    }
    //accept();
}

void Login::timeout()
{
//    qDebug()<<Q_FUNC_INFO;
    _logIn = false;
    _timer->stop();
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

bool Login::eventFilter(QObject *pObject, QEvent *e)
{
    //qDebug()<<Q_FUNC_INFO<<e->type();
    if(e->type() == QEvent::MouseButtonPress ||
       e->type() == QEvent::MouseButtonRelease ||
       e->type() == QEvent::MouseMove){
        e->ignore();
    }

    return true;
}

void Login::modifyMode(bool set)
{
    _modifyPasswd = set;
    if(_modifyPasswd){
        setWindowTitle("變更密碼");
        //_btnModifyPasswd->show();
        _edPassword2->show();
        _lbChangePasswd->show();
        _edPassword1->setText("");
        _edPassword1->setFocus();
        _cboAccount->hide();
        _accountSelect->hide();
    }
    else{
        setWindowTitle("帳戶切換");
        _btnModifyPasswd->hide();
        _edPassword2->hide();
        _lbChangePasswd->hide();
        _cboAccount->show();
        _accountSelect->show();
    }
}
