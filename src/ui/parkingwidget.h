#ifndef PARKINGWIDGET_H
#define PARKINGWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QPoint>
#include <QQueue>
#include <QColor>
#include <QTimer>
#include "entities/parkspace.h"
#include "entities/car.h"
#include "ui/managesetdialog.h"

class parkingWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit parkingWidget(QWidget *parent = nullptr);
    ~parkingWidget();


    void drawPark();
    void setRowCol(int row1,int col1);
    void drawline();

    bool parkIsFull();
    
    void onCarClicked(Car* car);
    
signals:
    void sendRowCol_to_managesetdialog(int x,int y);
    void carClicked(Car* car);
private slots:
    void updateFeeDisplay();

private:
    QTimer* feeUpdateTimer;
    
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
