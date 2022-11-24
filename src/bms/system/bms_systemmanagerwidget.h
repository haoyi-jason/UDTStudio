#ifndef BMS_SYSTEMMANAGERWIDGET_H
#define BMS_SYSTEMMANAGERWIDGET_H

#include <QWidget>
#include <QPushButton>

class BMS_SystemManagerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BMS_SystemManagerWidget(QWidget *parent = nullptr);


private:
    void createWidgets();

private slots:
    void buttonClicked();

signals:
    void validFunction(int function);

public slots:
};

#endif // BMS_SYSTEMMANAGERWIDGET_H
