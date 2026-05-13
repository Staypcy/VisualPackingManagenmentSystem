#include "loaddialog.h"
#include "ui_loaddialog.h"
#include<QFile>
#include<QTextStream>
#include<QMessageBox>

LoadDialog::LoadDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadDialog)
{
    ui->setupUi(this);
    ui->loadStyle->addItem("管理员登录",0);
    ui->loadStyle->addItem("用户登录",1);

    QString file_name="D:\\QtProject\\VisualParkingManagementSystem\\name_password.txt";
    read_name_passward(file_name);


    connect(ui->close,&QPushButton::clicked,this,&LoadDialog::reject);


    connect(ui->yes,&QPushButton::clicked,[=](){
        QString loadState=ui->loadStyle->currentData().toString();
        load_State=ui->loadStyle->currentData().toInt();
        QString name=ui->name->text();
        QString passward=ui->password->text();
        if(loadState=="0"){
            find_name(loadState,name,passward);
        }
        if(loadState=="1"){
            find_name(loadState,name,passward);
        }
        emit send_loadstate(load_State);
    });

    connect(ui->makenew,&QPushButton::clicked,[=](){
        QString name=ui->name->text();
        QString passward=ui->password->text();
        set_new(name,passward,file_name);
        qDebug()<<"注册成功";
    });
}

LoadDialog::~LoadDialog()
{
    delete ui;
}

void LoadDialog::read_name_passward(const QString &file_name)
{
    QFile file(file_name);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"无法打开文件读取"<<file.errorString();
        return;
    }

    QTextStream in(&file);
    while(!in.atEnd()){
        QString line=in.readLine();
        //0 [name] [passward]
        //->第一个数字（0/1）代表管理员/用户,会整合到passward上，首位加,用空格隔开
        QStringList parts=line.split(' ');
        QString name=parts[1];
        QString passward=parts[0]+' '+parts[2];
        name_set.insert(name,passward);
    }
    file.close();
}

bool LoadDialog::find_name(QString loadstate,const QString &name, const QString &passward)
{
    QString name_find=name;
    QString passward_find=loadstate+' '+passward;
    QMap<QString,QString>::iterator it= name_set.find(name_find);
    if(it!=name_set.end()){
        QString passward_in_name_set=it.value();
        if(passward_find!=passward_in_name_set){
            QMessageBox::warning(this,"错误","密码错误");
        }
        else{
            load_State = loadstate.toInt();
            accept();
            return true;
        }
    }
    return false;
}

void LoadDialog::set_new(const QString &name, const QString &passward,const QString& file_name)
{
    name_set.insert(name,passward);
    QFile file(file_name);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
        qDebug()<<"无法打开文件写入"<<file.errorString();
        return;
    }

    QTextStream out(&file);


    file.close();
}
