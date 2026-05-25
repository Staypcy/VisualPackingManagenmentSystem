#ifndef MANAGESETDIALOG_H
#define MANAGESETDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QColor>
#include <QSize>
#include <QTimer>
#include <QVector>
#include <QDateTime>
#include "entities/car.h"

namespace Ui {
class manageSetDialog;
}


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
