#ifndef CONFIG_H
#define CONFIG_H
#include <QMainWindow>
#include <QDialog>
#include <QtGui>
#include <QSettings>
#include "ui_confDialog.h"

class Configuration : public QDialog, public Ui::ConfDialog
{
    Q_OBJECT
public:
    Configuration (QWidget *parent = 0) {
        setupUi(this);
        settings = new QSettings("settings.conf", QSettings::NativeFormat);
        QString *str = new QString(settings->value("settings/login", "User").toString());
        QString *str_2 = new QString(settings->value("settings/address", "Address").toString());
//        QString *str_3 = new QString(settings->value("settings/constID", "-1").toString());
        confLineEdit->setText(*str);
        confLineEdit_2->setText(*str_2);
        connect(saveButton, SIGNAL(clicked()), this, SLOT(setNewConf()));
        //connect(confButtonBox, SIGNAL(rejected()), this, SLOT(close()));
        connect(this, SIGNAL(newConfig()), parent, SLOT(sendNewConfig()));
    }
private slots:
    void setNewConf() {
        if (QString::compare(settings->value("settings/login", "User").toString(), confLineEdit->text()) != 0) {
            settings->setValue("settings/oldNick", settings->value("settings/login", "User").toString());
            settings->setValue("settings/login", confLineEdit->text());
            settings->setValue("settings/address", confLineEdit_2->text());
            settings->sync();
            emit newConfig();
            close();
        }
    }

signals:
    void newConfig();

private:
    QSettings *settings;

};

#endif // CONFIG_H
