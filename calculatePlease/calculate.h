#ifndef CALCULATE_H
#define CALCULATE_H
#include <QMainWindow>
#include <QMessageBox>
#include <QTcpSocket>
#include <QFileDialog>
#include <QSet>
#include <QThread>
#include "ui_calculate.h"
#ifdef WIN32
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

class NewScp : public QThread
{
    Q_OBJECT
public:
    NewScp();
    void set(int, QString);
protected:
    void run();
private:
    int sendOrRecv;
    QString fileName;
signals:
    void scpFinished(int);
};

class CalculatePlease : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    CalculatePlease(QWidget *parent = 0);
private slots:
    void on_askLineEdit_textChanged();
    void enableConnectButton(const QString &text);
    void establishConnection();
//    void onSokConnected();
 //   void onSokDisconnected();
//    void onSokReadyRead();
//    void onSokDisplayError(QAbstractSocket::SocketError socketError);
    void sendChatMsg();
    void sendMsg();
    void closeConnection();
    void addMsgToChat();
    void setVisibilityOfConnectButtons(bool ifConnected);
    void showMembers(bool ifConnected);
    void setVisibilityOfChat();
    void setInvisibilityOfChat();
    void about();
    void author();
    void config();
    void display();
    void sendDisplay();
    void description();
    void sendNewConfig();
    void sendReport();
    void sendReportMsg(int);

private:
    QTcpSocket *sok;
    quint16 blockSize;
    bool ifConnected;
    void createStatusBar();
    QLabel *statusLabel;
    QString currentStatus;
    QFont font;
    QSet<QString> memList;
    QString strAddress;
    QString strMessage;
    QString fileName;
    //NewScp *thread;
signals:
    void valueChanged(bool ifConnected);
    void drawMap();
    //void newConfig();
};

#endif // CALCULATE_H
