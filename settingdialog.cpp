#include "settingdialog.h"
#include "ui_settingdialog.h"
#include "feeservice.h"
#include<QMessageBox>

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    
    // 加载当前计费规则
    FeeRule rule = FeeService::getInstance()->getRule();
    ui->firstHourRate->setValue(rule.firstHourRate);
    ui->additionalHourRate->setValue(rule.additionalHourRate);
    ui->dailyMax->setValue(rule.dailyMax);
    ui->freeMinutes->setValue(rule.freeMinutes);
    
    connect(ui->yes,&QPushButton::clicked,[=](){
        int row=ui->row->value();
        int col=ui->col->value();
        qDebug()<<row<<" "<<col;
        
        // 保存计费规则
        FeeRule rule;
        rule.firstHourRate = ui->firstHourRate->value();
        rule.additionalHourRate = ui->additionalHourRate->value();
        rule.dailyMax = ui->dailyMax->value();
        rule.freeMinutes = ui->freeMinutes->value();
        
        FeeService::getInstance()->setRule(rule);
        
        emit senddata(row,col);
        
        QMessageBox::information(this, "提示", "设置已保存");
        accept();
    });
    
    connect(ui->cancel, &QPushButton::clicked, this, &QDialog::reject);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}
