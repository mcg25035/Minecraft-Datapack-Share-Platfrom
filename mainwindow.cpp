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
#include <QFontDatabase>
#include <QTime>
#include <QApplication>
#include <QGuiApplication>
#include <QFile>
#include <qcompressor.h>
#include <QThread>
#include <QWidget>
#include <QMovie>
#include <QWaitCondition>
#include <QFont>
#include <QMutex>
#include <QTimer>
QString test_;
QString test_2;
QString os_type = QSysInfo::kernelType();
QJsonObject my_page_edit;
QJsonObject datapack_view_page_data;
QJsonObject datapack_page_edit;
QJsonArray datapack_page_background;
QJsonArray pre_set_datapack;
QJsonArray world_list;
QString private_key;
QString sign_id;
QString usertoken;
QString username;
QJsonObject userdata;
QString server_url = "https://script.google.com/macros/s/AKfycbzhu1XVOnL_7VEMK-leVlHfHDe60BUOqBjuuJhiJnpNcGk8geZ4NqLWLyQxX8LmWTnC/exec";
bool trial_mode= true;
bool login_on = false;
QString AppDir;
QString McDir;
QList <QPixmap> thread_return_pixmap;
QList <QByteArray> thread_return_files;
int datapack_edit_page_max=0;
int datapack_edit_page_current=0;
int datapack_view_page_max=0;
int datapack_view_page_current=1;
QList <QPixmap> view_page_background_list;
QStringList bug_information;
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

QByteArray load_bytearray_from_net(QString fileid,int index){
    QEventLoop eventLoop;
    QNetworkAccessManager mgr;

    QObject::connect(&mgr,SIGNAL(finished(QNetworkReply*)),&eventLoop,SLOT(quit()));
    QUrl url = QUrl("https://script.google.com/macros/s/AKfycbwBcOaz1nTvaWTohIIj7cacvgInYLdJSVPFZd5-LvvJhzelM3LSNFRC6NRjSIWgD3AZ/exec?fileid="+fileid);
    QNetworkRequest req(url);
    QNetworkReply * reply = mgr.get(req);
    eventLoop.exec();
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=302){
        qDebug()<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        thread_return_files.removeAt(index);
        thread_return_files.insert(index,QString("error").toUtf8());
        return QString("error").toUtf8();
    }else{
        QEventLoop eventLoop1;
        QNetworkAccessManager mgr1;
        QObject::connect(&mgr1,SIGNAL(finished(QNetworkReply*)),&eventLoop1,SLOT(quit()));
        QNetworkRequest req(reply->header(QNetworkRequest::LocationHeader).toUrl());
        QNetworkReply * reply1 = mgr1.get(req);
        eventLoop1.exec();
        QByteArray return_ = QByteArray::fromBase64(reply1->readAll());
        thread_return_files.removeAt(index);
        thread_return_files.insert(index,return_);
        return return_;
    }
}

QPixmap load_image_from_net(QString fileid,int index){
    QEventLoop eventLoop;
    QNetworkAccessManager mgr;

    QObject::connect(&mgr,SIGNAL(finished(QNetworkReply*)),&eventLoop,SLOT(quit()));
    QUrl url = QUrl("https://script.google.com/macros/s/AKfycbwBcOaz1nTvaWTohIIj7cacvgInYLdJSVPFZd5-LvvJhzelM3LSNFRC6NRjSIWgD3AZ/exec?fileid="+fileid);
    QNetworkRequest req(url);
    QNetworkReply * reply = mgr.get(req);
    eventLoop.exec();
    if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=302){
        qDebug()<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        thread_return_pixmap.removeAt(index);
        thread_return_pixmap.insert(index,QPixmap(":/image/no_image.png"));
        return QPixmap(":/image/no_image.png");
    }else{
        QEventLoop eventLoop1;
        QNetworkAccessManager mgr1;

        QObject::connect(&mgr1,SIGNAL(finished(QNetworkReply*)),&eventLoop1,SLOT(quit()));
        QNetworkRequest req(reply->header(QNetworkRequest::LocationHeader).toUrl());
        QNetworkReply * reply1 = mgr1.get(req);
        eventLoop1.exec();
        QByteArray image = QByteArray::fromBase64(reply1->readAll());
        QPixmap return_ = QPixmap();
        return_.loadFromData(image);
        thread_return_pixmap.removeAt(index);
        thread_return_pixmap.insert(index,return_);
        return return_;
    }
}

QList <QPixmap> batch_get_image(QStringList image_id_list){
    QList <QThread *> thread_list;
    thread_return_pixmap.clear();
    for (int i=0;i<=image_id_list.length()-1;i++){
        thread_return_pixmap.append(QPixmap(":/image/no_image.png"));
        thread_list.append(QThread::create(load_image_from_net,image_id_list.at(i),i));
        thread_list.at(i)->start();
    }
    for (int i=0;i<=thread_list.length()-1;i++){
        thread_list.at(i)->wait();
    }
    return thread_return_pixmap;
}

QList <QByteArray> batch_get_files(QStringList fileidlist){
    QList <QThread *> thread_list;
    thread_return_files.clear();
    for (int i=0;i<=fileidlist.length()-1;i++){
        thread_return_files.append("");
        thread_list.append(QThread::create(load_bytearray_from_net,fileidlist.at(i),i));
        thread_list.at(i)->start();
    }
    for (int i=0;i<=thread_list.length()-1;i++){
        thread_list.at(i)->wait();
    }
    return thread_return_files;
}

QStringList init_map_MSDP_read(QString map_name){
    QFile* if_map_exist = new QFile(McDir+"/"+map_name+"/level.dat");
    if (!if_map_exist->exists()){
        QStringList return_;
        return_.append("ERR");
        return return_;
    }
    QFile* map_information = new QFile(McDir+"/"+map_name+"/Map_Information.MDSP");
    if (!map_information->exists()){
        map_information->open(QFile::WriteOnly);
        map_information->write(QString("{\"datapacks\":[]}").toUtf8());
        map_information->resize(map_information->pos());
        map_information->close();
        return QStringList();
    }
    else
    {
        map_information->open(QFile::ReadOnly);
        QJsonArray datapacks_list = StringToJson(QString(map_information->readAll())).value("datapacks").toArray();
        map_information->close();
        QStringList return_;
        for (int i=0;i<=datapacks_list.size()-1;i++){
            return_+=datapacks_list.at(i).toString();
        }
        return return_;
    }
}

bool version_detector(QString top,QString floor,QString reference){
    if (!reference.contains(".")){
        return false;
    }
    int swap_area = 0;
    int top_ = top.split(".").at(1).toInt();
    int floor_ = floor.split(".").at(1).toInt();
    int reference_ = reference.split(".").at(1).toInt();
    if (top_>floor_){
        bool a = reference_>=floor_;
        bool b = top_>=reference_;
        return a&&b;
    }
    else if (top_<floor_){
        swap_area = top_;
        top_ = floor_;
        floor_ = swap_area;
        bool a = reference_>=floor_;
        bool b = top_>=reference_;
        return a&&b;
    }
    else if (top_==floor_){
        return top_==reference_;
    }

}
void MainWindow::test(QString params){
    open_datapack_page(params);
}

