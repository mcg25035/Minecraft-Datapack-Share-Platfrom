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
    void on_show_password_clicked();
    void on_datapackdatasetR_valueChanged(int arg1);
    void on_datapackdatasetG_valueChanged(int arg1);
    void on_datapackdatasetB_valueChanged(int arg1);
    void on_datapackdatasetA_valueChanged(int arg1);
    void on_pushButton_8_clicked();
    void datapack_set_change_image();
    void datapack_view_change_image();
    void on_pushButton_16_clicked();
    void on_pushButton_20_clicked();
    void on_pushButton_19_clicked();

    void on_edit_datapack_background_clicked();

    void on_edit_datapack_icon_clicked();

    void on_remove_datapack_background_2_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
