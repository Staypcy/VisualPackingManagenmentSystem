#ifndef PARKSPACE_H
#define PARKSPACE_H

#include <QWidget>
#include <QPoint>
#include <QColor>

class ParkSpace : public QWidget
{
public:
    ParkSpace(QWidget *parent = nullptr);

public:
    QPoint pos;
    bool haveCar = false;
    QColor m_color;
};

#endif // PARKSPACE_H
