#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QMutex>
#include "bms_ui/focusededitor.h"
#include <QTimer>
#include <QDebug>

class QTimer;

class Login : public QDialog
{
    Q_OBJECT
public:

    inline static Login *instance(){
        if(Login::_instance == nullptr){
            QMutexLocker lock(&_mutex);
            Login::_instance = new Login();
            atexit(release);
        }
        return Login::_instance;
    }

    inline static void release(){
        if(_instance != nullptr){
            _instance->deleteLater();
            _instance = nullptr;
        }
    }

    inline static bool isValid(){
        return Login::_logIn ;
    }
    inline static void resetTimer(){
        qDebug()<<Q_FUNC_INFO;
        Login::instance()->_timer->stop();
        Login::instance()->_timer->start(600*1000);
//        Login::instance()->_timer->singleShot(600*1000,Login::instance(),&Login::timeout);

    }

signals:
    void expired();

private slots:
    void validate();
    void modifyPassword();
    void timeout();

protected:
    void showEvent(QShowEvent *e);
    bool eventFilter(QObject *pObject, QEvent *e);

private:
    Login(QWidget *parent = nullptr);
    ~Login();
    static Login *_instance;
    static QMutex _mutex;
    void createWidgets();

    FocusedEditor *_edPassword1;
    FocusedEditor *_edPassword2;
    QLabel *_lbChangePasswd;

    static bool _logIn;
    QTimer *_timer;

};

#endif // LOGIN_H
