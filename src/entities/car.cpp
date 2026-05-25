#include "entities/car.h"
#include <QDateTime>

void Car::setCar(QString carnum, QString carstyle, QColor color)
{
    car_num = carnum;
    car_color = color;
    car_style = carstyle;
    enter_time = QDateTime::currentDateTime();
    total_fee = 0;
}

void Car::setCar(QString carnum, QString carstyle, QColor color, QString imagePath)
{
    car_num = carnum;
    car_color = color;
    car_style = carstyle;
    car_image_path = imagePath;
    enter_time = QDateTime::currentDateTime();
    total_fee = 0;
}
