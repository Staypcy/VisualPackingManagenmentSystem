#include <QtTest>
#include "utils/pathplanner.h"
#include "entities/parkspace.h"

class PathPlannerTest : public QObject
{
    Q_OBJECT

private:
    QVector<QVector<ParkSpace*>> createEmptyGrid(int rows, int cols)
    {
        QVector<QVector<ParkSpace*>> grid(rows);
        for (int i = 0; i < rows; ++i) {
            grid[i].resize(cols);
            for (int j = 0; j < cols; ++j) {
                ParkSpace *p = new ParkSpace();
                p->pos = QPoint(20 + j * 157, 20 + i * 122);
                p->haveCar = false;
                grid[i][j] = p;
            }
        }
        return grid;
    }

    void cleanupGrid(QVector<QVector<ParkSpace*>> &grid)
    {
        for (auto &row : grid) {
            for (auto &p : row) {
                delete p;
            }
        }
    }

private slots:
    void testValidPathGeneration()
    {
        auto grid = createEmptyGrid(4, 4);
        QPointF startPos(650, 100);
        QSize carSize(70, 30);
        QSize parkSize(157, 56);

        PathResult result = PathPlanner::generateParkingPath(
            startPos, 0, 0, grid, carSize, parkSize);

        QVERIFY(result.isValid);
        QVERIFY(result.waypoints.size() > 0);
        QVERIFY(result.error.isEmpty());
        cleanupGrid(grid);
    }

    void testInvalidRow()
    {
        auto grid = createEmptyGrid(4, 4);
        PathResult result = PathPlanner::generateParkingPath(
            QPointF(650, 100), 99, 0, grid, QSize(70, 30), QSize(157, 56));
        QVERIFY(!result.isValid);
        QVERIFY(!result.error.isEmpty());
        cleanupGrid(grid);
    }

    void testInvalidCol()
    {
        auto grid = createEmptyGrid(4, 4);
        PathResult result = PathPlanner::generateParkingPath(
            QPointF(650, 100), 0, 99, grid, QSize(70, 30), QSize(157, 56));
        QVERIFY(!result.isValid);
        QVERIFY(!result.error.isEmpty());
        cleanupGrid(grid);
    }

    void testOccupiedSpace()
    {
        auto grid = createEmptyGrid(4, 4);
        grid[0][0]->haveCar = true;

        PathResult result = PathPlanner::generateParkingPath(
            QPointF(650, 100), 0, 0, grid, QSize(70, 30), QSize(157, 56));
        QVERIFY(!result.isValid);
        cleanupGrid(grid);
    }

    void testWaypointsAreSequential()
    {
        auto grid = createEmptyGrid(4, 4);
        PathResult result = PathPlanner::generateParkingPath(
            QPointF(650, 100), 2, 2, grid, QSize(70, 30), QSize(157, 56));

        QVERIFY(result.isValid);
        // Verify waypoints form a reasonable path (first point is startPos)
        const auto &wp = result.waypoints;
        QCOMPARE(wp.first(), QPointF(650, 100));
        QVERIFY(wp.size() >= 80); // 4 segments * 20 points + 1 start
        cleanupGrid(grid);
    }
};

QTEST_MAIN(PathPlannerTest)
#include "tst_pathplanner.moc"
