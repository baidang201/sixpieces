#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QSemaphore>
#include <QObject>

//����ͨ��
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
    void connectionEstablished(); //  ��startwidget�����slot������ʹ�����ӽ������ת����Ϸ
    void gotXmlMessage(QString &xmlMsg);//��xml��ʽ����Ϣͨ��signal���͵�mainwindow�࣬��mianwindow��ִ�о���Ĳ���

    //for server
    void serverInitMessageToDisplay(QString &msg);//������������ʱ�����͵�������ѡ���е�label����ʾ����Ϣ
    void clientConnMessageToDisplay(QString &msg);//�������ͻ���ʱ�����͸����棬���Ե�����ʽ��ʾ�ĳ�����Ϣ

private:
    QString ipAddress;
    int serverPort;
    QTcpServer * server;
    QTcpSocket * tcpSocket;
};

#endif // COMMUNICATOR_H
