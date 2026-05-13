#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include<QMessageBox>
#include<QMenu>
#include<QAction>
#include<QPainter>
#include<QGraphicsProxyWidget>
#include<QVector>
#include<QPoint>
#include<QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , loaddlg(nullptr)
    , parkwidget(nullptr)
    , manageMenu(nullptr)
{
    ui->setupUi(this);
    this->resize(800,500);
    loaddlg=new LoadDialog(this);
    connect(ui->action,&QAction::triggered,[=](){
        if(!loaddlg){
            loaddlg=new LoadDialog(this);
        }
        if(loaddlg->exec()==QDialog::Accepted){
            load_state=loaddlg->load_State;
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
                        qDebug()<<"成功添加"<<"当前队列车辆数量"<<parkwidget->m_wait_carSet.size();

                        QGraphicsProxyWidget*proxy=parkwidget->m_scene->addWidget(car);
                        //动态属性，相当于添加了一个成员变量
                        car->setProperty("proxy",QVariant::fromValue<QGraphicsProxyWidget*>(proxy));
                        showCarInTheWaitQueue(parkwidget);
                    });


                    connect(parkwidget->managewidget,&manageSetDialog::sendGetXY,[=](int xGet,int yGet){
                        if(!parkwidget->parkIsFull()){
                            int row=parkwidget->row;
                            int col=parkwidget->col;
                            while(parkwidget->m_parkset[xGet][yGet]->haveCar){
                                xGet=rand()%row;
                                yGet=rand()%col;
                            }
                            if(parkwidget->m_wait_carSet.size()!=0){
                                Car *car=parkwidget->m_wait_carSet.dequeue();
                                car->x_col=col;
                                car->y_row=row;
                                parkwidget->m_in_carSet.push_back(car);
                                moveIn(xGet,yGet,car);
                            }

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

    setdlg=new SettingDialog(this);
    connect(ui->action_2,&QAction::triggered,[=](){
        if(!setdlg)
        setdlg=new SettingDialog(this);
        setdlg->show();
        connect(setdlg,&SettingDialog::senddata,[=](int x,int y){
            this->setRowCol(x,y);
            emit sendRowCol(x,y);
            //qDebug()<<parkwidget->row<<parkwidget->col;
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
    QVector<int>y_point;

    QGraphicsProxyWidget* proxy=car->property("proxy").value<QGraphicsProxyWidget*>();
    if(proxy==nullptr)return;

    QPointF startPos=proxy->pos();
    QSize size=car->m_size;

    QPointF tempPos=getParkSpaceCenter(xGet,yGet);
    QPointF endPos=QPointF(tempPos.x()-size.width()/2.0,tempPos.y()-size.height()/2.0);

    QPropertyAnimation*anim=new QPropertyAnimation(proxy,"pos");
    anim->setDuration(800);
    anim->setStartValue(startPos);
    anim->setEndValue(endPos);
    anim->setEasingCurve(QEasingCurve::OutQuad);//速度：减缓曲线

    connect(anim,&QPropertyAnimation::finished,[=](){
        parkwidget->m_parkset[xGet][yGet]->haveCar=true;
        anim->deleteLater();
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
    car->x_col=xGet;
    car->y_row=yGet;
}

void MainWindow::moveOut(Car *car)
{
    QGraphicsProxyWidget*proxy=car->property("proxy").value<QGraphicsProxyWidget*>();
    if(proxy==nullptr)return ;
    parkwidget->m_parkset[car->x_col][car->y_row]->haveCar=false;

    Car temp;
    QSize size=temp.m_size;
    double endX=parkwidget->width()-size.width()-20;
    double endY=parkwidget->height()-size.height()-20;
    QPointF endPos(endX,endY);
    QPropertyAnimation*anim=new QPropertyAnimation(proxy,"pos");
    anim->setDuration(800);
    anim->setStartValue(proxy->pos());
    anim->setEndValue(endPos);
    anim->setEasingCurve(QEasingCurve::OutQuad);

    connect(anim,&QPropertyAnimation::finished,[=](){
        car->deleteLater();
        anim->deleteLater();
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
