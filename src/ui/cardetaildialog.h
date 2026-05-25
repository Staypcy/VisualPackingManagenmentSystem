#ifndef CARDETAILDIALOG_H
#define CARDETAILDIALOG_H

#include <QDialog>
#include <QTimer>
#include "entities/car.h"

class IFeeService;

namespace Ui {
class CarDetailDialog;
}

class CarDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CarDetailDialog(Car* car, IFeeService *feeService, QWidget *parent = nullptr);
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
    IFeeService *m_feeService;
    QTimer* m_timer;
};

#endif // CARDETAILDIALOG_H