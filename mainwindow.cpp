#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qaesencryption.h"
#include <iostream>
#include <string>
#include <QDebug>
#include <QSignalMapper>
#include <QDesktopServices>
#include <QInputDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QTime>

QJsonObject my_page_edit;
QString private_key;
QString sign_id;
QString usertoken;
QString username;
QString server_url = "https://script.google.com/macros/s/AKfycbwTAOLr2Z724nLfxQfUeB_JO5CQRnumRUqdq_N8N4rdvmO7xcYogQ4Ug8vbL1nlY4BR/exec";

QString AppDir = qgetenv("HOME")+"/.Minecraft_Datapack_Share_Platfrom";
void MainWindow::test(QString params){
    qDebug()<<params;
}
QString post(QString connect_type,QByteArray connect_arg=QString("").toUtf8()){
    qDebug()<<connect_arg;
    QEventLoop eventLoop;
    QNetworkAccessManager nam;
    QUrl url = QUrl(QString(server_url+"?type="+connect_type));
    QNetworkRequest req(url);
    QObject::connect(&nam,SIGNAL(finished(QNetworkReply*)),&eventLoop,SLOT(quit()));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    QNetworkReply * reply = nam.post(req,connect_arg);
    eventLoop.exec();
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()==302){
        QEventLoop eventLoop1;
        QNetworkAccessManager nam1;
        QNetworkRequest req1(reply->header(QNetworkRequest::LocationHeader).toUrl());
        QObject::connect(&nam1,SIGNAL(finished(QNetworkReply*)),&eventLoop1,SLOT(quit()));
        QNetworkReply * reply1 = nam1.get(req1);
        eventLoop1.exec();
        return QString(reply1->readAll());
    }else{
        qDebug()<<QString(reply->readAll());
        qDebug()<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        QMessageBox messageBox;
        messageBox.critical(0,"錯誤","Minecraft Datapack Share Platfrom 遇到了無法預期的錯誤，程式即將停止");
        exit(0);
    }
}


