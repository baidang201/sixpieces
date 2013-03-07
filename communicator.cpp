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
        emit serverInitMessageToDisplay(msg);//emit,����һ��singnal
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
            //qDebug()<<tr("�˿�")<<serverPort<<endl;
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
 * �ͻ��˺ͷ�����֮��ͨ�ŵ���Ϣ�������£�
 * (1)����
 *    <Cmd>move</Cmd>  //��ʾ����Ϣ����
 *    <fromPos><x>xPos</x><y>yPos</y></fromPos>  //�˴������ԭ����
 *    <toPos><x>xPos</x><y>yPos</y></toPos>    //�˴������Ŀ������
 *    <eatNum>n</eatNum> //���Ե����ӵ���Ŀ
 *    <eatenPieces>    //���Ե����ӵ�����
 *        <eaten1>
 *            <x>x</x>
 *            <y>y</y>
 *        </eaten1>
 *        <eaten2>
 *        ...
 *        </eaten2>
 *    </eatenPieces>
 * (2)�Է����塣����ҷ�������ж϶Է����䣬����Է����������Ϣ
 *    <Cmd>youlose</Cmd>
 * (3)�����¿�ʼ��Ӯ��һ�����Է��������Ϣ���͹�ȥ�Ժ�����һ��ѯ�ʶԷ��Ƿ�����һ�ֵ���Ϣ������Է�ϣ������һ�֣���ط�acceptReplay
 *    <Cmd>acceptReplay</Cmd>
 * (4)�˳���Ϸ������ҷ��˳���Ϸ������Ҫ���Է����������Ϣ
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


