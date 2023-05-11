#ifndef QLEDINDICATOR_H
#define QLEDINDICATOR_H

#include <QAbstractButton>
#include <QResizeEvent>
#include <QColor>


class QLedIndicator : public QAbstractButton
{
    Q_PROPERTY(QColor onColor1 READ getOnColor1 WRITE setOnColor1  );
    Q_PROPERTY(QColor onColor2 READ getOnColor2 WRITE setOnColor2  );
    Q_PROPERTY(QColor offColor1 READ getOffColor1 WRITE setOffColor1  );
    Q_PROPERTY(QColor offColor2 READ getOffColor2 WRITE setOffColor2  );
    Q_OBJECT
public:
    QLedIndicator(QWidget *parent = nullptr);

    void setOnColor1(QColor c){onColor1 = c;}
    void setOffColor1(QColor c){offColor1 = c;}
    void setOnColor2(QColor c){onColor2 = c;}
    void setOffColor2(QColor c){offColor2 = c;}

    QColor getOnColor1() {return onColor1;}
    QColor getOffColor1(){return offColor1;}
    QColor getOnColor2() {return onColor2;}
    QColor getOffColor2(){return offColor2;}

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *event);

private:
    static const qreal scaledSize;
    QColor onColor1,offColor1;
    QColor onColor2,offColor2;
    QPixmap _ledBuffer;

};

#endif // QLEDINDICATOR_H
