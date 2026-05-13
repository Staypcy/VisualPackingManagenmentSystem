#include "managesetdialog.h"
#include "ui_managesetdialog.h"
#include<QColorDialog>
#include<QTime>
#include<QTimer>

manageSetDialog::manageSetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::manageSetDialog)
{
    ui->setupUi(this);

    connect(ui->ColorChoice,&QPushButton::clicked,[=](){
        color=QColorDialog::getColor(Qt::white,this,"选择颜色");
    });
    connect(ui->addCar,&QPushButton::clicked,[=](){
        QString car_name=ui->addCarNum->text();
        QString car_style=ui->addCarStyle->text();
        Car *car=new Car();
        if(!car_name.isEmpty()&&!car_style.isEmpty()){
            car->setCar(car_name,car_style,color);
            QSize size=car->m_size;
            car->setFixedSize(size);
            car->setText(car->car_num);
            car->setStyleSheet(QString("background-color: %1; border: 2px solid black;").arg(car->car_color.name()));
            emit sendNewCar(car);
            qDebug()<<"发送成功";
        }
    });

    connect(ui->CarEnter,&QPushButton::clicked,[=](){
        int x=rand()%row;
        int y=rand()%col;
        emit sendGetXY(x,y);
    });

    connect(ui->CarOut,&QPushButton::clicked,this,&manageSetDialog::CarToOut);
}

manageSetDialog::~manageSetDialog()
{
    delete ui;
}

void Car::setCar(QString carnum, QString carstyle, QColor color)
{
    car_num=carnum;
    car_color=color;
    car_style=carstyle;
}
