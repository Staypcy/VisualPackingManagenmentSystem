#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    connect(ui->yes,&QPushButton::clicked,[=](){
        int row=ui->row->value();
        int col=ui->col->value();
        qDebug()<<row<<" "<<col;
        emit senddata(row,col);
    });
}

SettingDialog::~SettingDialog()
{
    delete ui;
}
