#ifndef FOCUSEDEDITOR_H
#define FOCUSEDEDITOR_H

#include <QWidget>
#include <QLineEdit>
#include "numberpad.h"

class FocusedEditor : public QLineEdit
{
    Q_OBJECT
public:
    explicit FocusedEditor(QWidget *parent = nullptr);
    //explicit FocusedEditor(int min = INT_MIN, int max = INT_MAX, QWidget *parent = nullptr);

    void setPasswordMode(bool on);
    bool event(QEvent *e) override;

signals:
    void focused(bool on);
    void edited();

public slots:
    void accepted();
    void rejected();

private slots:
    void focusInternal();
//    void setText(QString &text);

protected:
    virtual void textEdited() ;
    //virtual void focusInEvent(QFocusEvent *e);
    //virtual void focusOutEvent(QFocusEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);


private:
    int _max;
    int _min;
    QString _resultString;
};

#endif // FOCUSEDEDITOR_H
