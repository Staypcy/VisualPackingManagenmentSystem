#ifndef LOADDIALOG_H
#define LOADDIALOG_H

#include <QDialog>

class IAuthService;

namespace Ui {
class LoadDialog;
}

class LoadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadDialog(IAuthService *authService, QWidget *parent = nullptr);
    ~LoadDialog();

signals:
    void send_loadstate(int loadstate);

private:
    Ui::LoadDialog *ui;
    IAuthService *m_authService;

public:
    int load_State = -1;
};

#endif // LOADDIALOG_H