QJsonObject read_level_dat(QString path){
    QFile * nbt = new QFile(path);
    nbt->open(QFile::ReadOnly);
    QByteArray level_dat_row = nbt->readAll();
    nbt->close();
    QByteArray level_dat;
    QJsonObject return__;
    if (QCompressor::gzipDecompress(level_dat_row,level_dat)==true || level_dat_row==""){
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
        QJsonObject ERR_OBJ;
        ERR_OBJ.insert("ERR",1);
        return ERR_OBJ;
    }
    //int pointer_data_start;
    //int pointer_data_end;
}

void detect_maps(){
    QStringList world_list_row = QDir(McDir).entryList();
    QJsonArray empty_json_array = QJsonArray();
    world_list = empty_json_array;
    world_list_row.removeAll(".");
    world_list_row.removeAll("..");
    qDebug()<<McDir;
    for (int i=0;i<=world_list_row.length()-1;i++){
        if (QFile(McDir+"/"+world_list_row.at(i)+"/level.dat").exists()){
            QJsonObject map;
            map.insert("Name",world_list_row.at(i));
            qDebug()<<"a";
            if (read_level_dat(McDir+"/"+world_list_row.at(i)+"/level.dat").contains("ERR")){
                continue;
            }
            map.insert("Version",read_level_dat(McDir+"/"+world_list_row.at(i)+"/level.dat").value("Name").toString());
            qDebug()<<"b";
            world_list.append(map);
        }
    }
}

QString post(QString connect_type,QByteArray connect_arg=QString("").toUtf8(),bool this_time_enable_bug_report=true){
    if (bug_information.size()>=5){
        bug_information.removeFirst();
    }
    bug_information.append(connect_arg);
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
        messageBox.critical(0,"錯誤","Minecraft Datapack Share Platfrom 遇到了無法預期的錯誤，程式即將停止 \n 此次崩潰無法製造八哥蕊破！");
        exit(0);
    }
}

void MainWindow::bug_report_type_1(QString crash_at, QString crash_recv){
    QString report_data;
    report_data+="Program crash at "+crash_at+".data recieve before crash : \""+crash_recv+"\",connect argument before crash : \"";
    for (int i=0;i<=bug_information.size()-1;i++){
        report_data+=bug_information.at(i);
        report_data+=",";
    }
    report_data+="\"";
    ui->Page->setCurrentWidget(ui->loading_page);
    ui->LR_status->setText("正在執行八哥蕊破...");
    ui->status->setText("正在執行八哥蕊破...");
    QByteArray data_before_send;
    data_before_send+="report="+report_data.toUtf8().toBase64();
    post("bug_report",data_before_send,false);

}

void MainWindow::de_timeout_thread_code(){
    QString recv = MainWindow::connect("GET_USR_DATA",username);
    if (!recv.contains("SUCESS")){
        bug_report_type_1("de_timeout_thread",recv);
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);
    }
}

