#include "bms_systemmanagerwidget.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QSizePolicy>

BMS_SystemManagerWidget::BMS_SystemManagerWidget(QWidget *parent) : QWidget(parent)
{
    createWidgets();
}

void BMS_SystemManagerWidget::createWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();
    //layout->setContentsMargins(0,0,0,0);

    layout->setSpacing(10);



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

    button = new QPushButton("Events");
    button->setProperty("ID", 4);
    connect(button,&QPushButton::clicked,this,&BMS_SystemManagerWidget::buttonClicked);
    layout->addWidget(button);

    //layout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Minimum,QSizePolicy::Maximum));

    setLayout(layout);

}


void BMS_SystemManagerWidget::buttonClicked()
{
    QPushButton *btn = (QPushButton*)sender();

    int id = btn->property("ID").toInt();
//    qDebug()<<Q_FUNC_INFO<<" ID:"<<id;
    emit validFunction(id);
}
