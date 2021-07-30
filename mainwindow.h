#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();
    void test(QString params);
    void on_userdatasetR_valueChanged();
    void on_userdatasetG_valueChanged();
    void on_userdatasetB_valueChanged();
    void on_userdatasetA_valueChanged();
    void on_pushButton_4_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_5_clicked();
    QString connect(QString connect_type,QString arg);
    void on_LR_Trigger_clicked();
    void after_login_init();
    void change_to_homepage();
    void app_init();
    void login_page_init();
    void back_to_login_page();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
