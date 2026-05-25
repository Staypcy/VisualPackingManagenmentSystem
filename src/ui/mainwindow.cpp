#include "ui/mainwindow.h"
#include "./ui_mainwindow.h"
#include "utils/logger.h"
#include<QMessageBox>
#include<QMenu>
#include<QAction>
#include<QPainter>
#include<QGraphicsProxyWidget>
#include<QVector>
#include<QPoint>
#include<QPropertyAnimation>
#include<QSequentialAnimationGroup>
#include<QParallelAnimationGroup>
#include "ui/cardetaildialog.h"
#include "utils/pathplanner.h"
#include "services/ifeeservice.h"
#include "services/iauthservice.h"

MainWindow::MainWindow(IFeeService *feeService, IAuthService *authService, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_feeService(feeService)
    , m_authService(authService)
    , loaddlg(nullptr)
    , parkwidget(nullptr)
    , manageMenu(nullptr)
{
    ui->setupUi(this);
    this->resize(800,500);
    loaddlg=new LoadDialog(m_authService, this);
    connect(ui->action,&QAction::triggered,[=](){
        if(!loaddlg){
            loaddlg=new LoadDialog(m_authService, this);
        }
        if(loaddlg->exec()==QDialog::Accepted){
            load_state=loaddlg->load_State;
            loaddlg->close(); // 立即关闭登录窗口
            if (load_state == 0) {
                QMessageBox::information(this, "提示", "管理员登录成功");
                update();
            } else if (load_state == 1) {
                QMessageBox::information(this, "提示", "普通用户登录成功");
                update();
            }
        }
    });
    connect(loaddlg,&LoadDialog::send_loadstate,[=](int load_s){
        load_state=load_s;
        if(load_state!=-1){
            parkwidget=new parkingWidget(this);
            setCentralWidget(parkwidget);
            parkwidget->setRowCol(row,col);
            connect(this,&MainWindow::sendRowCol,parkwidget,&parkingWidget::setRowCol);

            if(load_state==0){
                if(!manageMenu){
                manageMenu=ui->menubar->addMenu("管理");
                }
                QAction* setAct=new QAction("打开管理界面",manageMenu);
                manageMenu->addAction(setAct);
                connect(setAct,&QAction::triggered,[=](){

                    parkwidget->managewidget=new manageSetDialog(this);
                    parkwidget->managewidget->show();

                    connect(parkwidget->managewidget,&manageSetDialog::sendNewCar,[=](Car* car){
                        parkwidget->m_wait_carSet.push_back(car);
                        LOG_DEBUG()<<"成功添加"<<"当前队列车辆数量"<<parkwidget->m_wait_carSet.size();

                        QGraphicsProxyWidget*proxy=parkwidget->m_scene->addWidget(car);
                        //动态属性，相当于添加了一个成员变量
                        car->setProperty("proxy",QVariant::fromValue<QGraphicsProxyWidget*>(proxy));
                        showCarInTheWaitQueue(parkwidget);
                        
                        // 连接车辆点击事件
                        connect(car, &Car::clicked, this, [=](){
                            CarDetailDialog* dlg = new CarDetailDialog(car, m_feeService, this);
                            dlg->show();
                        });
                    });


                    connect(parkwidget->managewidget,&manageSetDialog::sendGetXY,[=](int xGet,int yGet){
                        if(isCarEntering) return; // 防止快速点击
                        // 检查是否有等待的车辆
                        if(parkwidget->m_wait_carSet.size() == 0){
                            return; // 队列为空，不执行
                        }
                        if(!parkwidget->parkIsFull()){
                            isCarEntering = true;
                            int row=parkwidget->row;
                            int col=parkwidget->col;
                            while(parkwidget->m_parkset[xGet][yGet]->haveCar){
                                xGet=rand()%row;
                                yGet=rand()%col;
                            }
                            Car *car=parkwidget->m_wait_carSet.dequeue();
                            car->x_col=xGet;
                            car->y_row=yGet;
                            parkwidget->m_in_carSet.push_back(car);
                            moveIn(xGet,yGet,car);
                        }
                    });

                    connect(parkwidget->managewidget,&manageSetDialog::CarToOut,[=](){
                        if(parkwidget->m_in_carSet.size()!=0){
                            Car *car = parkwidget->m_in_carSet.dequeue();
                            moveOut(car);
                        }
                    });

                });
            }
        }
    });

    setdlg=new SettingDialog(m_feeService, this);
    connect(ui->action_2,&QAction::triggered,[=](){
        if(!setdlg)
        setdlg=new SettingDialog(m_feeService, this);
        setdlg->show();
        connect(setdlg,&SettingDialog::senddata,[=](int x,int y){
            this->setRowCol(x,y);
            emit sendRowCol(x,y);
            //LOG_DEBUG()<<parkwidget->row<<parkwidget->col;
        });
    });

}
QPointF MainWindow::getParkSpaceCenter(int x,int y){
    ParkSpace* park=parkwidget->m_parkset[x][y];
    QPointF pos=park->pos;
    int centerX=pos.x()+park->width()/2.0;
    int centerY=pos.y()+park->height()/2.0;
    return QPointF(centerX,centerY);
};
void MainWindow::showCarInTheWaitQueue(parkingWidget *pw)
{//Car图标的尺寸(70,30)

    int startX=pw->width()-68;
    int startY=42;
    int sizeH=40;

    int i=0;
    for(auto car:pw->m_wait_carSet){
        QGraphicsProxyWidget* proxy=car->property("proxy").value<QGraphicsProxyWidget*>();
        if(proxy){
            proxy->setPos(startX,startY+i*sizeH);
            proxy->setZValue(1);
            QSizeF carSize=proxy->size();
            car->pos=QPoint(startX+carSize.width()/2,startY+i*sizeH+carSize.height()/2);
        }
        i++;
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::paintEvent(QPaintEvent *event)
{/*
    int width=this->width();
    int height=this->height();

    int num=height/20;
    int sizeH=20;
    int sizeW=20;

    QPainter painter(this);
    //auto temp=parkwidget->m_wait_carSet;
    painter.drawLine(width-40,40,width-40,height);
    for(int i=0;i<num;i++){
        painter.drawLine(width-40,40*(i+1),width,40*(i+1));
    }*/
}

void MainWindow::moveIn(int xGet, int yGet,Car *car)
{
    QGraphicsProxyWidget* proxy=car->property("proxy").value<QGraphicsProxyWidget*>();
    if(proxy==nullptr)return;

    QPointF startPos=proxy->pos();
    QSize carSize=car->m_size;

    // 计算停车位尺寸（与 parkingwidget.cpp 中的计算保持一致）
    int gap = 10;
    int startX = 20, startY = 20;
    int width = parkwidget->width() - 120;
    int height = parkwidget->height();
    int W = (width - startX - gap * (parkwidget->col - 1)) / parkwidget->col;
    int H = (height - startY - gap * (parkwidget->row - 1)) / parkwidget->row;
    QSize parkSize(W, H / 2);

    // 使用新的路径规划算法
    PathResult pathResult = PathPlanner::generateParkingPath(
        startPos,
        xGet,
        yGet,
        parkwidget->m_parkset,
        carSize,
        parkSize,
        5
    );

    if (!pathResult.isValid) {
        QMessageBox::warning(this, "路径规划失败", pathResult.error);
        isCarEntering = false;
        return;
    }

    // 创建平滑路径动画
    QSequentialAnimationGroup *group = new QSequentialAnimationGroup(this);
    const QVector<QPointF>& waypoints = pathResult.waypoints;

    for (int i = 0; i < waypoints.size() - 1; ++i) {
        QPropertyAnimation* anim = new QPropertyAnimation(proxy, "pos");
        anim->setDuration(40);
        anim->setStartValue(waypoints[i]);
        anim->setEndValue(waypoints[i + 1]);
        anim->setEasingCurve(QEasingCurve::Linear);
        group->addAnimation(anim);
    }

    // 倒车入库旋转动画
    QPropertyAnimation *rotateAnim = new QPropertyAnimation(proxy, "rotation");
    rotateAnim->setDuration(300);
    rotateAnim->setStartValue(0);
    rotateAnim->setEndValue(-8); // 轻微旋转模拟倒车入库
    rotateAnim->setEasingCurve(QEasingCurve::InOutQuad);
    group->addAnimation(rotateAnim);

    connect(group, &QSequentialAnimationGroup::finished, [=](){
        parkwidget->m_parkset[xGet][yGet]->haveCar=true;
        isCarEntering = false; // 重置标志，允许下次点击
        proxy->setRotation(0); // 恢复旋转角度
        group->deleteLater();
        // 更新等待队列显示，实现自动补位
        showCarInTheWaitQueue(parkwidget);
    });

    group->start(QAbstractAnimation::DeleteWhenStopped);
    car->x_col=xGet;
    car->y_row=yGet;
}

void MainWindow::moveOut(Car *car)
{
    QGraphicsProxyWidget*proxy=car->property("proxy").value<QGraphicsProxyWidget*>();
    if(proxy==nullptr)return ;
    parkwidget->m_parkset[car->x_col][car->y_row]->haveCar=false;

    Car temp;
    QSize carSize=temp.m_size;

    // 计算道路参数（与 pathplanner 一致）
    int gap = 10;
    int startY = 20;
    int width = parkwidget->width();
    int height = parkwidget->height();
    int parkAreaWidth = width - 120;
    int W = (parkAreaWidth - 20 - gap * (parkwidget->col - 1)) / parkwidget->col;
    int H = (height - startY - gap * (parkwidget->row - 1)) / parkwidget->row;
    int halfH = H / 2;

    // 当前行下方的道路中心（与pathplanner一致）
    int roadY = startY + car->x_col * (H + gap) + halfH + H / 4 + gap / 2;

    // 右侧垂直车道
    int dividerX = width - carSize.width();
    int rightLaneX = dividerX - carSize.width() - 10;

    // 出口位置（右下角）
    double endX = width - carSize.width() - 20;
    double endY = height - carSize.height() - 20;

    QPointF currentPos = proxy->pos();

    // 使用分段直线动画：驶出车位 → 沿道路行驶 → 沿右侧车道驶离
    QSequentialAnimationGroup *group = new QSequentialAnimationGroup(this);

    // 第1段：驶出停车位，进入下方道路
    QPointF roadEntry(currentPos.x(), roadY - carSize.height() / 2);
    QPropertyAnimation *anim1 = new QPropertyAnimation(proxy, "pos");
    anim1->setDuration(200);
    anim1->setStartValue(currentPos);
    anim1->setEndValue(roadEntry);
    anim1->setEasingCurve(QEasingCurve::Linear);
    group->addAnimation(anim1);

    // 第2段：沿道路水平行驶到右侧车道
    QPointF rightLaneEntry(rightLaneX, roadY - carSize.height() / 2);
    QPropertyAnimation *anim2 = new QPropertyAnimation(proxy, "pos");
    anim2->setDuration(300);
    anim2->setStartValue(roadEntry);
    anim2->setEndValue(rightLaneEntry);
    anim2->setEasingCurve(QEasingCurve::Linear);
    group->addAnimation(anim2);

    // 第3段：沿右侧车道向下行驶到出口
    QPointF exitPos(endX, endY);
    QPropertyAnimation *anim3 = new QPropertyAnimation(proxy, "pos");
    anim3->setDuration(300);
    anim3->setStartValue(rightLaneEntry);
    anim3->setEndValue(exitPos);
    anim3->setEasingCurve(QEasingCurve::Linear);
    group->addAnimation(anim3);

    connect(group, &QSequentialAnimationGroup::finished, [=](){
        car->deleteLater();
        group->deleteLater();
    });

    group->start(QAbstractAnimation::DeleteWhenStopped);
}
