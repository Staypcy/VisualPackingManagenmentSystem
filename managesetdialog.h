#ifndef MANAGESETDIALOG_H
#define MANAGESETDIALOG_H

#include <QDialog>
#include<QPushButton>
#include<QColor>
#include<QSize>
#include<QTimer>
#include<QVector>
#include<QDateTime>

namespace Ui {
class manageSetDialog;
}


class Car:public QPushButton{
public:
    void setCar(QString carnum,QString carstyle,QColor color);
    void setCar(QString carnum, QString carstyle, QColor color, QString imagePath);
    Car(QWidget*parent=nullptr){}
public:
    QString car_num;
    QString car_style;
    QColor car_color;
    QPoint pos=QPoint(0,0);
    QSize m_size=QSize(70,30);
    int x_col;
    int y_row;
    QString car_image_path;
    QDateTime enter_time;
    double total_fee;
};


class manageSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit manageSetDialog(QWidget *parent = nullptr);
    ~manageSetDialog();
    
    void updateParkingMap(QVector<QVector<bool>> map);
    
signals:
    void sendNewCar(Car* car);
    void sendGetXY(int x,int y);
    void CarToOut();
public slots:
    void setRowCol(int x,int y);
private:
    void startAutoEnter();
    
    Ui::manageSetDialog *ui;
    QColor color;
    QTimer* autoTimer;
    bool isAutoEntering;
    QVector<QVector<bool>> parkingMap;
    QString selectedImagePath;

    int row=4;
    int col=4;
};

#endif // MANAGESETDIALOG_H
