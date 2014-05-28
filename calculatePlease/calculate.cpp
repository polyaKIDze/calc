#include <QtGui>
#include "calculate.h"
#include "config.h"
#include <QFile>
#include <QString>
#include <QVector>
#include <QTextStream>
#include <stdio.h>
#include <iostream>

NewScp::NewScp()
{
}

void NewScp::set(int flag, QString name)
{
    sendOrRecv = flag; //sendFlag = 1; recvFlag = 0;
    fileName = name;
}

void NewScp::run()
{
    if (sendOrRecv) {
        FILE *in;
        QString str = "scp " + fileName + " inly@inly.tk:~";
        //printf("%s\n", str.toStdString().c_str());
        if(!(in = popen(str.toStdString().c_str(), "w")))
        {
            exit(1);
        }
        fputs("58545256\n", in);
        fclose(in);
    }
    else
    {
        int a = fileName.indexOf(".xls");
        QString str = "scp inly@inly.tk:~/s";
        str += fileName.at(a-1);
        str += ".csv";
        str += " .";
        FILE *in;
        if(!(in = popen(str.toStdString().c_str(), "w")))
        {
            exit(1);
        }
        fputs("58545256\n", in);
        fclose(in);
    }
}


CalculatePlease::CalculatePlease(QWidget *parent)
    :QMainWindow(parent)
{
    sok = new QTcpSocket(this);
    ifConnected = false;
    setupUi(this);
    QRegExp askExp(".*");
    askLineEdit->setValidator(new QRegExpValidator(askExp, this));
    QRegExp addressExp(".*");
    addressLineEdit->setValidator(new QRegExpValidator(addressExp, this));
    currentStatus = "<font color=grey>status: not connected to server<\font>";
    statusLabel = new QLabel(currentStatus);
    font = QFont("Helvetica", 10, QFont::Light);
    statusLabel->setFont(font);
    createStatusBar();
    connect(addressLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableConnectButton(const QString &)));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(establishConnection()));
    connect(disconnectButton, SIGNAL(clicked()), this, SLOT(closeConnection()));
    connect(reportButton, SIGNAL(clicked()), this, SLOT(sendReport()));
    connect(sendButton, SIGNAL(clicked()), this, SLOT(sendMsg()));
    connect(this, SIGNAL(valueChanged(bool)), this, SLOT(setVisibilityOfConnectButtons(bool)));
    connect(this, SIGNAL(valueChanged(bool)), this, SLOT(showMembers(bool)));
    connect(actionAbout,SIGNAL(triggered()),this,SLOT(about()));
    connect(actionAuthor,SIGNAL(triggered()),this,SLOT(author()));
    connect(actionEdit, SIGNAL(triggered()), this, SLOT(config()));
    connect(actionDisplay,SIGNAL(triggered()),this,SLOT(sendDisplay()));
    connect(this, SIGNAL(drawMap()), this, SLOT(display()));
    connect(actionDescription, SIGNAL(triggered()), this, SLOT(description()));
    answerText->setReadOnly(true);

    connect(sok, SIGNAL(readyRead()), this, SLOT(addMsgToChat()));
  //  connect(sok, SIGNAL(connected()), this, SLOT(onSokConnected()));
 //   connect(sok, SIGNAL(disconnected()), this, SLOT(onSokDisconnected()));
  //  connect(sok, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onSokDisplayError(QAbstractSocket::SocketError)));

    //connect(thread, SIGNAL(scpFinished(int)), this, SLOT(sendReportMsg(int)));

    //add chat
    chatBox->hide();
    hideChatButton->hide();
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    connect(openChatButton,SIGNAL(clicked()),this,SLOT(setVisibilityOfChat()));
    connect(hideChatButton,SIGNAL(clicked()),this,SLOT(setInvisibilityOfChat()));
    connect(sendChatButton, SIGNAL(clicked()), this, SLOT(sendChatMsg()));
}

void CalculatePlease::sendNewConfig()
{
    QSettings *settings = new QSettings("settings.conf", QSettings::NativeFormat);
    QString str = "newconf:" + settings->value("settings/login", "User").toString() + ":" + settings->value("settings/constID", "-1").toString() + ":" + settings->value("settings/oldNick", "User").toString();
    sok->write(str.toLocal8Bit(), strlen(str.toLocal8Bit()));
}

