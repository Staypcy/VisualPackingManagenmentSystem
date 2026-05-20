#include "cardetaildialog.h"
#include "ui_cardetaildialog.h"
#include "feeservice.h"
#include<QDateTime>
#include<QMessageBox>

CarDetailDialog::CarDetailDialog(Car* car, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CarDetailDialog)
    , m_car(car)
    , m_timer(nullptr)
{
    ui->setupUi(this);
    
    // 初始化显示车辆信息
    ui->carNumEdit->setText(car->car_num);
    ui->carStyleEdit->setText(car->car_style);
    ui->positionLabel->setText(QString("(%1, %2)").arg(car->x_col + 1).arg(car->y_row + 1));
    ui->enterTimeLabel->setText(car->enter_time.toString("yyyy-MM-dd HH:mm:ss"));
    
    // 启动定时器，每秒更新一次
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &CarDetailDialog::onTimerUpdate);
    m_timer->start(1000);
    
    updateFee();
}

CarDetailDialog::~CarDetailDialog()
{
    delete ui;
}

void CarDetailDialog::updateFee()
{
    QDateTime now = QDateTime::currentDateTime();
    qint64 seconds = m_car->enter_time.secsTo(now);
    
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    ui->durationLabel->setText(QString("%1小时%2分钟%3秒").arg(hours).arg(minutes).arg(secs));
    
    double fee = FeeService::getInstance()->calculateFee(m_car->enter_time, now);
    ui->feeLabel->setText(QString("%1元").arg(fee, 0, 'f', 2));
}

void CarDetailDialog::on_saveBtn_clicked()
{
    QString newCarNum = ui->carNumEdit->text();
    QString newCarStyle = ui->carStyleEdit->text();
    
    if (newCarNum.isEmpty()) {
        QMessageBox::warning(this, "警告", "车牌号不能为空");
        return;
    }
    
    m_car->car_num = newCarNum;
    m_car->car_style = newCarStyle;
    m_car->setText(newCarNum);
    
    emit carInfoUpdated(m_car);
    
    QMessageBox::information(this, "提示", "信息已更新");
}

void CarDetailDialog::on_closeBtn_clicked()
{
    if(m_timer){
        m_timer->stop();
    }
    close();
}

void CarDetailDialog::onTimerUpdate()
{
    updateFee();
}