#include "pathplanner.h"
#include "parkingwidget.h"
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsProxyWidget>
#include <QMessageBox>
#include <QDebug>

PathResult PathPlanner::generateParkingPath(
    QPointF startPos,
    int targetRow,
    int targetCol,
    const QVector<QVector<ParkSpace*>>& parkingGrid,
    QSize carSize,
    QSize parkSize,
    int safetyMargin)
{
    PathResult result;
    result.isValid = true;

    if (targetRow < 0 || targetRow >= parkingGrid.size()) {
        result.isValid = false;
        result.error = "目标行索引无效";
        return result;
    }

    if (targetCol < 0 || targetCol >= parkingGrid[0].size()) {
        result.isValid = false;
        result.error = "目标列索引无效";
        return result;
    }

    ParkSpace* targetPark = parkingGrid[targetRow][targetCol];
    if (!targetPark || targetPark->haveCar) {
        result.isValid = false;
        result.error = "目标车位不存在或已被占用";
        return result;
    }

    int row = parkingGrid.size();
    int col = parkingGrid[0].size();
    int gap = 10;
    int startX = 20, startY = 20;
    int width = 800;
    int height = 500;

    int parkAreaWidth = width - 120;
    int W = (parkAreaWidth - startX - gap * (col - 1)) / col;
    int H = (height - startY - gap * (row - 1)) / row;
    int halfH = H / 2;

    int dividerX = width - carSize.width();

    // 目标行下方的道路中心Y坐标
    // 道路空间: ParkSpace底部(startY+targetRow*(H+gap)+halfH) 到 下一行ParkSpace顶部(startY+(targetRow+1)*(H+gap))
    // 道路中心 = (ParkSpace底部 + 下一行顶部) / 2
    int roadY = startY + targetRow * (H + gap) + halfH + H / 4 + gap / 2;

    // 右侧垂直车道 X 坐标（车辆在此车道上行至目标道路）
    int rightLaneX = dividerX - carSize.width() - 10;

    QPointF parkTopLeft = QPointF(targetPark->pos.x(), targetPark->pos.y());
    QPointF targetPos(
        parkTopLeft.x() + parkSize.width() / 2 - carSize.width() / 2,
        parkTopLeft.y() + parkSize.height() - carSize.height() - 5
    );

    qDebug() << "========== [路径规划 - 直线道路] ==========";
    qDebug() << "起点:" << startPos;
    qDebug() << "目标车位:" << targetRow << "," << targetCol;
    qDebug() << "目标位置:" << targetPos;
    qDebug() << "道路Y:" << roadY << " 右侧车道X:" << rightLaneX;

    // 5个控制点，全部直线连接
    QVector<QPointF> controlPoints;

    // CP1: 当前位置（等待队列）
    controlPoints.append(startPos);

    // CP2: 水平行驶到右侧车道
    controlPoints.append(QPointF(rightLaneX, startPos.y()));

    // CP3: 沿右侧车道垂直行驶到目标行道路
    controlPoints.append(QPointF(rightLaneX, roadY - carSize.height() / 2));

    // CP4: 沿道路水平行驶到目标车位正前方
    controlPoints.append(QPointF(targetPos.x(), roadY - carSize.height() / 2));

    // CP5: 垂直驶入停车位
    controlPoints.append(targetPos);

    qDebug() << "控制点:" << controlPoints;

    // 碰撞检测
    for (const QPointF& pt : controlPoints) {
        if (checkCollision(pt, parkingGrid, carSize, parkSize)) {
            result.isValid = false;
            result.error = "路径与已占用车位冲突";
            return result;
        }
    }

    // 生成直线路段路径点（每段20个插值点）
    QPainterPath straightPath;
    QVector<QPointF> waypoints;

    if (!controlPoints.isEmpty()) {
        straightPath.moveTo(controlPoints[0]);
        waypoints.append(controlPoints[0]);

        const int pointsPerSegment = 20;
        for (int i = 0; i < controlPoints.size() - 1; ++i) {
            QPointF from = controlPoints[i];
            QPointF to = controlPoints[i + 1];
            straightPath.lineTo(to);

            for (int j = 1; j <= pointsPerSegment; ++j) {
                qreal t = (qreal)j / pointsPerSegment;
                waypoints.append(from * (1 - t) + to * t);
            }
        }
    }

    qDebug() << "路径点数量:" << waypoints.size();
    qDebug() << "=========================================";

    result.waypoints = waypoints;
    result.smoothPath = straightPath;

    return result;
}

void PathPlanner::createPathAnimation(QGraphicsProxyWidget* proxy, const QVector<QPointF>& waypoints)
{
    QSequentialAnimationGroup* group = new QSequentialAnimationGroup();
    group->setParent(proxy);

    for (int i = 0; i < waypoints.size() - 1; ++i) {
        QPropertyAnimation* anim = new QPropertyAnimation(proxy, "pos");
        anim->setDuration(35);
        anim->setStartValue(waypoints[i]);
        anim->setEndValue(waypoints[i + 1]);
        anim->setEasingCurve(QEasingCurve::Linear);
        group->addAnimation(anim);
    }

    QPropertyAnimation* rotateAnim = new QPropertyAnimation(proxy, "rotation");
    rotateAnim->setDuration(300);
    rotateAnim->setStartValue(0);
    rotateAnim->setEndValue(-10);

    group->addAnimation(rotateAnim);

    QObject::connect(group, &QSequentialAnimationGroup::finished, [=]() {
        proxy->setRotation(0);
        group->deleteLater();
    });

    group->start(QAbstractAnimation::DeleteWhenStopped);
}

bool PathPlanner::checkCollision(
    const QPointF& point,
    const QVector<QVector<ParkSpace*>>& parkingGrid,
    QSize carSize,
    QSize parkSize)
{
    for (const auto& row : parkingGrid) {
        for (ParkSpace* park : row) {
            if (park && park->haveCar) {
                QRectF occupiedRect(
                    park->pos.x(),
                    park->pos.y(),
                    parkSize.width(),
                    parkSize.height()
                );

                QRectF carRect(
                    point.x(),
                    point.y(),
                    carSize.width(),
                    carSize.height()
                );

                if (occupiedRect.intersects(carRect)) {
                    return true;
                }
            }
        }
    }
    return false;
}
