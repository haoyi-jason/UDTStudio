#include "bms_systemmanagerwidget.h"

#include <QDebug>
#include <QVBoxLayout>

BMS_SystemManagerWidget::BMS_SystemManagerWidget(QWidget *parent) : QWidget(parent)
{
    createWidgets();
}

void BMS_SystemManagerWidget::createWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(2);

    QPushButton *button;

    button = new QPushButton("Hardware Config");
    button->setProperty("ID", 1);
    connect(button,&QPushButton::clicked,this,&BMS_SystemManagerWidget::buttonClicked);
    layout->addWidget(button);

    button = new QPushButton("BMS Config");
    button->setProperty("ID", 2);
    connect(button,&QPushButton::clicked,this,&BMS_SystemManagerWidget::buttonClicked);
    layout->addWidget(button);

    button = new QPushButton("Criteria Config");
    button->setProperty("ID", 3);
    connect(button,&QPushButton::clicked,this,&BMS_SystemManagerWidget::buttonClicked);
    layout->addWidget(button);

    setLayout(layout);

}


void BMS_SystemManagerWidget::buttonClicked()
{
    QPushButton *btn = (QPushButton*)sender();

    int id = btn->property("ID").toInt();

//    switch(id){
//    case 1:
//        break;
//    case 2:
//        break;
//    case 3:
//        break;
//    default:break;
//    }

    qDebug()<<Q_FUNC_INFO<<" ID:"<<id;
    emit validFunction(id);
}