QString MainWindow::connect(QString connect_type,QString arg=""){
    qsrand(QTime::currentTime().msec());
    QString publickey = post("get_public");
    qDebug()<<publickey;
    if (publickey.contains("SUCESS")){
        publickey = publickey.split(",").at(1);
    }
    else{
        bug_report_type_1("getting public key",publickey);
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);
    }
    QString private_key;
    QString id;
    QString generate_char;
    QAESEncryption encryption(QAESEncryption::AES_128,QAESEncryption::CBC,QAESEncryption::Padding::PKCS7);
    for (int i=1;i<=16;i++){
        generate_char = QString((qrand()%79)+45);
        if (QString("0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_").contains(generate_char)){
            private_key+=generate_char;
        }else{
            private_key+="_";
        }
    }
    for (int i=1;i<=32;i++){
        generate_char = QString((qrand()%79)+45);
        if (QString("0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_").contains(generate_char)){
            id+=generate_char;
        }else{
            id+="_";
        }
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
            (0);
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
        (0);
    }
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
            qDebug()<<data_before_send;
            QString data_recv = post("recommend_datapack_list",data_before_send);
            return data_recv;
        }
    }
    if (connect_type == "upload_datapack"){
        QByteArray token_before_send = encryption.encode(QString(usertoken).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64().replace("=","");
        QByteArray data_before_send = "";
        data_before_send += "token="+token_before_send+"&username="+username+"&id="+id+"&datapack_data="+QJsonDocument(datapack_page_edit).toJson(QJsonDocument::Compact).toBase64();
        QString data_recv = post("upload_datapack",data_before_send);
        return data_recv;
    }
    if (connect_type == "get_datapack"){
        QByteArray data_before_send = "";
        if (trial_mode){
            data_before_send += "id="+id+"&datapack_id="+arg;
        }else{
            QByteArray token_before_send = encryption.encode(QString(usertoken).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64().replace("=","");
            data_before_send += "token="+token_before_send+"&username="+username+"&id="+id+"&datapack_id="+arg;
        }
        QString data_recv = post("get_datapack",data_before_send);
        qDebug()<<data_recv;
        return data_recv;
    }
    if (connect_type == "get_all_preset_datapack"){
        QByteArray data_before_send = "";
        if (trial_mode){
            data_before_send += "id="+id+"&datapackid="+ui->datapack_view_id->text();
        }else{
            QByteArray token_before_send = encryption.encode(QString(usertoken).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64().replace("=","");
            data_before_send += "token="+token_before_send+"&username="+username+"&id="+id+"&datapackid="+ui->datapack_view_id->text();
        }
        QString data_recv = post("get_all_preset_datapack",data_before_send);
        return data_recv;
    }
    if (connect_type == "search"){
        QByteArray data_before_send = "";
        if (trial_mode){
            data_before_send += "id="+id+"&keyword="+ui->serachbox->text();
        }else{
            QByteArray token_before_send = encryption.encode(QString(usertoken).toUtf8(),QString(private_key).toUtf8(),QString(private_key).toUtf8()).toBase64().replace("=","");
            data_before_send += "token="+token_before_send+"&username="+username+"&id="+id+"&keyword="+ui->serachbox->text().toUtf8().toBase64();
        }
        qDebug()<<data_before_send;
        QString data_recv = post("search",data_before_send);
        return data_recv;
    }
    return QString("");
}



int test_thread(QString arg){
    qDebug()<< arg;
    return 0;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    if (!QDir(AppDir).exists()){
        QDir().mkdir(AppDir);
    }
    /*
    QList <QThread *> aaa;
    aaa.append(QThread::create(test_thread,QString("Never gonna give you up")));
    aaa.append(QThread::create(test_thread,QString("Never gonna let you down")));
    aaa.append(QThread::create(test_thread,QString("Never gonna run around")));
    aaa.append(QThread::create(test_thread,QString("And Desert You")));
    aaa.at(0)->start();
    aaa.at(1)->start();
    aaa.at(2)->start();
    aaa.at(0)->wait();
    aaa.at(1)->wait();
    aaa.at(2)->wait();
    (0);
    */
    //qDebug()<<read_level_dat("/home/north-bear/.minecraft/saves/menu_test/level.dat");
    ui->setupUi(this);
    ui->uisettings->setVisible(false);
    QFontDatabase FontDatabase;
    QStringList fonts = FontDatabase.families();
    for (int i=0;i<=fonts.size()-1;i++){
        ui->font->addItem(fonts.at(i));
    }
    QMovie* loading = new QMovie(":/image/loading.gif");
    ui->loading->setMovie(loading);
    ui->user_icon_setting->setStyleSheet("background-color:rgba(0,0,0,0);border-radius:25%;");
    ui->setuserpage_description->setPlaceholderText("點擊即可編輯簡介");
    setStyleSheet("QLabel {color: rgb(255,255,255)} QRadioButton {color: rgb(255,255,255)} QPushButton{background-color: rgb(48, 54, 58);color: rgb(255,255,255)} QScrollArea{background: rgb(48, 54, 58)};");
    ui->Login->click();
    ui->show_password->click();
    ui->show_password->click();
    ui->Page->setStyleSheet("QStackedWidget{border:0.5px solid;border-color:rgb(68,74,78);background-color:rgb(0,0,0);border-radius:4px}");
    ui->top_container->setStyleSheet("border:0.5px solid;border-color:rgb(68,74,78);border-radius:4px");
    ui->usericon->setStyleSheet("border:0px solid;border-color:rgb(68,74,78);border-radius:25%;");
    QPixmap row_user_icon = QPixmap(":/image/user_icon.png");
    QPixmap big_user_icon = row_user_icon.scaled(51,51);
    QPixmap row_datapack_icon = QPixmap(":/image/datapackicon.png");
    QPixmap big_datapack_icon = row_datapack_icon.scaled(51,51);
    ui->datapack_icon_setting->setPixmap(big_datapack_icon);
    ui->user_icon_setting->setPixmap(big_user_icon);
    if (os_type=="winnt"){
        AppDir = qgetenv("APPDATA")+"/.Minecraft_Datapack_Share_Platfrom";
        if (!QDir(AppDir).exists()){
            QDir(qgetenv("APPDATA")).mkdir(".Minecraft_Datapack_Share_Platfrom");
        }
        McDir = qgetenv("APPDATA");
    }
    else if (os_type=="linux"){
        AppDir = qgetenv("HOME")+"/.Minecraft_Datapack_Share_Platfrom";
        if (!QDir(AppDir).exists()){
            QDir(qgetenv("HOME")).mkdir(".Minecraft_Datapack_Share_Platfrom");
        }
        McDir = qgetenv("HOME");
    }
    else if (os_type=="darwin"){
        AppDir = qgetenv("HOME")+"/.Minecraft_Datapack_Share_Platfrom";
        if (!QDir(AppDir).exists()){
            QDir(qgetenv("HOME")).mkdir(".Minecraft_Datapack_Share_Platfrom");
        }
        McDir = qgetenv("HOME")+"/Library/Application Support/minecraft/saves";
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
    QFont font = QFont(ui->font->currentText());
    font.setPixelSize(ui->font_size->value());
    QList<QWidget*> widgets = ((QObject*)ui->centralwidget)->findChildren<QWidget*>(QString(),Qt::FindChildrenRecursively);
    for (int i=0;i<=widgets.size()-1;i++){
        widgets.at(i)->setFont(font);
        widgets.at(i)->update();
        qDebug()<<widgets.at(i);
        qDebug()<<font;
    }
    font.setPixelSize(ui->font_size->value()+6);
    ui->Loading1->setFont(font);
    font.setPixelSize(ui->font_size->value()+5);
    ui->Loading1->setFont(font);
    ui->uisettings->setVisible(false);
    qApp->processEvents();
}

void MainWindow::on_userdatasetR_valueChanged(){
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:15px;");
}

void MainWindow::on_userdatasetG_valueChanged(){
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:15px;");
}

void MainWindow::on_userdatasetB_valueChanged(){
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:15px;");
}

void MainWindow::on_userdatasetA_valueChanged(){
    ui->userselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->userdatasetR->value())+","+QString::number(ui->userdatasetG->value())+","+QString::number(ui->userdatasetB->value())+","+QString::number(ui->userdatasetA->value())+") ; border-radius:15px;");
}

void MainWindow::on_pushButton_4_clicked(){
    QDesktopServices::openUrl(QUrl("https://script.google.com/macros/s/AKfycbwBcOaz1nTvaWTohIIj7cacvgInYLdJSVPFZd5-LvvJhzelM3LSNFRC6NRjSIWgD3AZ/exec"));
    QString text = QInputDialog::getText(this," ","程式已經在你的預設瀏覽器打開上傳網站\n上傳後請輸入網站提供的檔案id\n並且在12小時內完成設定\n否則存在於上傳的圖檔會被移除",QLineEdit::Normal);
    ui->status->setText("正在嘗試從雲端硬碟取得圖片...");
    ui->pushButton_4->setEnabled(false);
    if (text != ""){
        ui->background_setuserpage->setPixmap(load_image_from_net(text,2).scaled(731,381));
        if (my_page_edit.contains("background")){
            my_page_edit.remove("background");
        }
        my_page_edit.insert("background",text);
    }
    ui->status->setText("");
    ui->pushButton_4->setEnabled(true);
}

void MainWindow::on_pushButton_6_clicked(){
    QDesktopServices::openUrl(QUrl("https://script.google.com/macros/s/AKfycbwBcOaz1nTvaWTohIIj7cacvgInYLdJSVPFZd5-LvvJhzelM3LSNFRC6NRjSIWgD3AZ/exec"));
    QString text = QInputDialog::getText(this," ","程式已經在你的預設瀏覽器打開上傳網站\n上傳後請輸入網站提供的檔案id\n並且在12小時內完成設定\n否則存在於上傳的圖檔會被移除",QLineEdit::Normal);
    ui->status->setText("正在嘗試從雲端硬碟取得圖片...");
    ui->pushButton_6->setEnabled(false);
    if (text != ""){
        ui->user_icon_setting->setPixmap(load_image_from_net(text,2).scaled(51,51));
        if (my_page_edit.contains("usericon")){
            my_page_edit.remove("usericon");
        }
        my_page_edit.insert("usericon",text);
    }
    ui->status->setText("");
    ui->pushButton_6->setEnabled(true);
}
void MainWindow::change_to_homepage(){
    ui->main_username->setText(userdata.value("name").toString());
    app_init();
}

void MainWindow::after_login_init(){
    if (trial_mode){
        change_to_homepage();
    }else{
        ui->status->setText("執行伺服器溝通...");
        ui->Page->setCurrentWidget(ui->loading_page);
        ui->AppContainer->setCurrentWidget(ui->AppPage);
        QString user_data_ = connect("GET_USR_DATA",username);
        qDebug()<<user_data_;
        ui->status->setText("");
        if (user_data_.contains("SUCESS")){
            if (login_on == false){
                QTimer* timer = new QTimer(this);
                QObject::connect(timer,SIGNAL(timeout()) ,this, SLOT(de_timeout_thread_code()));
                timer->start(300000);
            }
            login_on = true;
            qDebug()<<user_data_;
            user_data_ = user_data_.split("**mdsp_split_tag**").at(1);
            qDebug()<<user_data_;
            if (user_data_==""){
                ui->Page->setCurrentWidget(ui->set_user_data_page);
            }else{
                login_on = true;
                userdata = StringToJson(user_data_);
                change_to_homepage();
            }
        }
        else{
            bug_report_type_1("after_login_init",user_data_);
            QMessageBox messageBox2;
            messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
            exit(0);
        }
    }
}
void MainWindow::search_init(){
    ui->Page->setCurrentWidget(ui->loading_page);
    while (ui->search_datapack_result->children().size()!=0){
        while (ui->search_datapack_result->children().at(0)->children().size()!=0){
            delete ui->search_datapack_result->children().at(0)->children().at(0);
        }
        delete ui->search_datapack_result->children().at(0);
    }
    ui->status->setText("執行伺服器溝通...");
    QString recv = connect("search");
    qDebug()<<recv;
    if (!recv.contains("SUCESS")){
        qDebug()<<recv;
        bug_report_type_1("search",recv);
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);
    }
    ui->status->setText("");
    recv = recv.split("**mdsp_split_tag**").at(1);
    QJsonObject search_result = StringToJson(recv);
    QJsonArray datapack_result = search_result.value("datapacks").toArray();
    QJsonArray user_result = search_result.value("users").toArray();
    QStringList pixmap_list;
    QStringList pixmap_list2;
    ui->status->setText("下載圖片...");
    ui->search_datapack_result->setMinimumHeight(datapack_result.size()*91);
    ui->search_user_result->setMinimumHeight(user_result.size()*91);
    for (int i=1;i<=datapack_result.size();i++){
        pixmap_list.append(datapack_result.at(i-1).toObject().value("datapack_icon").toString());
    }
    for (int i=1;i<=user_result.size();i++){
        pixmap_list2.append(user_result.at(i-1).toObject().value("usericon").toString());
    }
    QList <QPixmap> images = batch_get_image(pixmap_list);
    QList <QPixmap> images2 = batch_get_image(pixmap_list2);
    ui->status->setText("準備頁面...");
    QWidget * recommend_datapack_list_parent_container;
    QPushButton * recommend_datapack_list_picture_show_area;
    QLabel * recommend_datapack_list_datapack_name;
    QSignalMapper * mapper = new QSignalMapper(this);
    for (int i=1;i<=datapack_result.size();i++){
        recommend_datapack_list_parent_container = new QWidget(ui->search_datapack_result);
        recommend_datapack_list_parent_container->move(0,91*(i-1));
        recommend_datapack_list_parent_container->resize(351,91);
        recommend_datapack_list_picture_show_area = new QPushButton(recommend_datapack_list_parent_container);
        recommend_datapack_list_picture_show_area->move(10,20);
        recommend_datapack_list_picture_show_area->resize(61,61);
        recommend_datapack_list_picture_show_area->setIcon(QIcon(images.at(i-1)));
        recommend_datapack_list_picture_show_area->setIconSize(QSize(61,61));
        recommend_datapack_list_datapack_name = new QLabel(recommend_datapack_list_parent_container);
        recommend_datapack_list_datapack_name->move(70,20);
        recommend_datapack_list_datapack_name->resize(261,51);
        recommend_datapack_list_datapack_name->setText(datapack_result.at(i-1).toObject().value("datapack_name").toString()+"\n開發者："+datapack_result.at(i-1).toObject().value("owner_nick").toString());
        recommend_datapack_list_datapack_name->setStyleSheet("color:rgb(255,255,255)");
        recommend_datapack_list_datapack_name->setAlignment(Qt::AlignLeft);
        recommend_datapack_list_parent_container->show();
        recommend_datapack_list_picture_show_area->show();
        recommend_datapack_list_datapack_name->show();
        if (datapack_result.at(i-1).toObject().value("removed").toInt()==1){
            recommend_datapack_list_datapack_name->setText("資料包已移除");
            recommend_datapack_list_picture_show_area->setEnabled(false);
        }
        QObject::connect(recommend_datapack_list_picture_show_area,SIGNAL(clicked()), mapper, SLOT(map()));
        mapper->setMapping(recommend_datapack_list_picture_show_area,datapack_result.at(i-1).toObject().value("datapack_id").toString());
    }
    QObject::connect(mapper,SIGNAL(mapped(QString)),this,SLOT(open_datapack_page(QString)));
    mapper = new QSignalMapper(this);
    for (int i=1;i<=user_result.size();i++){
        recommend_datapack_list_parent_container = new QWidget(ui->search_user_result);
        recommend_datapack_list_parent_container->move(0,91*(i-1));
        recommend_datapack_list_parent_container->resize(351,91);
        recommend_datapack_list_picture_show_area = new QPushButton(recommend_datapack_list_parent_container);
        recommend_datapack_list_picture_show_area->move(10,20);
        recommend_datapack_list_picture_show_area->resize(61,61);
        recommend_datapack_list_picture_show_area->setIcon(QIcon(images2.at(i-1)));
        recommend_datapack_list_picture_show_area->setIconSize(QSize(61,61));
        recommend_datapack_list_datapack_name = new QLabel(recommend_datapack_list_parent_container);
        recommend_datapack_list_datapack_name->move(70,20);
        recommend_datapack_list_datapack_name->resize(261,51);
        recommend_datapack_list_datapack_name->setText(user_result.at(i-1).toObject().value("name").toString());
        recommend_datapack_list_datapack_name->setStyleSheet("color:rgb(255,255,255)");
        recommend_datapack_list_datapack_name->setAlignment(Qt::AlignLeft);
        recommend_datapack_list_parent_container->show();
        recommend_datapack_list_picture_show_area->show();
        recommend_datapack_list_datapack_name->show();
        QObject::connect(recommend_datapack_list_picture_show_area,SIGNAL(clicked()), mapper, SLOT(map()));
        mapper->setMapping(recommend_datapack_list_picture_show_area,user_result.at(i-1).toObject().value("userid").toString());
    }
    QObject::connect(mapper,SIGNAL(mapped(QString)),this,SLOT(open_user_page(QString)));
    ui->status->setText("");
    ui->Page->setCurrentWidget(ui->search_result);
}

