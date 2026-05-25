#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#pragma once
#include <QMainWindow>
#include"loaddialog.h"
#include "parkingwidget.h"
#include "settingdialog.h"

class IFeeService;
class IAuthService;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(IFeeService *feeService, IAuthService *authService, QWidget *parent = nullptr);
    void showCarInTheWaitQueue(parkingWidget *pw);
    ~MainWindow() override;

    void paintEvent(QPaintEvent *event);

    void moveIn(int xGet, int yGet, Car *car);
    void moveOut(Car* car);
    QPointF getParkSpaceCenter(int x,int y);
public slots:
    void setRowCol(int row1,int col1){row=row1;col=col1;}
signals:
    void sendRowCol(int x,int y);
private:
    Ui::MainWindow *ui;
    IFeeService *m_feeService;
    IAuthService *m_authService;
    LoadDialog* loaddlg;
    parkingWidget* parkwidget;
    SettingDialog*setdlg;
    QMenu* manageMenu;
    int load_state=-1;

    int row=4;
    int col=4;
    
    bool isCarEntering = false; // 防止快速点击导致同一车位停多辆车

};
#endif // MAINWINDOW_H