void CalculatePlease::sendReport()
{
    fileName = QFileDialog::getOpenFileName(this,
        tr("Open Report"), "", tr("Excel Files (*.xls *.xlsx)"));
    NewScp *thread = new NewScp();
    thread->set(1, fileName);
    thread->start();
    thread->wait();
    QSettings *settings = new QSettings("settings.conf", QSettings::NativeFormat);
    QString str = "report:" + settings->value("settings/login", "User").toString();
    sok->write(str.toLocal8Bit(), strlen(str.toLocal8Bit()));
}

void CalculatePlease::sendReportMsg(int flag)
{/*
    if (flag)
    {
        QSettings *settings = new QSettings("settings.conf", QSettings::NativeFormat);
        QString str = "report:" + settings->value("settings/login", "User").toString();
        sok->write(str.toLocal8Bit(), strlen(str.toLocal8Bit()));
    }
    else
    {
        QMessageBox* pmbx = new QMessageBox("New file from Server.",
                                            "Your report has been handled.",
                                            QMessageBox::Information,
                                            QMessageBox::Close,
                                            QMessageBox::NoButton,
                                            QMessageBox::NoButton);
        pmbx->exec();
        delete pmbx;
    }*/
}

void CalculatePlease::createStatusBar()
{
    statusBar()->addWidget(statusLabel);
}

void CalculatePlease::on_askLineEdit_textChanged()
{
    sendButton->setEnabled(askLineEdit->hasAcceptableInput());
}

void CalculatePlease::enableConnectButton(const QString &text)
{
    connectButton->setEnabled(!text.isEmpty());
}

void CalculatePlease::establishConnection()
{
    QString address = addressLineEdit->text();
    sok->connectToHost(address, 51000);
    ifConnected = true;
    emit valueChanged(ifConnected);
    QSettings *settings = new QSettings("settings.conf", QSettings::NativeFormat);
    QString login = settings->value("settings/login", "User").toString();
    QString data = "newbie:" + login;
    sok->write(data.toLocal8Bit(), strlen(data.toLocal8Bit()));
}

void CalculatePlease::closeConnection()
{
//    sok->disconnectFromHost();
    QSettings *settings = new QSettings("settings.conf", QSettings::NativeFormat);
    QString login = settings->value("settings/login", "User").toString();
    QString data = "exit:" + login;
    sok->write(data.toLocal8Bit(), strlen(data.toLocal8Bit()));
    addressLineEdit->clear();
    askLineEdit->clear();
    answerText->clear();
    chatBrowser->clear();
    ifConnected = false;
    emit valueChanged(ifConnected);
}

void CalculatePlease::sendMsg()
{
    QString msg = askLineEdit->text();
    char *message = msg.toLocal8Bit().data();
    sok->write(message);
}

void CalculatePlease::sendChatMsg()
{
    QSettings *settings = new QSettings("settings.conf", QSettings::NativeFormat);
    QString msg = "echo:" + settings->value("settings/login", "User").toString() + ":" + chatEdit->text();
    char *message = msg.toLocal8Bit().data();
    sok->write(message, strlen(message));
    chatEdit->clear();
}

void CalculatePlease::addMsgToChat(/*QString msg*/)
{
    char data[1024];
    memset(data, '\n', 1024);
    sok->read(data, sizeof(data));
    QString msg = (QString) data;
    QString ifEcho = msg.mid(0, 5);
    QString ifClose = msg.mid(0, 5);
    QString ifNewbie = msg.mid(0, 7);
    QString ifAddress = msg.mid(0, 7);
    QString ifIds = msg.mid(0, 3);
    QString ifRegister = msg.mid(0, 8);
    QString ifConstID = msg.mid(0, 8);
    QString ifRep = msg.mid(0, 7);
    if (QString::compare(ifEcho, "echo:") == 0)
    {
        chatBrowser->append(msg.mid(5, -1));
    }
    else if (QString::compare(ifNewbie, "newbie:") == 0)
    {
        memList.insert(msg.mid(7, -1));
        emit valueChanged(ifConnected);
    }
    else if (QString::compare(ifClose, "many:") == 0)
    {
        int from, current, next;
        for (from = 0; next != -1; )
        {
            current = msg.indexOf("newbie", from+1);
            next = msg.indexOf("newbie:", current+1);
            memList.insert(msg.mid(current+7, next-current-7));
            from = current;
        }
        emit valueChanged(ifConnected);
    }
    else if (QString::compare(ifClose, "exit:") == 0)
    {
        memList.remove(msg.mid(5, -1));
        emit valueChanged(ifConnected);
    }
    else if (QString::compare(ifAddress, "address") == 0)
    {
        strAddress = msg;
    }
    else if (QString::compare(ifIds, "ids") == 0)
    {
        strMessage = msg;
        if (ifConnected)
            emit drawMap();
    }
    else if (QString::compare(ifRegister, "register") == 0)
    {
        config();
    }
    else if (QString::compare(ifConstID, "constID:") == 0)
    {
        QSettings *settings = new QSettings("settings.conf", QSettings::NativeFormat);
        settings->setValue("settings/constID", msg.mid(8, -1));
    }
    else if (QString::compare(ifRep, "takeRep") == 0)
    {
        NewScp *thread = new NewScp();
        thread->set(0, fileName);
        thread->start();
        thread->wait();
        QMessageBox* pmbx =
                            new QMessageBox("New file from Server.",
                            "Your report has been handled.",
                                            QMessageBox::Information,
                                            QMessageBox::Close,
                                            QMessageBox::NoButton,
                                            QMessageBox::NoButton);
        pmbx->exec();
        delete pmbx;
    }
    else
    {
        answerText->clear();
        answerText->setText(msg);
    }
}