void MainWindow::app_init(){
    if (QFile(AppDir+"/mcpath.MDSP").exists()){
        QFile * MCPATH = new QFile(AppDir+"/mcpath.MDSP");
        MCPATH->open(QFile::ReadOnly);
        McDir = MCPATH->readAll();
        MCPATH->close();
    }else
    {
        if (QDir(McDir+"/.minecraft/saves").exists()){
            McDir += "/.minecraft/saves";
        }else if (os_type!="darwin")
        {
            //引導設定頁面
        }
    }
    while (ui->MainPageDatapacks->children().size()!=0){
        while (ui->MainPageDatapacks->children().at(0)->children().size()!=0){
            delete ui->MainPageDatapacks->children().at(0)->children().at(0);
        }
        delete ui->MainPageDatapacks->children().at(0);
    }
    ui->status->setText("執行伺服器溝通...");
    ui->Page->setCurrentWidget(ui->loading_page);
    ui->usericon->setPixmap(load_image_from_net(userdata.value("usericon").toString(),2).scaled(51,51));
    QString recv = connect("get_recommend");
    if (!recv.contains("SUCESS")){
        bug_report_type_1("getting_recommend_datapack",recv);
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);
    }
    ui->status->setText("");
    qDebug()<<recv;
    recv = recv.split("**mdsp_split_tag**").at(1);
    QJsonObject datapack_list = StringToJson(recv);
    QJsonArray datapacks = datapack_list["datapacks"].toArray();
    qDebug()<<datapacks;
    qDebug()<<datapacks.size();
    ui->MainPageDatapacks->setMinimumWidth(datapacks.size()*141);
    QWidget * recommend_datapack_list_parent_container;
    QPushButton * recommend_datapack_list_picture_show_area;
    QLabel * recommend_datapack_list_datapack_name;
    QSignalMapper * mapper = new QSignalMapper(this);
    QStringList pixmap_list;
    for (int i=1;i<=datapacks.size();i++){
        pixmap_list.append(datapacks.at(i-1).toObject().value("datapack_icon").toString());
    }
    QList <QPixmap> images = batch_get_image(pixmap_list);
    ui->Page->setCurrentWidget(ui->home_page);
    for (int i=1;i<=datapacks.size();i++){
        recommend_datapack_list_parent_container = new QWidget(ui->MainPageDatapacks);
        recommend_datapack_list_parent_container->move(141*(i-1),20);
        recommend_datapack_list_parent_container->resize(141,110);
        recommend_datapack_list_picture_show_area = new QPushButton(recommend_datapack_list_parent_container);
        recommend_datapack_list_picture_show_area->move(40,0);
        recommend_datapack_list_picture_show_area->resize(61,61);
        recommend_datapack_list_picture_show_area->setIcon(QIcon(images.at(i-1)));
        recommend_datapack_list_picture_show_area->setIconSize(QSize(61,61));
        recommend_datapack_list_datapack_name = new QLabel(recommend_datapack_list_parent_container);
        recommend_datapack_list_datapack_name->move(0,61);
        recommend_datapack_list_datapack_name->resize(141,50);
        recommend_datapack_list_datapack_name->setText(datapacks.at(i-1).toObject().value("datapack_name").toString()+"\n開發者："+datapacks.at(i-1).toObject().value("owner_nick").toString());
        recommend_datapack_list_datapack_name->setStyleSheet("color:rgb(255,255,255)");
        recommend_datapack_list_datapack_name->setAlignment(Qt::AlignCenter);
        recommend_datapack_list_parent_container->show();
        recommend_datapack_list_picture_show_area->show();
        recommend_datapack_list_datapack_name->show();
        if (datapacks.at(i-1).toObject().value("removed").toInt()==1){
            recommend_datapack_list_datapack_name->setText("資料包已移除");
            recommend_datapack_list_picture_show_area->setEnabled(false);
        }
        QObject::connect(recommend_datapack_list_picture_show_area,SIGNAL(clicked()), mapper, SLOT(map()));
        mapper->setMapping(recommend_datapack_list_picture_show_area,datapacks.at(i-1).toObject().value("datapack_id").toString());
    }
    QObject::connect(mapper,SIGNAL(mapped(QString)),this,SLOT(open_datapack_page(QString)));
    qDebug()<<ui->MainPageDatapacks->children();
    qDebug()<<ui->MainPageDatapacks->children().at(0)->children();
    qDebug()<<ui->MainPageDatapacks->children();
}
//http://drive.google.com/uc?id=1oYa_o-9BHagDnGT2hma7DZ1rcnM98hEf&export=download
void MainWindow::login_page_init(){
    if (QFile::exists(AppDir+"/userdata.MDSP")){
        QFile * user_login_data = new QFile(AppDir+"/userdata.MDSP");
        user_login_data->open(QFile::ReadOnly);
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
    if (ui->setpage_username->text()=="" || ui->setpage_username->text().length()>=15){
        QMessageBox messageBox;
        messageBox.warning(0,"錯誤","使用者暱稱必須介於1到15個字元之間");
        return;
    }
    ui->status->setText("執行自我介紹長度檢查");
    if (ui->setuserpage_description->toPlainText().length()>=15000){
        QMessageBox messageBox;
        messageBox.warning(0,"錯誤","自我介紹必須少於15000個字元");
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
    ui->Page->setCurrentWidget(ui->loading_page);
    ui->status->setText("執行伺服器溝通...");
    QString recv = connect("set_userdata");
    if (recv == "SUCESS"){
        ui->status->setText("");
        after_login_init();
        return;
    }else{
        bug_report_type_1("setting_userdata",recv);
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);
    }
}
void MainWindow::open_datapack_page(QString datapack_id){
    ui->install_datapack->setEnabled(true);
    ui->install_datapack->setText("安裝");
    ui->Page->setCurrentWidget(ui->loading_page);
    QString recv = MainWindow::connect("get_datapack",datapack_id);
    if (recv.contains("SUCESS")){
        qDebug()<<recv;
        datapack_view_page_data = StringToJson(recv.split("**mdsp_split_tag**").at(1));
        qDebug()<<datapack_view_page_data;
        ui->datapack_view_id->setText(datapack_id);
        ui->datapack_view_name->setText(datapack_view_page_data.value("datapack_name").toString());
        ui->developer_name->setText(datapack_view_page_data.value("owner_nick").toString());
        ui->viewdatapackpage_description->setText(datapack_view_page_data.value("description").toString());
        QString support_version1 = datapack_view_page_data.value("support_version_1").toString();
        QString support_version2 = datapack_view_page_data.value("support_version_2").toString();
        detect_maps();
        ui->map_list->clear();
        qDebug()<<world_list;
        for (int i=0;i<=world_list.size()-1;i++){
            qDebug()<<"GAN";
            qDebug()<<world_list.at(i).toObject().value(("Version"));
            if (version_detector(support_version2,support_version1,world_list.at(i).toObject().value("Version").toString())){
                qDebug()<<"dasdas";
                ui->map_list->addItem(world_list.at(i).toObject().value("Name").toString());
            }
        }
        datapack_edit_page_current = 1;
        QStringList images_request;
        images_request.append(datapack_view_page_data.value("datapack_icon").toString());
        for (int i=0;i<=datapack_view_page_data.value("image").toArray().size()-1;i++){
            images_request += datapack_view_page_data.value("image").toArray().at(i).toString();
        }
        qDebug()<<"adasd";
        view_page_background_list = batch_get_image(images_request);
        ui->user_icon_view_page->setIcon(view_page_background_list.at(0));
        ui->user_icon_view_page->setIconSize(QSize(61,61));
        view_page_background_list.removeAt(0);
        datapack_view_change_image();
        ui->datapackviewcolorboard->setStyleSheet("background-color:rgba("+QString::number(datapack_view_page_data.value("display_board_color").toArray().at(0).toInt())+","+QString::number(datapack_view_page_data.value("display_board_color").toArray().at(1).toInt())+","+QString::number(datapack_view_page_data.value("display_board_color").toArray().at(2).toInt())+","+QString::number(datapack_view_page_data.value("display_board_color").toArray().at(3).toInt())+") ; border-radius:10px;");
        ui->Page->setCurrentWidget(ui->datapack_view_page);

    }
    else{
        bug_report_type_1("getting_datapack_data",recv);
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);
    }

}
void MainWindow::open_user_page(QString userid){
    qDebug()<<userid;
    ui->Page->setCurrentWidget(ui->loading_page);
    ui->status->setText("執行伺服器溝通...");
    while (ui->user_view_page_datapacks->children().size()!=0){
        while (ui->user_view_page_datapacks->children().at(0)->children().size()!=0){
            delete ui->user_view_page_datapacks->children().at(0)->children().at(0);
        }
        delete ui->user_view_page_datapacks->children().at(0);
    }
    QString recv = MainWindow::connect("GET_USR_DATA",userid);
    if (!recv.contains("SUCESS")){
        bug_report_type_1("opening_user_page",recv);
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);
    }
    QJsonObject user_page_view_data = StringToJson(recv.split("**mdsp_split_tag**").at(1));
    qDebug()<<recv.split("**mdsp_split_tag**").at(1);
    qDebug()<<user_page_view_data;
    QJsonArray current_user_datapacks = StringToJson(recv.split("**mdsp_split_tag**").at(2)).value("datapacks").toArray();
    ui->view_user_data_board->setStyleSheet("background-color:rgba("+QString::number(user_page_view_data.value("display_board_color").toArray().at(0).toInt())+","+QString::number(user_page_view_data.value("display_board_color").toArray().at(1).toInt())+","+QString::number(user_page_view_data.value("display_board_color").toArray().at(2).toInt())+","+QString::number(user_page_view_data.value("display_board_color").toArray().at(3).toInt())+"); border-radius:15px;");
    ui->view_page_username->setText(user_page_view_data.value("name").toString());
    ui->viewuserpage_description->setText(user_page_view_data.value("description").toString());
    ui->user_icon_view->setPixmap(load_image_from_net(user_page_view_data.value("usericon").toString(),0).scaled(51,51));
    ui->background_view_user_page->setPixmap(load_image_from_net(user_page_view_data.value("background").toString(),0).scaled(731,381));
    QWidget * recommend_datapack_list_parent_container;
    QPushButton * recommend_datapack_list_picture_show_area;
    QLabel * recommend_datapack_list_datapack_name;
    QSignalMapper * mapper = new QSignalMapper(this);
    QStringList pixmap_list;
    for (int i=1;i<=current_user_datapacks.size();i++){
        pixmap_list.append(current_user_datapacks.at(i-1).toObject().value("datapack_icon").toString());
    }
    ui->status->setText("下載圖片...");
    QList <QPixmap> images = batch_get_image(pixmap_list);
    ui->status->setText("載入頁面...");
    ui->user_view_page_datapacks->setMinimumWidth(141*current_user_datapacks.size());
    for (int i=1;i<=current_user_datapacks.size();i++){
        recommend_datapack_list_parent_container = new QWidget(ui->user_view_page_datapacks);
        recommend_datapack_list_parent_container->move(141*(i-1),0);
        recommend_datapack_list_parent_container->resize(141,80);
        recommend_datapack_list_picture_show_area = new QPushButton(recommend_datapack_list_parent_container);
        recommend_datapack_list_picture_show_area->move(40,0);
        recommend_datapack_list_picture_show_area->resize(61,61);
        recommend_datapack_list_picture_show_area->setIcon(QIcon(images.at(i-1)));
        recommend_datapack_list_picture_show_area->setIconSize(QSize(61,61));
        recommend_datapack_list_datapack_name = new QLabel(recommend_datapack_list_parent_container);
        recommend_datapack_list_datapack_name->move(0,60);
        recommend_datapack_list_datapack_name->resize(141,20);
        recommend_datapack_list_datapack_name->setText(current_user_datapacks.at(i-1).toObject().value("datapack_name").toString());
        recommend_datapack_list_datapack_name->setStyleSheet("color:rgb(255,255,255)");
        recommend_datapack_list_datapack_name->setAlignment(Qt::AlignCenter);
        recommend_datapack_list_parent_container->show();
        recommend_datapack_list_picture_show_area->show();
        recommend_datapack_list_datapack_name->show();
        if (current_user_datapacks.at(i-1).toObject().value("removed").toInt()==1){
            recommend_datapack_list_datapack_name->setText("資料包已移除");
            recommend_datapack_list_picture_show_area->setEnabled(false);
        }
        QObject::connect(recommend_datapack_list_picture_show_area,SIGNAL(clicked()), mapper, SLOT(map()));
        mapper->setMapping(recommend_datapack_list_picture_show_area,current_user_datapacks.at(i-1).toObject().value("datapack_id").toString());
    }
    QObject::connect(mapper,SIGNAL(mapped(QString)),this,SLOT(open_datapack_page(QString)));
    ui->status->setText("");
    ui->Page->setCurrentWidget(ui->user_view_page);

}



