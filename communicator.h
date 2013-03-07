#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QSemaphore>
#include <QObject>

//网络通信
extern bool isServer;

class Communicator: public QObject
{
    Q_OBJECT

public:
    Communicator();
    ~Communicator();

    //****************for client
    void initClient();
    void setServerIp(QString ip);
    void setServerPort(int port);
    void connectToServer();

    //**************for server
    void initServer();
    void getServerPort();
    QString getLocalIp();

    //*****client and server   shared  function
    bool sendMessage(QString &xmlMsg);
    QString getOpponentIp();


public slots:
    //for server
    void acceptConnection();

    //for client
    void displayError(QAbstractSocket::SocketError error );

    //client and server shared slots
    void readMessage(); //analyze message, change to xml format

signals:
    void connectionEstablished(); //  在startwidget中添加slot函数，使得连接建立后就转到游戏
    void gotXmlMessage(QString &xmlMsg);//将xml格式的消息通过signal发送到mainwindow类，由mianwindow类执行具体的操作

    //for server
    void serverInitMessageToDisplay(QString &msg);//本机做服务器时，发送到服务器选项中的label上显示的消息
    void clientConnMessageToDisplay(QString &msg);//本机做客户端时，发送给界面，并以弹窗形式显示的出错消息

private:
    QString ipAddress;
    int serverPort;
    QTcpServer * server;
    QTcpSocket * tcpSocket;
};

#endif // COMMUNICATOR_H
