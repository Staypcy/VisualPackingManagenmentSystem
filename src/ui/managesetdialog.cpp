#include "ui/managesetdialog.h"
#include "ui_managesetdialog.h"
#include<QColorDialog>
#include<QTime>
#include "utils/logger.h"
#include<QTimer>
#include<QMessageBox>
#include<QDateTime>
#include<QFileDialog>
#include<QIcon>

manageSetDialog::manageSetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::manageSetDialog)
    , isAutoEntering(false)
    , selectedImagePath("")
{
    ui->setupUi(this);
    
    // 初始化 parkingMap
    parkingMap.resize(row);
    for(int i = 0; i < row; i++){
        parkingMap[i].resize(col);
        for(int j = 0; j < col; j++){
            parkingMap[i][j] = false; // 初始时所有车位都是空的
        }
    }

    connect(ui->ColorChoice,&QPushButton::clicked,[=](){
        color=QColorDialog::getColor(Qt::white,this,"选择颜色");
    });
    
    connect(ui->ImageChoice, &QPushButton::clicked, [=](){
        QString path = QFileDialog::getOpenFileName(this, "选择车辆图片", "", "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif)");
        if(!path.isEmpty()){
            selectedImagePath = path;
            ui->imagePathLabel->setText(path.mid(path.lastIndexOf("/") + 1));
        }
    });
    
    connect(ui->addCar,&QPushButton::clicked,[=](){
        QString car_name=ui->addCarNum->text();
        QString car_style=ui->addCarStyle->text();
        Car *car=new Car();
        if(!car_name.isEmpty()&&!car_style.isEmpty()){
            if(selectedImagePath.isEmpty()){
                car->setCar(car_name,car_style,color);
            }else{
                car->setCar(car_name,car_style,color,selectedImagePath);
            }
            QSize size=car->m_size;
            car->setFixedSize(size);
            car->setText(car->car_num);
            
            if(!car->car_image_path.isEmpty()){
                QIcon icon(car->car_image_path);
                car->setIcon(icon);
                car->setIconSize(size);
                car->setStyleSheet("border: 2px solid black;");
            }else{
                car->setStyleSheet(QString("background-color: %1; border: 2px solid black;").arg(car->car_color.name()));
            }
            
            emit sendNewCar(car);
            LOG_DEBUG()<<"发送成功";
            
            // 清空图片选择
            selectedImagePath = "";
            ui->imagePathLabel->setText("(可选)");
        }
    });

    connect(ui->CarEnter,&QPushButton::clicked,[=](){
        LOG_DEBUG() << "========== [车辆进入日志] ==========";
        LOG_DEBUG() << "时间:" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        LOG_DEBUG() << "操作: 手动点击 [车辆进入] 按钮";

        int x=rand()%row;
        int y=rand()%col;
        LOG_DEBUG() << "随机选择车位: (" << x << "," << y << ")";
        LOG_DEBUG() << "停车场规模:" << row << "行 x" << col << "列";

        emit sendGetXY(x,y);
        LOG_DEBUG() << "信号已发送: sendGetXY(" << x << "," << y << ")";
        LOG_DEBUG() << "===================================";
    });

    connect(ui->CarEnterAuto,&QPushButton::clicked,[=](){
        if(!isAutoEntering){
            LOG_DEBUG() << "========== [自动驶入日志] ==========";
            LOG_DEBUG() << "时间:" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            LOG_DEBUG() << "操作: 启动自动驶入模式";
            LOG_DEBUG() << "自动驶入间隔: 1500ms";

            isAutoEntering = true;
            ui->CarEnterAuto->setText("停止自动驶入");
            startAutoEnter();

            LOG_DEBUG() << "状态: 自动驶入已启动";
            LOG_DEBUG() << "===================================";
        }else{
            LOG_DEBUG() << "========== [自动驶入日志] ==========";
            LOG_DEBUG() << "时间:" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            LOG_DEBUG() << "操作: 停止自动驶入模式";

            isAutoEntering = false;
            ui->CarEnterAuto->setText("车辆进入（auto）");
            if(autoTimer){
                autoTimer->stop();
                autoTimer->deleteLater();
                autoTimer = nullptr;
            }
        }
    });

    connect(ui->CarOut,&QPushButton::clicked,this,&manageSetDialog::CarToOut);
}

void manageSetDialog::startAutoEnter()
{
    autoTimer = new QTimer(this);
    connect(autoTimer, &QTimer::timeout, this, [=](){
        // 检查是否有等待的车辆
        // 注意：这里无法直接访问 parkingWidget 的队列，需要通过信号传递
        // 简化处理：直接发送信号，让 MainWindow 检查队列
        // 智能寻找最近的空位（优先选择靠近入口的位置）
        int bestX = -1, bestY = -1;
        int minDistance = row + col;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < col; j++){
                // 假设入口在左上角，计算距离
                int distance = i + j;
                if(!parkingMap[i][j] && distance < minDistance){
                    minDistance = distance;
                    bestX = i;
                    bestY = j;
                }
            }
        }
        if(bestX != -1){
            emit sendGetXY(bestX, bestY);
        }else{
            // 没有空位了
            QMessageBox::information(this, "提示", "停车场已满！");
            isAutoEntering = false;
            ui->CarEnterAuto->setText("车辆进入（auto）");
            autoTimer->stop();
            autoTimer->deleteLater();
            autoTimer = nullptr;
        }
    });
    autoTimer->start(1500); // 每 1.5 秒自动驶入一辆车
}

manageSetDialog::~manageSetDialog()
{
    delete ui;
}

void manageSetDialog::setRowCol(int x, int y)
{
    row = x;
    col = y;
    parkingMap.resize(row);
    for(int i = 0; i < row; i++){
        parkingMap[i].resize(col, false);
    }
}

void manageSetDialog::updateParkingMap(QVector<QVector<bool>> map)
{
    parkingMap = map;
}

