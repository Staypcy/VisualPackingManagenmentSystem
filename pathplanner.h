#ifndef PATHPLANNER_H
#define PATHPLANNER_H

#include <QPointF>
#include <QVector>
#include <QPainterPath>
#include <QRectF>
#include <QSize>

class ParkSpace;
class QGraphicsProxyWidget;

struct PathResult {
    QVector<QPointF> waypoints;
    QPainterPath smoothPath;
    bool isValid;
    QString error;
};

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