#ifndef PATHPLANNER_H
#define PATHPLANNER_H

#include <QPointF>
#include <QVector>
#include <QPainterPath>
#include <QRectF>
#include <QSize>
#include "entities/pathresult.h"

class ParkSpace;
class QGraphicsProxyWidget;

class PathPlanner
{
public:
    static PathResult generateParkingPath(
        QPointF startPos,
        int targetRow,
        int targetCol,
        const QVector<QVector<ParkSpace*>>& parkingGrid,
        QSize carSize,
        QSize parkSize,
        int safetyMargin = 5
    );
    
    static void createPathAnimation(QGraphicsProxyWidget* proxy, const QVector<QPointF>& waypoints);
    
private:
    static bool checkCollision(
        const QPointF& point,
        const QVector<QVector<ParkSpace*>>& parkingGrid,
        QSize carSize,
        QSize parkSize
    );
};

#endif // PATHPLANNER_H