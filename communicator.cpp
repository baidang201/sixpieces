#include <QMessageBox>
#include <QList>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDebug>
#include <QTimer>
#include <QTextStream>


#include "communicator.h"
#include "startwidget.h"
#include "mainwindow.h"


extern StartWidget *widget;
extern MainWindow *w;

Communicator::Communicator()
{
    server=0;
    tcpSocket=0;
    ipAddress="";
    serverPort=0;
}

Communicator::~Communicator()
{
    if(server)
    {
        server->disconnect();
        delete server;
    }

    if(tcpSocket)
    {
        tcpSocket->disconnect();
        delete tcpSocket;
    }
}

void Communicator::initServer()
{
    if(!server)
        server=new QTcpServer(this);

    QString msg;
    bool flag=false;

    if(!server->isListening())
        flag=server->listen();

    if(!flag)
    {
        msg=QString("start listen failed");
        emit serverInitMessageToDisplay(msg);//emit,发送一个singnal
    }
    else
    {
        QList<QHostAddress> addressList=QNetworkInterface::allAddresses();
        int i;
        for(i=0; i<addressList.size(); i++)
        {
            if(addressList.at(i) != QHostAddress::LocalHost && addressList.at(i).toIPv4Address())
            {
                ipAddress=addressList.at(i).toString();
                break;
            }
        }

        if(ipAddress.isEmpty())
        {
            msg=QString("no local ip available");
            emit serverInitMessageToDisplay(msg);
        }
        else
        {
            serverPort=server->serverPort();
            QTextStream ts(&msg);
            //qDebug()<<tr("端口")<<serverPort<<endl;
            ts<<"ip: "<<ipAddress<<" port:"<<serverPort;
            emit serverInitMessageToDisplay(msg);
            connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()) );
        }
    }
}

void Communicator::initClient()
{
    if(!tcpSocket)
        tcpSocket=new QTcpSocket(this);
}

void Communicator::setServerIp(QString ip)
{
   ipAddress=ip;
}

void Communicator::setServerPort(int port)
{
    serverPort=port;
}

void Communicator::connectToServer()
{
    tcpSocket->abort();
    tcpSocket->connectToHost(ipAddress, serverPort);

    connect(tcpSocket, SIGNAL(connected()), this, SIGNAL( connectionEstablished()) );
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError))  );
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()) );
    connect(tcpSocket, SIGNAL(disconnected()), tcpSocket, SLOT(deleteLater()));
    connect(tcpSocket, SIGNAL(disconnected()), w, SLOT(opponentClosed()));
    qDebug()<<"connect server chenggong";

}

QString Communicator::getLocalIp()
{
    return ipAddress;
}

void Communicator::acceptConnection()
{
    tcpSocket = server->nextPendingConnection();
    connect(tcpSocket, SIGNAL(disconnected()), tcpSocket, SLOT(deleteLater()));
    connect(tcpSocket, SIGNAL(disconnected()), w, SLOT(opponentClosed()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    server->close();

    emit connectionEstablished();
}

void Communicator::displayError(QAbstractSocket::SocketError error)
{
    QString msg="";

    switch(error)
    {
    case QAbstractSocket::HostNotFoundError:
        msg="can't find host "+ipAddress;
        break;

    case QAbstractSocket::ConnectionRefusedError:
        msg="connection refused!";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        msg="remote host closed";
        break;

    default:
        msg=tcpSocket->errorString();
    }
    
    emit clientConnMessageToDisplay(msg);
    //qDebug()<<"error code"<<error;
}

void Communicator::readMessage()
{
    quint32 blocksize=0;
    QString message;
    QDataStream in(tcpSocket);

    in.setVersion(QDataStream::Qt_4_8);
    if(blocksize == 0)
    {
        if(tcpSocket->bytesAvailable() < (int)sizeof(quint32))
            return;
        in>>blocksize;

    }

    if(tcpSocket->bytesAvailable()< blocksize)
        return;
    in>>message;

    emit gotXmlMessage(message);
}

/*
 * 客户端和服务器之间通信的消息种类如下：
 * (1)走棋
 *    <Cmd>move</Cmd>  //表示该消息类型
 *    <fromPos><x>xPos</x><y>yPos</y></fromPos>  //此次走棋的原坐标
 *    <toPos><x>xPos</x><y>yPos</y></toPos>    //此次走棋的目的坐标
 *    <eatNum>n</eatNum> //被吃的棋子的数目
 *    <eatenPieces>    //被吃的棋子的坐标
 *        <eaten1>
 *            <x>x</x>
 *            <y>y</y>
 *        </eaten1>
 *        <eaten2>
 *        ...
 *        </eaten2>
 *    </eatenPieces>
 * (2)对方输棋。如果我方走棋后判断对方已输，则给对方发送这个消息
 *    <Cmd>youlose</Cmd>
 * (3)答复重新开始。赢棋一方将对方输棋的消息发送过去以后，隐含一个询问对方是否再来一局的消息，如果对方希望再来一局，则回发acceptReplay
 *    <Cmd>acceptReplay</Cmd>
 * (4)退出游戏。如果我方退出游戏，则需要给对方发送这个消息
 *    <Cmd>quit</Cmd>
**/

bool Communicator::sendMessage(QString &xmlMsg)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out<<(quint32)0;
    out<<xmlMsg;
    out.device()->seek(0);

    out<<(quint32)(block.size() - sizeof(quint32));

    if(tcpSocket->write(block) == -1)
        return false;
    else
        return true;
}

QString Communicator::getOpponentIp()
{
    if(tcpSocket->isValid())
        return tcpSocket->peerAddress().toString();
    else
        return "";
}