void CalculatePlease::setVisibilityOfConnectButtons(bool ifConnected)
{
    if (ifConnected)
    {
        currentStatus = "<font color=grey>status: connected to ";
        currentStatus += addressLineEdit->text();
        currentStatus += "<\font>";
        disconnectButton->setEnabled(true);
        connectButton->setEnabled(false);
        openChatButton->setEnabled(true);
        reportButton->setEnabled(true);
    }
    else
    {
        currentStatus = "<font color=grey>status: not connected to server<\font>";
        disconnectButton->setEnabled(false);
        connectButton->setEnabled(false);
        chatBox->setVisible(false);
        hideChatButton->setVisible(false);
        openChatButton->setVisible(true);
        openChatButton->setEnabled(false);
        reportButton->setEnabled(false);
    }
    statusLabel->setText(currentStatus);

}

void CalculatePlease::showMembers(bool ifConnected) {
    if (ifConnected)
    {
        memberBrowser->clear();
        QSet<QString>::iterator it = memList.begin();
        while (it != memList.end()) {
             memberBrowser->append(*it);
             ++it;
        }
    }
    else
    {
        memList.clear();
        memberBrowser->clear();
    }
}

void CalculatePlease::setVisibilityOfChat()
{
    chatBox->setVisible(true);
    hideChatButton->setVisible(true);
    openChatButton->setVisible(false);
}

void CalculatePlease::setInvisibilityOfChat()
{
    chatBox->setVisible(false);
    hideChatButton->setVisible(false);
    openChatButton->setVisible(true);
}

void CalculatePlease::about()
{
    QString str="This is a client program for server, which can calculate an arifmetic equation. ";
    str+="It may consist of numbers, \"+\", \"-\", \"*\", \"/\", \"(\" and \")\".";
    QMessageBox msg;
    msg.setWindowTitle("About");
    msg.setText(str);
    msg.exec();

}

void CalculatePlease::author() {
    QString str="S.Polyakov \npolyakov-sergey1993@mail.ru\nM.Kasheeva\nE.Vdovina";
    QMessageBox msg;
    msg.setWindowTitle("Author");
    msg.setText(str);
    msg.exec();
}

void CalculatePlease::config() {
    if (!ifConnected) {
        QMessageBox* pmbx =
                            new QMessageBox("Attention.",
                            "You are not connected to server.",
                                            QMessageBox::Information,
                                            QMessageBox::Close,
                                            QMessageBox::NoButton,
                                            QMessageBox::NoButton);
        pmbx->exec();
        delete pmbx;
    }
    else {
        Configuration *conf = new Configuration(this);
        conf->show();
    }
}

void CalculatePlease::description() {
    QString str="The map is opened by browser. Internet connection is necessary.\nThe stores are shown by two types of markers.\nA pink marker means that the store has sent a message.\nA red marker means that the store hasn't sent a message.";
    QMessageBox msg;
    msg.setWindowTitle("Description");
    msg.setText(str);
    msg.exec();
}