QString MainWindow::connect(QString connect_type,QString arg=""){
    QString publickey = post("get_public");
    qDebug()<<publickey;
    if (publickey.contains("SUCESS")){
        publickey = publickey.split(",").at(1);
    }
    else{
        QMessageBox messageBox;
        messageBox.critical(0,"錯誤","Minecraft Datapack Share Platfrom 遇到了無法預期的錯誤，程式即將停止");
        exit(0);
    }
    QString private_key;
    QString id;
    QAESEncryption encryption(QAESEncryption::AES_128,QAESEncryption::CBC,QAESEncryption::Padding::PKCS7);
    for (int i=1;i<=16;i++){
        private_key += QString((qrand()%79)+45);
    }
    for (int i=1;i<=32;i++){
        id += QString((qrand()%79)+45);
    }
    QString setseesionkeycheck_databeforesend = "id="+id+"&seesion_key="+encryption.encode(QString(private_key).toUtf8(),QString(publickey).toUtf8(),QString(publickey).toUtf8()).toBase64()+"&seesion_key_check="+QCryptographicHash::hash(publickey.toUtf8(),QCryptographicHash::Sha256).toHex();
    QString setseesionkeycheck = post("set_seesion_key",setseesionkeycheck_databeforesend.toUtf8());
    while (setseesionkeycheck=="change_public_key" || setseesionkeycheck=="INFO.IDEXIST"){
        publickey = post("get_public");
        if (publickey.contains("SUCESS")){
            publickey = publickey.split(",").at(1);
        }
        else{
            QMessageBox messageBox;
            messageBox.critical(0,"錯誤","Minecraft Datapack Share Platfrom 遇到了無法預期的錯誤，程式即將停止");
            exit(0);
        }
        private_key = "";
        id = "";
        for (int i=1;i<=16;i++){
            private_key += QString((qrand()%79)+45);
        }
        for (int i=1;i<=32;i++){
            id += QString((qrand()%79)+45);
        }
        QString setseesionkeycheck_databeforesend = "id="+id+"&seesion_key="+encryption.encode(QString(private_key).toUtf8(),QString(publickey).toUtf8(),QString(publickey).toUtf8()).toBase64()+"&seesion_key_check="+QCryptographicHash::hash(publickey.toUtf8(),QCryptographicHash::Sha256).toHex();
        setseesionkeycheck = post("set_seesion_key",setseesionkeycheck_databeforesend.toUtf8());
    }
    qDebug()<<private_key;
    if (setseesionkeycheck == "SUCESS"){
        QMessageBox messageBox;
        messageBox.critical(0,"錯誤","Minecraft Datapack Share Platfrom 遇到了無法預期的錯誤，程式即將停止");
        exit(0);
    }
    if (connect_type == "Register"){
        QString reg_data = QString("{\"username\":\""+ui->LR_username->text()+"\",\"password\":\""+QCryptographicHash::hash(ui->LR_password->text().toUtf8(),QCryptographicHash::Sha256).toHex()+"\"}");
        encryption.encode(QString(reg_data).toUtf8().toBase64(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64();
        QByteArray data_before_send = "";
        data_before_send+="security_data="+encryption.encode(QString(reg_data).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64()+"&id="+id;
        return post("register",data_before_send);
    }
    if (connect_type == "Login"){
        QString reg_data = QString("{\"username\":\""+ui->LR_username->text()+"\",\"password\":\""+QCryptographicHash::hash(ui->LR_password->text().toUtf8(),QCryptographicHash::Sha256).toHex()+"\"}");
        encryption.encode(QString(reg_data).toUtf8().toBase64(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64();
        QByteArray data_before_send = "";
        data_before_send+="security_data="+encryption.encode(QString(reg_data).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64()+"&id="+id;
        QString data_recv = post("login",data_before_send);
        if (data_recv.contains("SUCESS")){
            usertoken = encryption.decode(QString(data_recv.split(",").at(1)).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64();
        }
        return data_recv();
    }

}


QPixmap load_image_from_net(QString fileid){
    QEventLoop eventLoop;
    QNetworkAccessManager mgr;

    QObject::connect(&mgr,SIGNAL(finished(QNetworkReply*)),&eventLoop,SLOT(quit()));
    QUrl url = QUrl("https://drive.google.com/uc?id="+fileid);
    QNetworkRequest req(url);
    QNetworkReply * reply = mgr.get(req);
    eventLoop.exec();
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=302){
        return QPixmap(":/image/no_image.png");
    }else{
        QEventLoop eventLoop1;
        QNetworkAccessManager mgr1;

        QObject::connect(&mgr1,SIGNAL(finished(QNetworkReply*)),&eventLoop1,SLOT(quit()));
        QNetworkRequest req(reply->header(QNetworkRequest::LocationHeader).toUrl());
        QNetworkReply * reply1 = mgr1.get(req);
        eventLoop1.exec();
        QByteArray image = reply1->readAll();
        QPixmap return_ = QPixmap();
        return_.loadFromData(image);
        return return_;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    if (!QDir(AppDir).exists()){
        QDir().mkdir(AppDir);
    }
    ui->setupUi(this);
    ui->user_icon_setting->setStyleSheet("background-color:rgba(0,0,0,0);border-radius:25%;");
    ui->setuserpage_description->setPlaceholderText("簡介欄位\n點擊即可編輯");
    setStyleSheet("QLabel {color: rgb(255,255,255)} QRadioButton {color: rgb(255,255,255)} QPushButton{background-color: rgb(48, 54, 58);color: rgb(255,255,255)}");
    ui->Login->click();
    QString app_path = qgetenv("HOME")+"/.MCDPSP/";
    ui->Page->setStyleSheet("QStackedWidget{border:0.5px solid;border-color:rgb(68,74,78);background-color:rgb(0,0,0);border-radius:4px}");
    ui->top_container->setStyleSheet("border:0.5px solid;border-color:rgb(68,74,78);border-radius:4px");
    ui->usericon->setStyleSheet("border:0px solid;border-color:rgb(68,74,78);border-radius:25%;background-color:rgb(255,255,255)");
    QPixmap row_user_icon = QPixmap(":/image/user_icon.png");
    QPixmap big_user_icon = row_user_icon.scaled(51,51);
    ui->usericon->setPixmap(big_user_icon);
    ui->user_icon_setting->setPixmap(big_user_icon);

    /*
    QPushButton *button[10];
    button[0] = new QPushButton(ui->MainPageDatapacks);
    button[1] = new QPushButton(ui->MainPageDatapacks);
    button[2] = new QPushButton(ui->MainPageDatapacks);
    button[3] = new QPushButton(ui->MainPageDatapacks);
    QSignalMapper * mapper = new QSignalMapper(this);
    button[0]->move(10,0);
    button[1]->move(20,0);
    button[2]->move(30,0);
    button[3]->move(40,0);
    connect(button[0],SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(button[0],QString("never gonna give you up"));
    connect(button[1],SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(button[1],QString("never gonna let you down"));
    connect(button[2],SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(button[2],QString("never gonna run around"));
    connect(button[3],SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(button[3],QString("and desert you"));
    connect(mapper,SIGNAL(mapped(QString)),this,SLOT(test(QString)));
    */
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    std::cout<<"hi"<<std::endl;
}

void MainWindow::on_userdatasetR_valueChanged()
{
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_userdatasetG_valueChanged()
{
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_userdatasetB_valueChanged()
{
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_userdatasetA_valueChanged()
{
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_pushButton_4_clicked()
{
    //QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=dQw4w9WgXcQ"));
    QString text = QInputDialog::getText(this," ","我們已經在你的預設瀏覽器打開上傳網站\n上傳後請輸入網站提供的檔案id\n並且在5分鐘內完成設定\n否則存在於上傳的圖檔會被移除",QLineEdit::Normal);
    ui->status->setText("正在嘗試從雲端硬碟取得圖片...");
    if (text != ""){
        ui->background_setuserpage->setPixmap(load_image_from_net(text));
        my_page_edit.insert("background",text);
    }
    ui->status->setText("");
}

void MainWindow::on_pushButton_6_clicked()
{
    QString text = QInputDialog::getText(this," ","我們已經在你的預設瀏覽器打開上傳網站\n上傳後請輸入網站提供的檔案id\n並且在5分鐘內完成設定\n否則存在於上傳的圖檔會被移除",QLineEdit::Normal);
    ui->status->setText("正在嘗試從雲端硬碟取得圖片...");
    if (text != ""){
        ui->user_icon_setting->setPixmap(load_image_from_net(text));
        my_page_edit.insert("usericon",text);
    }
    ui->status->setText("");
}

void MainWindow::on_pushButton_5_clicked()
{
    //my_page_edit.insert("name","")
    if (ui->setpage_username->text()=="" || ui->setpage_username->text().length()>=16){
        QMessageBox msgBox;
        msgBox.setInformativeText("使用者暱稱必須介於1到16個字元之間");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    if (ui->setuserpage_description->toPlainText().length()>=16){
        QMessageBox msgBox;
        msgBox.setInformativeText("自我介紹必須少於16個字元");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    QJsonArray RGBAArray;
    RGBAArray.append(ui->userdatasetR->value());
    RGBAArray.append(ui->userdatasetG->value());
    RGBAArray.append(ui->userdatasetB->value());
    RGBAArray.append(ui->userdatasetA->value());
    if (!my_page_edit.contains("usericon")){
        my_page_edit.insert("usericon","1PB5yMWvQO1UfgPYxLJe_m1ClipbFX1YQ");
    }
    if (!my_page_edit.contains("background")){
        my_page_edit.insert("background","1vuqZlBs0UvQBpSauTmKg9zl-U6Jf430F");
    }
    my_page_edit.insert("name",ui->setpage_username->text());
    my_page_edit.insert("description",ui->setuserpage_description->toPlainText());
    my_page_edit.insert("display_board_color",RGBAArray);
    qDebug()<<my_page_edit;
}

void MainWindow::on_LR_Trigger_clicked()
{
    if (ui->Register->isChecked()){
        ui->LR_status->setText("執行帳密長度檢查...");
        if (ui->LR_username->text().length()<8 || ui->LR_username->text().length()>100){
            QMessageBox messageBox;
            messageBox.warning(0,"錯誤","帳號必須介於8到100個字元之間");
            ui->LR_status->setText("");
            return;
        }
        if (ui->LR_password->text().length()<8){
            QMessageBox messageBox;
            messageBox.warning(0,"錯誤","密碼必須多於8個字元");
            ui->LR_status->setText("");
            return;
        }
        ui->LR_status->setText("執行伺服器溝通...");
        QString status = MainWindow::connect("Register");
        if (status=="SUCESS"){
            ui->LR_status->setText("註冊成功!");
            return;
        }
        if (status=="ERR.REGED"){
            ui->LR_status->setText("註冊失敗，已經有同帳號使用者存在!");
            return;
        }
        qDebug()<<status;
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);

    }
}
