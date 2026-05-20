#ifndef CARDETAILDIALOG_H
#define CARDETAILDIALOG_H

#include <QDialog>
#include <QTimer>
#include "managesetdialog.h"

namespace Ui {
class CarDetailDialog;
}

class CarDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CarDetailDialog(Car* car, QWidget *parent = nullptr);
    ~CarDetailDialog();
    
    void updateFee();
    
signals:
    void carInfoUpdated(Car* car);
    
private slots:
    void on_saveBtn_clicked();
    void on_closeBtn_clicked();
    void onTimerUpdate();

private:
    Ui::CarDetailDialog *ui;
    Car* m_car;
    QTimer* m_timer;
};

#endif // CARDETAILDIALOG_H