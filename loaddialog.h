#ifndef LOADDIALOG_H
#define LOADDIALOG_H

#include <QDialog>
#include<QMap>

namespace Ui {
class LoadDialog;
}

class LoadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadDialog(QWidget *parent = nullptr);
    ~LoadDialog();

    void read_name_passward(const QString& file_name);
    bool find_name(QString loadstate,const QString& name,const QString& passward);
    void set_new(const QString& name,const QString&passward,const QString& file_name);
signals:
    void send_loadstate(int loadstate);
private:
    Ui::LoadDialog *ui;
    QMap<QString,QString>name_set;
public:
    int load_State=-1;
};

#endif // LOADDIALOG_H
