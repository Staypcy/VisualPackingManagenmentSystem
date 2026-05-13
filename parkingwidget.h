#ifndef PARKINGWIDGET_H
#define PARKINGWIDGET_H

#include <QGraphicsView>
#include<QGraphicsScene>
#include<QPushButton>
#include<QPoint>
#include<QQueue>
#include<QColor>
#include "managesetdialog.h"

class ParkSpace:public QWidget{
public:
    ParkSpace(QWidget* parent=nullptr);
public:
    QPoint pos;
    bool haveCar=false;
    QColor m_color;
};

class parkingWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit parkingWidget(QWidget *parent = nullptr);


    void drawPark();
    void setRowCol(int row1,int col1){row=row1;col=col1;drawPark();}
    void drawline();

    bool parkIsFull();
signals:
    void sendRowCol_to_managesetdialog(int x,int y);
public:
    QGraphicsScene*m_scene;
    QVector<QVector<ParkSpace*>>m_parkset;
    QQueue<Car*>m_wait_carSet;
    QQueue<Car*>m_in_carSet;
    manageSetDialog* managewidget;
    bool isfull=false;

    int row=4;
    int col=4;
};

#endif // PARKINGWIDGET_H