void MainWindow::on_LR_Trigger_clicked()
{
    if (ui->Register->isChecked()){
        ui->LR_username->setEnabled(false);
        ui->LR_password->setEnabled(false);
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
        ui->LR_status->setText("");
        ui->LR_status->setText("執行伺服器溝通...");
        QString status = MainWindow::connect("Register");
        ui->LR_username->setEnabled(true);
        ui->LR_password->setEnabled(true);
        if (status=="SUCESS"){
            ui->LR_status->setText("註冊成功!");
            return;
        }
        if (status=="ERR.REGED"){
            ui->LR_status->setText("註冊失敗，已經有同帳號使用者存在!");
            return;
        }
        bug_report_type_1("register",status);
        QMessageBox messageBox2;
        messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
        exit(0);

    }
    if (ui->Login->isChecked()){
        ui->LR_username->setEnabled(false);
        ui->LR_password->setEnabled(false);
        username = ui->LR_username->text();
        ui->LR_status->setText("執行伺服器溝通...");
        for (int i=1;i<=ui->LR_username->text().length();i++){
            if (!QString("0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~").contains(ui->LR_username->text().at(i-1))){
                ui->LR_status->setText("帳號或密碼錯誤");
                return;
            }
        }
        QString status = MainWindow::connect("Login");
        ui->LR_username->setEnabled(true);
        ui->LR_password->setEnabled(true);
        if (status=="SUCESS"){
            trial_mode = false;
            qDebug()<<usertoken;
            QFile * user_login_data = new QFile(AppDir+"/userdata.MDSP");
            user_login_data->open(QFile::WriteOnly);
            user_login_data->write(QString(ui->LR_username->text()+","+ui->LR_password->text()).toUtf8());
            user_login_data->resize(user_login_data->pos());
            user_login_data->close();
            ui->LR_status->setText("執行伺服器溝通...");
            after_login_init();
            return;
        }
        if (status=="ERR.USRORPWD"){
            ui->LR_status->setText("帳號或密碼錯誤");
            return;
        }
        bug_report_type_1("login",status);
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
    ui->datapackselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->datapackdatasetR->value())+","+QString::number(ui->datapackdatasetG->value())+","+QString::number(ui->datapackdatasetB->value())+","+QString::number(ui->datapackdatasetA->value())+") ; border-radius:10px;");
}
void MainWindow::on_datapackdatasetG_valueChanged(int arg1)
{
    ui->datapackselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->datapackdatasetR->value())+","+QString::number(ui->datapackdatasetG->value())+","+QString::number(ui->datapackdatasetB->value())+","+QString::number(ui->datapackdatasetA->value())+") ; border-radius:10px;");
}

