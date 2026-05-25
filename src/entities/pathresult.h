#ifndef PATHRESULT_H
#define PATHRESULT_H

#include <QVector>
#include <QPointF>
#include <QPainterPath>
#include <QString>

struct PathResult
{
    QVector<QPointF> waypoints;
    QPainterPath smoothPath;
    bool isValid;
    QString error;
};

#endif // PATHRESULT_H
