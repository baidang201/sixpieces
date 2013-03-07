#include "startwidget.h"
#include "ui_startwidget.h"
#include "communicator.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <QSemaphore>

extern bool canContinue;
extern bool isServer;
extern Communicator *communicator;
extern MainWindow *w;

StartWidget::StartWidget(QWidget * parent):QWidget(parent),ui(new Ui::StartWidget)
{

    ui->setupUi(this);
}

StartWidget::~StartWidget()
{
    delete ui;
}

void StartWidget::displayClientMessage(QString &msg)
{
    ui->clientLabel->setText(msg);
}

void StartWidget::displayServerMessage(QString &msg)
{
    ui->serverLabel->setText(msg);
}

void StartWidget::enterMainWindow()
{
    this->hide();
    w->initPieces();
    w->show();
}



void StartWidget::on_pushButton_startdanji_clicked()
{

}


void StartWidget::on_serverradioButton_clicked(bool checked)
{
    if(checked)
    {
        //qDebug()<<"server radiobutton click on"<<endl;
        isServer=true;
        ui->clientLabel->clear();
        ui->label_3->setEnabled(false);//iplabel
        ui->iplineEdit->setEnabled(false);
        ui->label_4->setEnabled(false);//portlabel
        ui->portLineEdit->setEnabled(false);
        ui->pushButton->setEnabled(false);
        w->setServer(true);
        //找到一个尚未使用的端口， 在ipLabel上显示出来
        communicator=new Communicator();
        connect((QObject*)communicator, SIGNAL(serverInitMessageToDisplay(QString&)), this, SLOT(displayServerMessage(QString&)));
        connect((QObject*)communicator, SIGNAL(connectionEstablished()), this, SLOT(enterMainWindow()));
        connect((QObject*)communicator, SIGNAL(connectionEstablished()), w, SLOT(replay()));
        connect((QObject*)communicator, SIGNAL(gotXmlMessage(QString&)), w, SLOT(handleOpponentMessage(QString&)));
        communicator->initServer();

    }
    else
    {
        communicator->disconnect();
        delete communicator;
        //qDebug()<<"server radiobutton click on cancel............";
    }

}

void StartWidget::on_clientradioButton_clicked(bool checked)
{
    if(checked)
    {
        //qDebug()<<"client radiobutton click on"<<endl;
        isServer=false;
        ui->serverLabel->clear();
        ui->label_3->setEnabled(true);//iplabel
        ui->iplineEdit->setEnabled(true);
        ui->label_4->setEnabled(true);//portlabel
        ui->portLineEdit->setEnabled(true);
        ui->pushButton->setEnabled(true);

        w->setServer(false);
        communicator=new Communicator();
        connect((QObject*)communicator, SIGNAL(clientConnMessageToDisplay(QString&)), this, SLOT(displayClientMessage(QString&)));
        communicator->initClient();
    }
    else
    {
        communicator->disconnect();
        delete communicator;
    }

}

void StartWidget::on_pushButton_clicked()
{
    communicator->setServerIp(ui->iplineEdit->text().trimmed());
    communicator->setServerPort(ui->portLineEdit->text().trimmed().toShort());

    //qDebug()<<communicator->get


    connect((QObject*)communicator, SIGNAL(connectionEstablished()), this, SLOT(enterMainWindow()));
    connect((QObject*)communicator, SIGNAL(connectionEstablished()), w, SLOT(replay()));
    connect((QObject*)communicator, SIGNAL(gotXmlMessage(QString&)), w, SLOT(handleOpponentMessage(QString&)));
    communicator->connectToServer();

}