void MainWindow::on_datapackdatasetB_valueChanged(int arg1)
{
    ui->datapackselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->datapackdatasetR->value())+","+QString::number(ui->datapackdatasetG->value())+","+QString::number(ui->datapackdatasetB->value())+","+QString::number(ui->datapackdatasetA->value())+") ; border-radius:10px;");
}

void MainWindow::on_datapackdatasetA_valueChanged(int arg1)
{
    ui->datapackselfcolorboard->setStyleSheet("background-color:rgba("+QString::number(ui->datapackdatasetR->value())+","+QString::number(ui->datapackdatasetG->value())+","+QString::number(ui->datapackdatasetB->value())+","+QString::number(ui->datapackdatasetA->value())+") ; border-radius:10px;");
}

void MainWindow::datapack_set_change_image(){
    if (datapack_page_background.size()==0){
        ui->set_datapack_background->setPixmap(QPixmap(":/image/Deafault_Background.png"));
        ui->datapack_set_picture_index->setText("目前是預設圖片，沒有任何自定義圖片");
        datapack_edit_page_current = 0;
    }else{
        if (datapack_page_background.size()==1){
            datapack_edit_page_current=1;
        }
        ui->datapack_set_picture_index->setText("第 "+QString::number(datapack_edit_page_current)+" / "+QString::number(datapack_page_background.size())+" 張");
        ui->status->setText("正在嘗試從雲端硬碟取得圖片...");
        ui->pushButton_8->setEnabled(false);
        ui->pushButton_16->setEnabled(false);
        ui->edit_datapack_background->setEnabled(false);
        ui->set_datapack_background->setPixmap(load_image_from_net(datapack_page_background.at(datapack_edit_page_current-1).toString(),2).scaled(731,381));
        ui->pushButton_8->setEnabled(true);
        ui->pushButton_16->setEnabled(true);
        ui->edit_datapack_background->setEnabled(true);
        ui->status->setText("");
    }

}
void MainWindow::datapack_view_change_image(){
    ui->datapack_view_picture_index->setText("第 "+QString::number(datapack_view_page_current)+" / "+QString::number(view_page_background_list.size())+" 張");
    ui->datapack_view_page_background->setPixmap(view_page_background_list.at(datapack_view_page_current-1).scaled(781,381));
}
void MainWindow::on_pushButton_8_clicked()
{
    ui->edit_datapack_background->setEnabled(false);
    ui->remove_datapack_background->setEnabled(false);
    if (datapack_edit_page_current-1>0){
        datapack_edit_page_current-=1;
        datapack_set_change_image();
    }
    ui->edit_datapack_background->setEnabled(true);
    ui->remove_datapack_background->setEnabled(true);
}


