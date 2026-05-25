#include "ui/loaddialog.h"
#include "ui_loaddialog.h"
#include "utils/logger.h"
#include "services/iauthservice.h"
#include <QMessageBox>

LoadDialog::LoadDialog(IAuthService *authService, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadDialog)
    , m_authService(authService)
{
    ui->setupUi(this);
    ui->loadStyle->addItem("管理员登录", 0);
    ui->loadStyle->addItem("用户登录", 1);

    connect(ui->close, &QPushButton::clicked, this, &LoadDialog::reject);

    connect(ui->yes, &QPushButton::clicked, [=]() {
        int role = ui->loadStyle->currentData().toInt();
        QString name = ui->name->text();
        QString password = ui->password->text();

        if (name.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "错误", "用户名和密码不能为空");
            return;
        }

        int authenticatedRole = -1;
        if (m_authService->authenticate(name, password, authenticatedRole)) {
            if (authenticatedRole == role || authenticatedRole == 0) {
                // Admin can login as any role; user can only login as user
                load_State = role;
                emit send_loadstate(load_State);
                LOG_INFO() << "Login successful for" << name << "as role" << role;
                accept();
            } else {
                QMessageBox::warning(this, "错误", "权限不足");
                LOG_WARNING() << "Insufficient permissions for" << name;
            }
        } else {
            QMessageBox::warning(this, "错误", "用户名或密码错误");
            LOG_WARNING() << "Login failed for" << name;
        }
    });

    connect(ui->makenew, &QPushButton::clicked, [=]() {
        QString name = ui->name->text();
        QString password = ui->password->text();

        if (name.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "错误", "用户名和密码不能为空");
            return;
        }

        int role = ui->loadStyle->currentData().toInt();
        if (m_authService->registerUser(name, password, role)) {
            QMessageBox::information(this, "提示", "注册成功");
            LOG_INFO() << "User registered:" << name << "role:" << role;
        } else {
            QMessageBox::warning(this, "错误", "注册失败，用户可能已存在");
        }
    });
}

LoadDialog::~LoadDialog()
{
    delete ui;
}
