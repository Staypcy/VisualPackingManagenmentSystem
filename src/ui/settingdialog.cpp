#include "ui/settingdialog.h"
#include "ui_settingdialog.h"
#include "utils/logger.h"
#include "services/ifeeservice.h"
#include<QMessageBox>

SettingDialog::SettingDialog(IFeeService *feeService, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingDialog)
    , m_feeService(feeService)
{
    ui->setupUi(this);

    // 加载当前计费规则
    FeeRule rule = m_feeService->getRule();
    ui->firstHourRate->setValue(rule.firstHourRate);
    ui->additionalHourRate->setValue(rule.additionalHourRate);
    ui->dailyMax->setValue(rule.dailyMax);
    ui->freeMinutes->setValue(rule.freeMinutes);

    connect(ui->yes,&QPushButton::clicked,[=](){
        int row=ui->row->value();
        int col=ui->col->value();
        LOG_DEBUG()<<row<<" "<<col;

        // 保存计费规则
        FeeRule rule;
        rule.firstHourRate = ui->firstHourRate->value();
        rule.additionalHourRate = ui->additionalHourRate->value();
        rule.dailyMax = ui->dailyMax->value();
        rule.freeMinutes = ui->freeMinutes->value();

        m_feeService->setRule(rule);

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
