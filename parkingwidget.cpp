#include "parkingwidget.h"
#include<QGraphicsProxyWidget>
#include<QPainter>
#include<QDebug>
#include<QFont>

parkingWidget::parkingWidget(QWidget *parent)
    :QGraphicsView(parent),
    feeUpdateTimer(nullptr)
{
    m_scene=new QGraphicsScene(this);
    m_scene->setSceneRect(0,0,800,500);

    // 设置场景背景色（地面）
    m_scene->setBackgroundBrush(QColor(180, 190, 170));

    m_parkset.resize(row);
    for(int i=0;i<row;i++){
        m_parkset[i].resize(col);
    }
    qDebug()<<m_parkset.size()<<" "<<m_parkset[0].size();

    setScene(m_scene);
    setFixedSize(800,500);

    drawPark(); // 内部已调用 drawline()

    // 启动费用更新定时器
    feeUpdateTimer = new QTimer(this);
    connect(feeUpdateTimer, &QTimer::timeout, this, &parkingWidget::updateFeeDisplay);
    feeUpdateTimer->start(1000); // 每秒更新一次
}

parkingWidget::~parkingWidget()
{
    if(feeUpdateTimer){
        feeUpdateTimer->stop();
        delete feeUpdateTimer;
    }
}

void parkingWidget::drawPark()
{
    m_scene->clear();
    m_parkset.clear();
    m_parkset.resize(row);
    for (int i = 0; i < row; ++i) {
        m_parkset[i].resize(col);
    }

    int width = this->width()-120;
    int height = this->height();

    // 画停车位
    int gap = 10;
    int startX = 20, startY = 20;

    int W = (width - startX - gap*(col-1)) / col;
    int H = (height - startY - gap*(row-1)) / row;
    int halfH = H / 2;

    // 先绘制道路（在停车位下方）
    drawline();

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            int x = startX + j * (W);
            int y = startY + i * (H + gap);

            ParkSpace *park = new ParkSpace(nullptr);
            park->setFixedSize(W, halfH);
            park->pos=QPoint(x,y);
            park->setStyleSheet(
                "background-color: #5cb85c;"
                "border: 2px solid #3d8b40;"
                "border-radius: 3px;"
            );

            QGraphicsProxyWidget *proxy = m_scene->addWidget(park);
            proxy->setPos(x, y);
            proxy->setZValue(1); // 停车位在道路上方

            m_parkset[i][j]=(park);
        }
    }
}