void MainWindow::on_pushButton_16_clicked()
{
    ui->edit_datapack_background->setEnabled(false);
    ui->remove_datapack_background->setEnabled(false);
    if (datapack_edit_page_current+1<=datapack_page_background.size()){
        datapack_edit_page_current+=1;
        datapack_set_change_image();
    }
    ui->edit_datapack_background->setEnabled(true);
    ui->remove_datapack_background->setEnabled(true);
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
    if (datapack_view_page_current+1<=view_page_background_list.size()){
        datapack_view_page_current+=1;
    }
    datapack_view_change_image();
}

void MainWindow::on_edit_datapack_background_clicked()
{
    ui->edit_datapack_background->setEnabled(false);
    ui->remove_datapack_background->setEnabled(false);
    QDesktopServices::openUrl(QUrl("https://script.google.com/macros/s/AKfycbwBcOaz1nTvaWTohIIj7cacvgInYLdJSVPFZd5-LvvJhzelM3LSNFRC6NRjSIWgD3AZ/exec"));
    QString text = QInputDialog::getText(this," ","程式已經在你的預設瀏覽器打開上傳網站\n上傳後請輸入網站提供的檔案id\n並且在12小時內完成設定\n否則存在於伺服器的圖檔會被移除",QLineEdit::Normal);
    ui->status->setText("正在嘗試從雲端硬碟取得圖片...");
    if (text != ""){
        datapack_page_background.append(text);
    }
    datapack_set_change_image();
    ui->status->setText("");
    ui->edit_datapack_background->setEnabled(true);
    ui->remove_datapack_background->setEnabled(true);
}

void MainWindow::on_edit_datapack_icon_clicked()
{
    QDesktopServices::openUrl(QUrl("https://script.google.com/macros/s/AKfycbwBcOaz1nTvaWTohIIj7cacvgInYLdJSVPFZd5-LvvJhzelM3LSNFRC6NRjSIWgD3AZ/exec"));
    QString text = QInputDialog::getText(this," ","程式已經在你的預設瀏覽器打開上傳網站\n上傳後請輸入網站提供的檔案id\n並且在12小時內完成設定\n否則存在於上傳的圖檔會被移除",QLineEdit::Normal);
    ui->status->setText("正在嘗試從雲端硬碟取得圖片...");
    if (text != ""){
        ui->datapack_icon_setting->setPixmap(load_image_from_net(text,2).scaled(50,50));
        if (datapack_page_edit.contains("datapack_icon")){
            datapack_page_edit.remove("datapack_icon");
        }
        datapack_page_edit.insert("datapack_icon",text);
    }
    ui->status->setText("");
}

void MainWindow::on_remove_datapack_background_clicked()
{
    ui->edit_datapack_background->setEnabled(false);
    ui->remove_datapack_background->setEnabled(false);
    datapack_page_background.removeAt(datapack_edit_page_current-1);
    datapack_edit_page_current-=1;
    datapack_set_change_image();
    ui->edit_datapack_background->setEnabled(true);
    ui->remove_datapack_background->setEnabled(true);
}

void MainWindow::on_pushButton_21_clicked()
{
    pre_set_datapack.append(ui->datapack_id_setpage->text());
    ui->pre_set_datapacks_setpage->addItem(ui->datapack_id_setpage->text());
}

void MainWindow::on_pushButton_22_clicked()
{
    if (pre_set_datapack.size()>0){
        pre_set_datapack.removeAt(ui->pre_set_datapacks_setpage->currentIndex());
        ui->pre_set_datapacks_setpage->removeItem(ui->pre_set_datapacks_setpage->currentIndex());
    }
    qDebug()<<pre_set_datapack;
}

