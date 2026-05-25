#ifndef CAR_H
#define CAR_H

#include <QPushButton>
#include <QColor>
#include <QSize>
#include <QPoint>
#include <QDateTime>

class Car : public QPushButton
{
public:
    void setCar(QString carnum, QString carstyle, QColor color);
    void setCar(QString carnum, QString carstyle, QColor color, QString imagePath);
    Car(QWidget *parent = nullptr) {}

public:
    QString car_num;
    QString car_style;
    QColor car_color;
    QPoint pos = QPoint(0, 0);
    QSize m_size = QSize(70, 30);
    int x_col;
    int y_row;
    QString car_image_path;
    QDateTime enter_time;
    double total_fee;
};

#endif // CAR_H