void parkingWidget::drawline()
{
    int width = this->width();
    int height = this->height();

    int gap = 10;
    int startX = 20, startY = 20;
    int parkAreaWidth = width - 120;

    int W = (parkAreaWidth - startX - gap * (col - 1)) / col;
    int H = (height - startY - gap * (row - 1)) / row;
    int halfH = H / 2;

    Car car(nullptr);
    QSize carSize = car.m_size;
    int dividerX = width - carSize.width();          // 等待队列分隔线
    int rightLaneLeft = dividerX - carSize.width() - 10;  // 右侧垂直车道左边界

    qreal parkBottom = (row > 0) ? startY + row * (H + gap) : startY;

    // =============================================
    // 1. 停车场内部地面（沥青色）
    // =============================================
    QBrush asphaltBrush(QColor(120, 120, 120));
    QRectF parkFloor(startX, startY, dividerX - startX, parkBottom - startY);
    m_scene->addRect(parkFloor, Qt::NoPen, asphaltBrush)->setZValue(-1);

    // =============================================
    // 2. 水平道路路面（深灰色，位于每两行之间）
    // =============================================
    QBrush roadBrush(QColor(100, 100, 100));
    for (int i = 0; i < row - 1; ++i) {
        qreal roadTop = startY + i * (H + gap) + halfH;
        qreal roadBottom = startY + (i + 1) * (H + gap);
        QRectF roadRect(startX, roadTop, dividerX - startX, roadBottom - roadTop);
        m_scene->addRect(roadRect, Qt::NoPen, roadBrush)->setZValue(0);
    }
    // 最后一行下方的道路
    if (row > 0) {
        qreal roadTop = startY + (row - 1) * (H + gap) + halfH;
        qreal roadBottom = parkBottom;
        QRectF roadRect(startX, roadTop, dividerX - startX, roadBottom - roadTop);
        m_scene->addRect(roadRect, Qt::NoPen, roadBrush)->setZValue(0);
    }

    // =============================================
    // 3. 右侧垂直车道
    // =============================================
    {
        qreal laneTop = startY;
        qreal laneBottom = parkBottom;
        QRectF laneRect(rightLaneLeft, laneTop, dividerX - rightLaneLeft, laneBottom - laneTop);
        m_scene->addRect(laneRect, Qt::NoPen, roadBrush)->setZValue(0);
    }

    // =============================================
    // 4. 水平道路中心虚线（黄色虚线）
    // =============================================
    {
        QPen centerPen(QColor(255, 215, 0), 2, Qt::DashLine);
        centerPen.setDashPattern({10, 6});

        for (int i = 0; i < row - 1; ++i) {
            qreal roadCenterY = startY + i * (H + gap) + halfH + (H + gap - halfH) / 2.0;
            // = startY + i*(H+gap) + halfH + H/4 + gap/2
            m_scene->addLine(startX, roadCenterY, dividerX, roadCenterY, centerPen)->setZValue(0);
        }
        if (row > 0) {
            qreal roadCenterY = startY + (row - 1) * (H + gap) + halfH + (H + gap - halfH) / 2.0;
            m_scene->addLine(startX, roadCenterY, dividerX, roadCenterY, centerPen)->setZValue(0);
        }
    }

    // =============================================
    // 5. 右侧垂直车道中心虚线
    // =============================================
    {
        QPen vCenterPen(QColor(255, 215, 0), 2, Qt::DashLine);
        vCenterPen.setDashPattern({6, 6});
        qreal laneCenterX = (rightLaneLeft + dividerX) / 2.0;
        m_scene->addLine(laneCenterX, startY, laneCenterX, parkBottom, vCenterPen)->setZValue(0);
    }

    // =============================================
    // 6. 停车位标线（每个停车位内部白色虚线框）
    // =============================================
    {
        QPen stallPen(Qt::white, 1, Qt::SolidLine);
        for (int i = 0; i < row; ++i) {
            for (int j = 0; j < col; ++j) {
                qreal x = startX + j * W;
                qreal y = startY + i * (H + gap);
                // 停车位左右边界线
                m_scene->addLine(x, y, x, y + halfH, stallPen)->setZValue(2);
                m_scene->addLine(x + W, y, x + W, y + halfH, stallPen)->setZValue(2);
                // 停车位后方横线
                m_scene->addLine(x, y + halfH - 2, x + W, y + halfH - 2, stallPen)->setZValue(2);
            }
        }
    }

    // =============================================
    // 7. 等待队列分隔线（实线）
    // =============================================
    {
        QPen dividerPen(Qt::white, 2, Qt::SolidLine);
        m_scene->addLine(dividerX, 40, dividerX, height - 20, dividerPen)->setZValue(0);
    }

    // =============================================
    // 8. 等待队列停车位虚线
    // =============================================
    {
        QPen waitSlotPen(QColor(200, 200, 200), 1, Qt::DashLine);
        waitSlotPen.setDashPattern({4, 4});
        int num = (height - 40) / 40;
        for (int i = 0; i < num; ++i) {
            m_scene->addLine(dividerX, 40.0 * (i + 1), (qreal)width, 40.0 * (i + 1), waitSlotPen)->setZValue(0);
        }
    }

    // =============================================
    // 9. 停车场外框
    // =============================================
    {
        QPen borderPen(Qt::white, 3, Qt::SolidLine);
        m_scene->addRect(startX - 2, startY - 2, dividerX - startX + 4, parkBottom - startY + 4,
                         borderPen)->setZValue(0);
    }

    // =============================================
    // 10. 入口/出口文字标注
    // =============================================
    {
        QFont font("Microsoft YaHei", 10, QFont::Bold);
        QColor labelColor(255, 255, 255);

        // 入口标注（右上方）
        QGraphicsTextItem *entryLabel = m_scene->addText("入口 ↓", font);
        entryLabel->setDefaultTextColor(labelColor);
        entryLabel->setPos(dividerX + 5, 5);
        entryLabel->setZValue(0);

        // 出口标注（右下方）
        QGraphicsTextItem *exitLabel = m_scene->addText("出口 ↓", font);
        exitLabel->setDefaultTextColor(labelColor);
        exitLabel->setPos(dividerX + 5, height - 40);
        exitLabel->setZValue(0);
    }
}

bool parkingWidget::parkIsFull()
{
    int rowSet=m_parkset.size();
    if(rowSet==0)return false;
    int colSet=m_parkset[0].size();
    for(int i=0;i<rowSet;i++){
        for(int j=0;j<colSet;j++){
            if(!m_parkset[i][j]->haveCar){
                isfull=false;
                return false;
            }
        }
    }
    isfull=true;
    return true;
}

ParkSpace::ParkSpace(QWidget *parent)
    :QWidget(parent),
    haveCar(false),pos(0,0)
{
    setStyleSheet(
        "background-color: #5cb85c;"
        "border: 2px solid #3d8b40;"
        "border-radius: 3px;"
    );
}

void parkingWidget::setRowCol(int row1, int col1)
{
    row = row1;
    col = col1;
    drawPark();
}

void parkingWidget::onCarClicked(Car* car)
{
    emit carClicked(car);
}

void parkingWidget::updateFeeDisplay()
{
    // 费用更新逻辑在CarDetailDialog中处理
}
