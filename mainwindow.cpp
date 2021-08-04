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
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QTime>
#include <QFile>
#include <qcompressor.h>

QString os_type = QSysInfo::kernelType();
QJsonObject my_page_edit;
QString private_key;
QString sign_id;
QString usertoken;
QString username;
QJsonObject userdata;
QString server_url = "https://script.google.com/macros/s/AKfycbzJQUmm1ozWqcv18aeBOje_oh6QZHk3uMzkWQR-kShnS8MV73UgHS5IRxSf8mHNn9Zk/exec";
bool trial_mode= true;
QString AppDir;
int datapack_edit_page_max=1;
int datapack_edit_page_current=1;
int datapack_view_page_max=1;
int datapack_view_page_current=1;
void MainWindow::test(QString params){
    qDebug()<<params;
}
QJsonObject StringToJson(QString str){
        QJsonObject obj;
        QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
        if(!doc.isNull()){
            if(doc.isObject()){
                obj = doc.object();
            }
        }
        return obj;
}
QJsonObject read_level_dat(QString path){
    QFile * nbt = new QFile(path);
    nbt->open(QFile::ReadOnly);
    QByteArray level_dat_row = nbt->readAll();
    nbt->close();
    QByteArray level_dat;
    QJsonObject return__;
    if (QCompressor::gzipDecompress(level_dat_row,level_dat)==true){
        level_dat = QString(level_dat.toHex()).split("0a000756657273696f6e").at(1).toUtf8();
        QList<QByteArray> Hex_Array;
        QByteArray current_processing_hex;

        for (int i=1;i<=level_dat.size();i+=2){
            current_processing_hex = (QString(level_dat.at(i-1))+QString(level_dat.at(i))).toUtf8();
            Hex_Array.append(current_processing_hex);
        }
        int current_pointer=1;
        for (current_pointer=1;current_pointer<=Hex_Array.length();current_pointer++){
            if (Hex_Array.at(current_pointer-1).toUInt(0,16) == 1){
                int name_length = (Hex_Array.at(current_pointer)+Hex_Array.at(current_pointer+1)).toUInt(0,16);
                int value = 0;
                current_pointer+=2;
                QString name;
                for (int i=1;i<=name_length;i++){
                    current_pointer+=1;
                    name += QString(Hex_Array.at(current_pointer-1).toUInt(0,16));
                }
                current_pointer+=1;
                return__.insert(name,value);
            }
            else if (Hex_Array.at(current_pointer-1).toUInt(0,16) == 3){
                int name_length = (Hex_Array.at(current_pointer)+Hex_Array.at(current_pointer+1)).toUInt(0,16);
                int value = 0;
                current_pointer+=2;
                QString name;
                for (int i=1;i<=name_length;i++){
                    current_pointer+=1;
                    name += QString(Hex_Array.at(current_pointer-1).toUInt(0,16));
                }
                current_pointer+=4;
                return__.insert(name,value);
            }
            else if (Hex_Array.at(current_pointer-1).toUInt(0,16) == 8){
                int name_length = (Hex_Array.at(current_pointer)+Hex_Array.at(current_pointer+1)).toUInt(0,16);
                QString value = "";
                current_pointer+=2;
                QString name;
                for (int i=1;i<=name_length;i++){
                    current_pointer+=1;
                    name += QString(Hex_Array.at(current_pointer-1).toUInt(0,16));
                }
                int string_length = (Hex_Array.at(current_pointer)+Hex_Array.at(current_pointer+1)).toUInt(0,16);
                current_pointer+=2;
                for (int i=1;i<=string_length;i++){
                    current_pointer+=1;
                    value += QString(Hex_Array.at(current_pointer-1).toUInt(0,16));
                }
                return__.insert(name,value);
            }
            else if (Hex_Array.at(current_pointer-1).toUInt(0,16) == 0){
                break;
            }
        }
        return return__;
        //qDebug()<<Hex_Array;
    }
    else{
        QMessageBox messageBox;
        messageBox.critical(0,"錯誤","Minecraft Datapack Share Platfrom 遇到了無法預期的錯誤，程式即將停止");
        exit(0);
    }
    //int pointer_data_start;
    //int pointer_data_end;
}
QString post(QString connect_type,QByteArray connect_arg=QString("").toUtf8()){
    qsrand(QTime::currentTime().msec());
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
    ui->LR_username->setEnabled(false);
    ui->LR_password->setEnabled(false);
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
    QString setseesionkeycheck_databeforesend = "id="+id+"&seesion_key="+encryption.encode(QString(private_key).toUtf8(),QString(publickey).toUtf8(),QString(publickey).toUtf8()).toBase64().replace("=","")+"&seesion_key_check="+QCryptographicHash::hash(publickey.toUtf8(),QCryptographicHash::Sha256).toHex();
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
            id += QString((qrand()%79)+45).replace("=","*");
        }

        QString setseesionkeycheck_databeforesend = "id="+id+"&seesion_key="+encryption.encode(QString(private_key).toUtf8(),QString(publickey).toUtf8(),QString(publickey).toUtf8()).toBase64().replace("=","")+"&seesion_key_check="+QCryptographicHash::hash(publickey.toUtf8(),QCryptographicHash::Sha256).toHex();
        setseesionkeycheck = post("set_seesion_key",setseesionkeycheck_databeforesend.toUtf8());
    }
    qDebug()<<private_key;
    if (setseesionkeycheck != "SUCESS"){
        QMessageBox messageBox;
        messageBox.critical(0,"錯誤","Minecraft Datapack Share Platfrom 遇到了無法預期的錯誤，程式即將停止");
        exit(0);
    }
    ui->LR_username->setEnabled(true);
    ui->LR_password->setEnabled(true);
    if (connect_type == "Register"){
        QString reg_data = QString("{\"username\":\""+ui->LR_username->text()+"\",\"password\":\""+QCryptographicHash::hash(ui->LR_password->text().toUtf8(),QCryptographicHash::Sha256).toHex()+"\"}");
        QByteArray data_before_send = "";
        data_before_send+="security_data="+encryption.encode(QString(reg_data).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64().replace("=","")+"&id="+id;
        return post("register",data_before_send);
    }
    if (connect_type == "Login"){
        QString reg_data = QString("{\"username\":\""+ui->LR_username->text()+"\",\"password\":\""+QCryptographicHash::hash(ui->LR_password->text().toUtf8(),QCryptographicHash::Sha256).toHex()+"\"}");
        QByteArray data_before_send = "";
        data_before_send+="security_data="+encryption.encode(QString(reg_data).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64().replace("=","")+"&id="+id;
        QString data_recv = post("login",data_before_send);
        if (data_recv.contains("SUCESS")){
            qDebug()<<"token";
            qDebug()<<data_recv;
            usertoken = encryption.decode(QByteArray::fromBase64(data_recv.split(",").at(1).toUtf8()),QString(private_key).toUtf8(),QString(private_key).toUtf8()).replace("\u0010","");
            return "SUCESS";
        }
        return data_recv;
    }
    if (connect_type == "GET_USR_DATA"){
        if (trial_mode){
            QByteArray data_before_send = "";
            data_before_send += "target_username="+arg;
            QString data_recv = post("get_userdata",data_before_send);
            return data_recv;
        }else{
            QByteArray token_before_send = encryption.encode(QString(usertoken).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64().replace("=","");
            QByteArray data_before_send = "";
            data_before_send += "token="+token_before_send+"&username="+username+"&target_username="+arg+"&id="+id;
            QString data_recv = post("get_userdata",data_before_send);
            return data_recv;
        }
    }
    if (connect_type == "set_userdata"){
        QByteArray token_before_send = encryption.encode(QString(usertoken).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64().replace("=","");
        QByteArray data_before_send = "";
        data_before_send += "token="+token_before_send+"&username="+username+"&id="+id+"&userdata="+QJsonDocument(my_page_edit).toJson(QJsonDocument::Compact).toBase64().replace("=","");
        QString data_recv = post("set_userdata",data_before_send);
        return data_recv;
    }
    if (connect_type == "get_recommend"){
        if (trial_mode){
            QByteArray data_before_send = "";
            data_before_send += "&id="+id;
            QString data_recv = post("recommend_datapack_list",data_before_send);
            return data_recv;
        }else{
            QByteArray token_before_send = encryption.encode(QString(usertoken).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64().replace("=","");
            QByteArray data_before_send = "";
            data_before_send += "token="+token_before_send+"&username="+username+"&id="+id;
            QString data_recv = post("recommend_datapack_list",data_before_send);
            return data_recv;
        }
    }
    return QString("");
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
    qDebug()<<read_level_dat("/home/north-bear/.minecraft/saves/menu_test/level.dat");
    ui->setupUi(this);
    ui->user_icon_setting->setStyleSheet("background-color:rgba(0,0,0,0);border-radius:25%;");
    ui->setuserpage_description->setPlaceholderText("簡介欄位\n點擊即可編輯");
    setStyleSheet("QLabel {color: rgb(255,255,255)} QRadioButton {color: rgb(255,255,255)} QPushButton{background-color: rgb(48, 54, 58);color: rgb(255,255,255)}");
    ui->Login->click();
    ui->show_password->click();
    ui->show_password->click();
    ui->Page->setStyleSheet("QStackedWidget{border:0.5px solid;border-color:rgb(68,74,78);background-color:rgb(0,0,0);border-radius:4px}");
    ui->top_container->setStyleSheet("border:0.5px solid;border-color:rgb(68,74,78);border-radius:4px");
    ui->usericon->setStyleSheet("border:0px solid;border-color:rgb(68,74,78);border-radius:25%;background-color:rgb(255,255,255)");
    QPixmap row_user_icon = QPixmap(":/image/user_icon.png");
    QPixmap big_user_icon = row_user_icon.scaled(51,51);
    ui->usericon->setPixmap(big_user_icon);
    ui->user_icon_setting->setPixmap(big_user_icon);
    if (os_type=="winnt"){
        AppDir = qgetenv("APPDATA")+"/.Minecraft_Datapack_Share_Platfrom";
        if (!QDir(AppDir).exists()){
            QDir(qgetenv("APPDATA")).mkdir(".Minecraft_Datapack_Share_Platfrom");
        }
    }
    else if (os_type=="linux"){
        AppDir = qgetenv("HOME")+"/.Minecraft_Datapack_Share_Platfrom";
        if (!QDir(AppDir).exists()){
            QDir(qgetenv("HOME")).mkdir(".Minecraft_Datapack_Share_Platfrom");
        }
    }
    else if (os_type=="darwin"){
        AppDir = qgetenv("HOME")+"/.Minecraft_Datapack_Share_Platfrom";
        if (!QDir(AppDir).exists()){
            QDir(qgetenv("HOME")).mkdir(".Minecraft_Datapack_Share_Platfrom");
        }
    }
    else{
        AppDir = QDir::currentPath()+"/.Minecraft_Datapack_Share_Platfrom";
        if (!QDir(AppDir).exists()){
            QDir(QDir::currentPath()).mkdir(".Minecraft_Datapack_Share_Platfrom");
        }
    }
    login_page_init();
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    std::cout<<"hi"<<std::endl;
}

void MainWindow::on_userdatasetR_valueChanged(){
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_userdatasetG_valueChanged(){
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_userdatasetB_valueChanged(){
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_userdatasetA_valueChanged(){
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_pushButton_4_clicked(){
    //QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=dQw4w9WgXcQ"));
    QString text = QInputDialog::getText(this," ","我們已經在你的預設瀏覽器打開上傳網站\n上傳後請輸入網站提供的檔案id\n並且在5分鐘內完成設定\n否則存在於上傳的圖檔會被移除",QLineEdit::Normal);
    ui->status->setText("正在嘗試從雲端硬碟取得圖片...");
    if (text != ""){
        ui->background_setuserpage->setPixmap(load_image_from_net(text));
        my_page_edit.insert("background",text);
    }
    ui->status->setText("");
}

void MainWindow::on_pushButton_6_clicked(){
    QString text = QInputDialog::getText(this," ","我們已經在你的預設瀏覽器打開上傳網站\n上傳後請輸入網站提供的檔案id\n並且在5分鐘內完成設定\n否則存在於上傳的圖檔會被移除",QLineEdit::Normal);
    ui->status->setText("正在嘗試從雲端硬碟取得圖片...");
    if (text != ""){
        ui->user_icon_setting->setPixmap(load_image_from_net(text));
        my_page_edit.insert("usericon",text);
    }
    ui->status->setText("");
}
void MainWindow::change_to_homepage(){
    ui->Page->setCurrentWidget(ui->home_page);
    app_init();
}

void MainWindow::after_login_init(){
    if (trial_mode){
        ui->AppContainer->setCurrentWidget(ui->AppPage);
        change_to_homepage();
    }else{
        ui->status->setText("執行伺服器溝通...");
        QString user_data = connect("GET_USR_DATA",username);
        ui->status->setText("");
        if (user_data.contains("SUCESS")){
            qDebug()<<user_data;
            user_data = user_data.split("**mdsp_split_tag**").at(1);
            qDebug()<<user_data;
            if (user_data==""){
                ui->Page->setCurrentWidget(ui->set_user_data_page);
            }else{
                userdata = StringToJson(user_data);
                change_to_homepage();
            }
            ui->AppContainer->setCurrentWidget(ui->AppPage);
        }else{
            qDebug()<<user_data;
            QMessageBox messageBox2;
            messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
            exit(0);
        }
    }
}
void MainWindow::app_init(){
    QString recv = connect("get_recommend");
    if (!recv.contains("SUCESS")){
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);
    }
    qDebug()<<recv;
    recv = recv.split("**mdsp_split_tag**").at(1);
    QJsonObject datapack_list = StringToJson(recv);
    QJsonArray datapacks = datapack_list["datapacks"].toArray();
    qDebug()<<datapacks;
    qDebug()<<datapacks.size();
    ui->MainPageDatapacks->setMinimumWidth(datapacks.size()*101);
    QWidget * recommend_datapack_list_parent_container;
    QPushButton * recommend_datapack_list_picture_show_area;
    QLabel * recommend_datapack_list_datapack_name;
    QSignalMapper * mapper = new QSignalMapper(this);
    for (int i=1;i<=datapacks.size();i++){
        recommend_datapack_list_parent_container = new QWidget(ui->MainPageDatapacks);
        recommend_datapack_list_parent_container->move(101*(i-1),20);
        recommend_datapack_list_parent_container->resize(101,80);
        recommend_datapack_list_picture_show_area = new QPushButton(recommend_datapack_list_parent_container);
        recommend_datapack_list_picture_show_area->move(20,0);
        recommend_datapack_list_picture_show_area->resize(61,61);
        recommend_datapack_list_picture_show_area->setIcon(QIcon(load_image_from_net(datapacks.at(i-1).toObject()["datapack_icon"].toString())));
        recommend_datapack_list_picture_show_area->setIconSize(QSize(61,61));
        recommend_datapack_list_datapack_name = new QLabel(recommend_datapack_list_parent_container);
        recommend_datapack_list_datapack_name->move(0,60);
        recommend_datapack_list_datapack_name->resize(101,20);
        recommend_datapack_list_datapack_name->setText(datapacks.at(i-1).toObject()["datapack_name"].toString());
        recommend_datapack_list_datapack_name->setAlignment(Qt::AlignCenter);
        QObject::connect(recommend_datapack_list_picture_show_area,SIGNAL(clicked()), mapper, SLOT(map()));
        mapper->setMapping(recommend_datapack_list_picture_show_area,QString(datapacks.at(i-1).toObject()["datapack_id"].toString()));
    }
    QObject::connect(mapper,SIGNAL(mapped(QString)),this,SLOT(test(QString)));
}
//http://drive.google.com/uc?id=1oYa_o-9BHagDnGT2hma7DZ1rcnM98hEf&export=download
void MainWindow::login_page_init(){
    if (QFile::exists(AppDir+"/userdata.MDSP")){
        QFile * user_login_data = new QFile(AppDir+"/userdata.MDSP");
        user_login_data->open(QFile::ReadWrite);
        QString user_data_login = user_login_data->readAll();
        user_login_data->close();
        if (user_data_login.split(",").length()==2){
            ui->LR_username->setText(user_data_login.split(",").at(0));
            ui->LR_password->setText(user_data_login.split(",").at(1));
        }
    }
}

void MainWindow::back_to_login_page(){
    ui->AppContainer->setCurrentWidget(ui->Login_RegisterPage);
    ui->LR_status->setText("");
    login_page_init();
}

void MainWindow::on_pushButton_5_clicked(){
    //my_page_edit.insert("name","")
    ui->status->setText("執行使用者暱稱檢查");
    if (ui->setpage_username->text()=="" || ui->setpage_username->text().length()>=16){
        QMessageBox messageBox;
        messageBox.warning(0,"錯誤","使用者暱稱必須介於1到16個字元之間");
        return;
    }
    ui->status->setText("執行自我介紹長度檢查");
    if (ui->setuserpage_description->toPlainText().length()>=16){
        QMessageBox messageBox;
        messageBox.warning(0,"錯誤","自我介紹必須少於16個字元");
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
    ui->status->setText("執行伺服器溝通...");
    QString recv = connect("set_userdata");
    if (recv == "SUCESS"){
        ui->status->setText("");
        change_to_homepage();
        return;
    }
    if (recv == "ERR.TIMEOUT"){
        QMessageBox messageBox;
        messageBox.warning(0,"錯誤","你閒置太久了，請重新登入");
        back_to_login_page();
        return;
    }
        qDebug()<<recv;
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);
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
        ui->LR_status->setText("執行帳號字元檢查...");
        for (int i=1;i<=ui->LR_username->text().length();i++){
            if (!QString("0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~").contains(ui->LR_username->text().at(i-1))){
                QMessageBox messageBox;
                messageBox.warning(0,"錯誤","帳號僅限於以下字元\n0~9、a~z、A~Z、:;<=>[\\]^_'{|}~@");
                ui->LR_status->setText("");
                return;
            }
        }
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
    if (ui->Login->isChecked()){
        username = ui->LR_username->text();
        ui->LR_status->setText("執行伺服器溝通...");
        for (int i=1;i<=ui->LR_username->text().length();i++){
            if (!QString("0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~").contains(ui->LR_username->text().at(i-1))){
                ui->LR_status->setText("帳號或密碼錯誤");
                return;
            }
        }
        QString status = MainWindow::connect("Login");
        if (status=="SUCESS"){
            trial_mode = false;
            qDebug()<<usertoken;
            QFile * user_login_data = new QFile(AppDir+"/userdata.MDSP");
            user_login_data->open(QFile::ReadWrite);
            user_login_data->write(QString(ui->LR_username->text()+","+ui->LR_password->text()).toUtf8());
            user_login_data->close();
            ui->LR_status->setText("執行伺服器溝通...");
            after_login_init();
            return;
        }
        if (status=="ERR.USRORPWD"){
            ui->LR_status->setText("帳號或密碼錯誤");
            return;
        }
        qDebug()<<status;
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);

    }
}

void MainWindow::on_show_password_clicked()
{
    if (ui->show_password->isChecked()==false){
        ui->LR_password->setEchoMode(QLineEdit::Password);
    }
    if (ui->show_password->isChecked()==true){
        ui->LR_password->setEchoMode(QLineEdit::Normal);
    }
}

void MainWindow::on_datapackdatasetR_valueChanged(int arg1)
{
    ui->datapackselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->datapackdatasetR->value())+","+QString::number(ui->datapackdatasetG->value())+","+QString::number(ui->datapackdatasetB->value())+","+QString::number(ui->datapackdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_datapackdatasetG_valueChanged(int arg1)
{
    ui->datapackselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->datapackdatasetR->value())+","+QString::number(ui->datapackdatasetG->value())+","+QString::number(ui->datapackdatasetB->value())+","+QString::number(ui->datapackdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_datapackdatasetB_valueChanged(int arg1)
{
    ui->datapackselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->datapackdatasetR->value())+","+QString::number(ui->datapackdatasetG->value())+","+QString::number(ui->datapackdatasetB->value())+","+QString::number(ui->datapackdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::on_datapackdatasetA_valueChanged(int arg1)
{
    ui->datapackselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->datapackdatasetR->value())+","+QString::number(ui->datapackdatasetG->value())+","+QString::number(ui->datapackdatasetB->value())+","+QString::number(ui->datapackdatasetA->value())+") ; border-radius:25px;");
}

void MainWindow::datapack_set_change_image(){
    ui->datapack_set_picture_index->setText("第 "+QString::number(datapack_edit_page_current)+" / "+QString::number(datapack_edit_page_max)+" 張");
}
void MainWindow::datapack_view_change_image(){
    ui->datapack_view_picture_index->setText("第 "+QString::number(datapack_view_page_current)+" / "+QString::number(datapack_view_page_max)+" 張");
}
void MainWindow::on_pushButton_8_clicked()
{
    if (datapack_edit_page_current-1>0){
        datapack_edit_page_current-=1;
    }
    datapack_set_change_image();
}


void MainWindow::on_pushButton_16_clicked()
{
    if (datapack_edit_page_current+1<=datapack_edit_page_max){
        datapack_edit_page_current+=1;
    }
    datapack_set_change_image();
}

void MainWindow::on_pushButton_20_clicked()
{
    if (datapack_view_page_current-1>0){
        datapack_view_page_current-=1;
    }
    datapack_view_change_image();
}

void MainWindow::on_pushButton_19_clicked()
{
    if (datapack_view_page_current+1<=datapack_view_page_max){
        datapack_view_page_current+=1;
    }
    datapack_view_change_image();
}