void CalculatePlease::sendDisplay() {
    if (!ifConnected) {
        QMessageBox* pmbx =
                            new QMessageBox("Attention.",
                            "You are not connected to server.",
                                            QMessageBox::Information,
                                            QMessageBox::Close,
                                            QMessageBox::NoButton,
                                            QMessageBox::NoButton);
        pmbx->exec();
        delete pmbx;
    }
    else {
    QString msg = "address";
    char *message = msg.toLocal8Bit().data();
    sok->write(message);
    }
}

void CalculatePlease::display() {
    //QString strAddress("address:\"ulitsa Borisevicha, 4B, Krasnoyarsk, Krasnoyarskiy kray, Russia\"Internatsionalnaya ulitsa, 100A, Nizhniy Novgorod, Nizhegorodskaya oblast', Russia\"ulitsa Margolina, 22, Yaroslavl, Yaroslavskaya oblast', Russia\"Domostroitelnaya ulitsa, 4, Sankt-Peterburg, Russia\"bulvar Engelsa, 14a, Volgograd, Volgogradskaya oblast', Russia");
    //QString strMessage("ids:00001");
    int i;
    int Num;
    int n = strMessage.size()-4;
    QVector<int> message(n);
    bool ok = 0;
    QString strMessageModif = strMessage.mid(4,n);
    Num = strMessageModif.toInt(&ok,10);
    if (ok){
        for (i = 0; i<n; i++){
            message[n-i-1] = Num % 10;
            Num = Num - message[n-i-1];
            Num = Num/10;
            //printf("message[n-i-1]=%d, Num=%d\n",message[n-i-1],Num);
        }
    }

    QStringList addresses = strAddress.split(QRegExp("\""));
    /*for (int i = 0; i < addresses.size(); i++)
            std::cout << addresses.at(i).toLocal8Bit().constData() << std::endl;*/

    QFile fileMap("Map.html");
    QFile input("beginning.txt");
    if (input.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&input);
        if (fileMap.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&fileMap);
            while (!in.atEnd()) {
                QString line = in.readLine();
                out << line << "\n";
            }
            input.flush();
            input.close();
            out << "   var address=[";
            for (i = 1; i <= n; i++){
                if (i == n){
                    out << "\"" << addresses.at(i).toLocal8Bit().constData() << "\"";
                } else {
                    out << "\"" << addresses.at(i).toLocal8Bit().constData() << "\",";
                }
            }
            out << "];\n   var message=[";
            for (i = 0; i<n; i++){
                if (i == n-1){
                    out << message[i];
                } else {
                    out << message[i] << ",";
                }
            }
            out << "];\n   var n=" << n << ";\n";
            QFile input("ending.txt");
            if (input.open(QIODevice::ReadOnly | QIODevice::Text)){
                QTextStream in(&input);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    out << line << "\n";
                }
                input.flush();
                input.close();
            }
            fileMap.flush();
            fileMap.close();
        }

    }
#ifdef WIN32
    /*
    UINT WINAPI WinExec(
      _In_  LPCSTR lpCmdLine,
      _In_  UINT uCmdShow
    );
    */
    WinExec(
      "Map.html", //полное имя файла с нулем в конце?
      SW_MAXIMIZE
    );
    /*
    CreateProcess(
      LPCTSTR lpApplicationName ,                 // имя исполняемого модуля
      LPTSTR lpCommandLine,                       // командная строка
      LPSECURITY_ATTRIBUTES lpProcessAttributes , // SD (дескриптор безопасности)
      LPSECURITY_ATTRIBUTES lpThreadAttributes,   // SD
      BOOL bInheritHandles,                       // дескриптор параметра наследования
      DWORD dwCreationFlags,                      // флажки создания
      LPVOID lpEnvironment,                       // новый блок конфигурации
      LPCTSTR lpCurrentDirectory,                 // имя текущего каталога
      LPSTARTUPINFO lpStartupInfo,                // информация предустановки
      LPPROCESS_INFORMATION lpProcessInformation  // информация о процессе
    );
    */
    /*
    CreateProcess(
      NULL,
      "Map.html",                       // полное имя файла Map.html
      NULL,
      NULL,
      TRUE,
      NULL,
      NULL,
      NULL,
      LPSTARTUPINFO lpStartupInfo,                // информация предустановки
      LPPROCESS_INFORMATION lpProcessInformation  // информация о процессе
    );
    */
#else
    int f_st = fork();
    if (f_st == 0){
        execlp("xdg-open","xdg-open",
               "Map.html",(char *)NULL );
    }
#endif
}