void MainWindow::on_pushButton_15_clicked()
{
    QDesktopServices::openUrl(QUrl("https://script.google.com/macros/s/AKfycbwBcOaz1nTvaWTohIIj7cacvgInYLdJSVPFZd5-LvvJhzelM3LSNFRC6NRjSIWgD3AZ/exec"));
    QString text = QInputDialog::getText(this," ","程式已經在你的預設瀏覽器打開上傳網站\n請上傳資料包後請輸入網站提供的檔案id",QLineEdit::Normal);
    if (text != ""){
        datapack_page_edit.insert(QString("datapack"),text);
        if (ui->datapack_name_set->text().length()>=5&&ui->datapack_name_set->text().length()<=15000&&ui->setdatapackpage_description->toPlainText().length()>=5&&ui->setdatapackpage_description->toPlainText().length()<=15000){
            datapack_page_edit.insert(QString("datapack_name"),ui->datapack_name_set->text());
            datapack_page_edit.insert(QString("description"),ui->setdatapackpage_description->toPlainText());
            if (datapack_page_background.size()==0){
                datapack_page_background.append("1vuqZlBs0UvQBpSauTmKg9zl-U6Jf430F");
            }
            datapack_page_edit.insert(QString("image"),datapack_page_background);
            qDebug()<<datapack_page_edit;
            qDebug()<<datapack_page_edit.value("datapack_icon");
            qDebug()<<datapack_page_edit.contains("datapack_icon");
            if (!datapack_page_edit.contains("datapack_icon")){
                datapack_page_edit.insert("datapack_icon","1kf30NcUsbWPkuJTxBNuqGlzW-IbcamPF");
            }
            QJsonArray display_board_color;
            display_board_color.append(ui->datapackdatasetR->value());
            display_board_color.append(ui->datapackdatasetG->value());
            display_board_color.append(ui->datapackdatasetB->value());
            display_board_color.append(ui->datapackdatasetA->value());
            datapack_page_edit.insert("display_board_color",display_board_color);
            datapack_page_edit.insert("preset_datapack",pre_set_datapack);
            datapack_page_edit.insert("support_version_1",ui->support_version1->currentText());
            datapack_page_edit.insert("support_version_2",ui->support_version2->currentText());
            ui->status->setText("執行伺服器溝通...");
            QString recv = connect("upload_datapack");
            if (recv.contains("SUCESS")){
                change_to_homepage();
            }
            else if (recv=="ERR.NOFILE"){
                QMessageBox messageBox;
                messageBox.warning(0,"錯誤","伺服器偵測不到你上傳的資料包或是圖片，請保證自己上傳的資料包符合資料包上架規定，否則將有可能帳號被停用數天");
                return;
            }
            else if (recv=="ERR.PRESETDPPACK"){
                QMessageBox messageBox;
                messageBox.warning(0,"錯誤","你引用的前置資料包不存在或是已經被移除");
                return;
            }
            else
            {
                bug_report_type_1("setting_datapack",recv);
                QMessageBox messageBox2;
                messageBox2.critical(0,"錯誤","由於未知的錯誤，程式無法繼續執行");
                exit(0);
            }
            ui->status->setText("");
            return;
        }
        else
        {
            QMessageBox messageBox;
            messageBox.warning(0,"錯誤","資料包名稱和資料包敘述都各需介於5~15000個字元之間");
            return;
        }
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    datapack_edit_page_current = 0;
    QJsonObject empty_json;
    datapack_page_edit = empty_json;
    QJsonArray empty_JA;
    datapack_page_background = empty_JA;
    ui->Page->setCurrentWidget(ui->datapack_setting_page);
    ui->user_icon_setting->setPixmap(QPixmap(":/image/user_icon.png").scaled(51,51));
    datapack_set_change_image();
}

void MainWindow::on_Page_currentChanged(int arg1)
{
    if (arg1==4){
        ui->loading->movie()->start();
    }
    else{
        ui->loading->movie()->stop();
    }
}

void MainWindow::on_back_to_home_page_clicked()
{
    change_to_homepage();
}

void MainWindow::on_goto_home_clicked()
{
    change_to_homepage();
}

void MainWindow::on_install_datapack_clicked()
{
    ui->map_list->setEnabled(false);
    ui->install_datapack->setEnabled(false);
    ui->back_to_home_page->setEnabled(false);
    ui->install_datapack->setText("安裝中");
    if (ui->map_list->currentText()!=""){
        QString recv = connect("get_all_preset_datapack");
        qDebug()<<recv;
        if (recv.contains("SUCESS")){
            QJsonArray preset_datapack_fileid_list_row = StringToJson(recv.split("**mdsp_split_tag**").at(1)).value("datapack_files").toArray();
            QJsonArray preset_datapack_id_list_row = StringToJson(recv.split("**mdsp_split_tag**").at(1)).value("datapack_id_list").toArray();
            QStringList preset_datapack_fileid_list;
            for (int i=0;i<=preset_datapack_fileid_list_row.size()-1;i++){
                preset_datapack_fileid_list.append(preset_datapack_fileid_list_row.at(i).toString());
            }
            QStringList preset_datapack_id_list;
            for (int i=0;i<=preset_datapack_id_list_row.size()-1;i++){
                preset_datapack_id_list.append(preset_datapack_id_list_row.at(i).toString());
            }
            QList <QByteArray> file_list = batch_get_files(preset_datapack_fileid_list);
            if (file_list.contains("error")){
                ui->install_datapack->setEnabled(true);
                ui->back_to_home_page->setEnabled(true);
                ui->install_datapack->setText("安裝");
                ui->map_list->setEnabled(true);
                qDebug()<<QString("Error1");
                return;
            }
            QStringList map_list = init_map_MSDP_read(ui->map_list->currentText());
            for (int i=0;i<=preset_datapack_fileid_list_row.size()-1;i++){
                QFile * datapack_file = new QFile(McDir+"/"+ui->map_list->currentText()+"/datapacks/"+preset_datapack_id_list.at(i)+".zip");
                datapack_file->open(QFile::WriteOnly);
                if (!datapack_file->isOpen()){
                    datapack_file->close();
                    ui->install_datapack->setEnabled(true);
                    ui->back_to_home_page->setEnabled(true);
                    ui->install_datapack->setText("安裝");
                    ui->map_list->setEnabled(true);
                    qDebug()<<QString("Error2");
                    return;
                }
                datapack_file->write(file_list.at(i));
                datapack_file->resize(datapack_file->pos());
                datapack_file->close();
                map_list.append(preset_datapack_id_list.at(i));
            }
            QFile* map_information = new QFile(McDir+"/"+ui->map_list->currentText()+"/Map_Information.MDSP");
            QJsonObject write_in_later_json;
            write_in_later_json.insert("datapacks",QJsonArray::fromStringList(map_list));
            QJsonDocument write_in_later_json_document(write_in_later_json);
            qDebug()<<write_in_later_json;
            map_information->open(QFile::ReadWrite);
            map_information->write(write_in_later_json_document.toJson(QJsonDocument::Compact));
            map_information->resize(map_information->pos());
            map_information->close();
            ui->install_datapack->setEnabled(false);
            ui->install_datapack->setText("已安裝");
            ui->back_to_home_page->setEnabled(true);
            ui->map_list->setEnabled(true);
        }
        else{
            ui->install_datapack->setEnabled(true);
            ui->back_to_home_page->setEnabled(true);
            ui->install_datapack->setText("安裝");
            ui->map_list->setEnabled(true);
            return;
        }
    }
    
}

void MainWindow::on_map_list_currentTextChanged(const QString &arg1)
{
    ui->install_datapack->setEnabled(false);
    ui->map_list->setEnabled(false);
    ui->install_datapack->setText("檢查中");
    QString selected_map = arg1;
    QStringList map_datapack_installed = init_map_MSDP_read(selected_map);
    ui->map_list->setEnabled(true);
    ui->map_list->setEnabled(true);
    if (map_datapack_installed.contains(ui->datapack_view_id->text())){
        ui->install_datapack->setEnabled(false);
        ui->install_datapack->setText("已安裝");
    }
    else if (map_datapack_installed.contains("ERR")){
        ui->install_datapack->setEnabled(false);
        ui->install_datapack->setText("錯誤");
    }
    else
    {
        ui->install_datapack->setEnabled(true);
        ui->install_datapack->setText("安裝");
    }
    //if ()
}

void MainWindow::on_pushButton_3_clicked()
{
    if (ui->serachbox->text() != ""){
        ui->status->setText("檢查關鍵字");
        QStringList keyword_check = ui->serachbox->text().split(" ");
        for (int i=0;i<=keyword_check.size()-1;i++){
            if (keyword_check.at(i)==""){
                return;
            }
        }
        search_init();
    }
}

void MainWindow::on_back_to_home_page_2_clicked()
{
    change_to_homepage();
}

void MainWindow::on_to_userpage_clicked()
{
    open_user_page(datapack_view_page_data.value("owner").toString());
}



void MainWindow::on_back_to_home_page_3_clicked()
{
    change_to_homepage();
}

void MainWindow::on_pushButton_clicked()
{
    ui->uisettings->setVisible(true);
}

void MainWindow::on_setting_button_clicked()
{
    ui->uisettings->setVisible(true);
}

void MainWindow::on_pushButton_10_clicked()
{
    open_user_page(username);
}

void MainWindow::on_goto_home_2_clicked()
{
    change_to_homepage();
}

void MainWindow::on_pushButton_9_clicked()
{
    ui->goto_home_2->setEnabled(true);
    ui->Page->setCurrentWidget(ui->set_user_data_page);
}
