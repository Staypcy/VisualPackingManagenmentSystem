#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

class IFeeService;

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(IFeeService *feeService, QWidget *parent = nullptr);
    ~SettingDialog();
signals:
    void senddata(int row,int col);
private:
    Ui::SettingDialog *ui;
    IFeeService *m_feeService;
};

#endif // SETTINGDIALOG_H
