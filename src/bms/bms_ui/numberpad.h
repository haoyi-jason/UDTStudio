#ifndef NUMBERPAD_H
#define NUMBERPAD_H

#include <QDialog>
#include <QWidget>
#include <QToolButton>
#include <QLineEdit>
#include <QLabel>
#include <QEvent>
#include "focusededitor.h"
#include <QTimer>

static QMap<QString,int> key_map{
    {"0",0},
    {"1",1},
    {"2",2},
    {"3",3},
    {"4",4},
    {"5",5},
    {"6",6},
    {"7",7},
    {"8",8},
    {"9",9},
    {".",10},
    {"OK",11},
    {"CAN",12},
    {"<-",13},
    {"+/-",14},
};

class Button:public QToolButton
{
    Q_OBJECT
public:
    explicit Button(const QString &text, QWidget *parent = nullptr);
    QSize sizeHint() const override;

};

class NumberPad : public QDialog
{
    Q_OBJECT
public:
    explicit NumberPad(QWidget *parent = nullptr);
    QString result();

private:
    Button *createButton(const QString &text, const char *member);

signals:
    void result(QString);
    void isvalid(bool);

public slots:
    void setText(QString &content);
    void setMax(int value = INT_MAX);
    void setMin(int value = INT_MIN);
    void passwordMode(bool on);
    void validInput(bool on);

private slots:
    void padClicked();
    void timeout();

private:
    bool _checkInput;
    bool _passChar;
    bool _isNumber;
    int _maxInput;
    int _minInput;
    QString _inputString;
    QLineEdit *_inBox;
    QList<Button*> _buttons;
    QLabel *_hintText;
    QString _validString;
    QString _resultString;
    QTimer *_timer;
};

#endif // NUMBERPAD_H
