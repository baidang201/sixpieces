
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QSemaphore>
#include <QChar>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "communicator.h"
#include "xmlHandler.h"

extern bool canContinue;
extern Communicator *communicator;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMinimumSize(windowWidth, windowHeight);

    //ʹ����Դ·������ԭʼ�ļ�ϵͳ·��
    if(!blackPiece.load(":/images/black.png"))//if(!blackPiece.load("D:\\qtwork\\sixchess\\graduationproject\\sixchessnewtesting\\images\\black.png"))
        qDebug()<<tr("���غ���ͼƬʧ��");

    if(!blackhi.load(":/images/blackhi.png"))
        qDebug()<<tr("���غ������ͼƬʧ��");

    if(!whitePiece.load(":/images/white.png"))
        qDebug()<<tr("���ذ���ͼƬʧ��");

    if(!whitehi.load(":/images/whitehi.png"))
        qDebug()<<tr("���ذ������ͼƬʧ��");

    mySound=new QSound("./sounds/my.wav", this);
    oppoSound=new QSound("./sounds/oppo.wav");
    myValue=-1;
    myPiece=blackPiece;
    myhi=blackhi;
    counterPiece=whitePiece;

    curSelected=false;
    preViewPos.setX(-1);
    preViewPos.setY(-1);
    initLabels();
    initPieces();

    myLabel=new QLabel(this);
    oppoLabel =new QLabel(this);
    oppoIpLabel=new QLabel(this);
    oppoClockLabel=new QLabel(this);
	myClockLabel = new QLabel(this);
    timer=new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(updateClock()));
    qDebug()<<tr("mainwindow ��ʼ�����");

}

void MainWindow::setServer(bool iamServer)
{
    if(iamServer)
    {
        myValue=-1;
        myPiece=blackPiece;
        myhi=blackhi;
        counterPiece=whitePiece;
        counterhi=whitehi;
    }
    else
    {
        myValue=1;
        myPiece=whitePiece;
        myhi=whitehi;
        counterPiece=blackPiece;
        counterhi=blackhi;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mySound;
    delete oppoSound;
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            if(label[i][j])
            {
                delete label[i][j];
                label[i][j]=0;
            }
        }
    }

    if(oppoLabel)
    {
        delete oppoLabel;
        oppoLabel=0;
    }

    if(oppoIpLabel)
    {
        delete oppoIpLabel;
        oppoIpLabel=0;
    }

    if(myLabel)
    {
        delete myLabel;
        myLabel=0;
    }

    if(oppoClockLabel)
    {
        delete oppoClockLabel;
        oppoClockLabel=0;
    }

    if(myClockLabel)
    {
        delete myClockLabel;
        myClockLabel=0;
    }

    if(targetClockLabel)
    {
        delete targetClockLabel;
        targetClockLabel=0;
    }

    if(timer)
    {
        delete timer;
        timer=0;
    }

}

void MainWindow::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);

    //ʹ����Դ·������ԭʼ�ļ�ϵͳ·��
    bool flag=boardPic.load(":/images/board.png");
    if(!flag)
        qDebug()<<tr("��������ͼƬʧ�ܣ�");
    else
        qDebug()<<tr("��������ͼƬ�ɹ���");

    painter.drawPixmap(0, 0, boardWidth, boardWidth, boardPic);
    QPen pen;
    pen.setColor(QColor(0,0,0));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawRect(boardWidth+1, 1, windowWidth-boardWidth-1, windowHeight-1);

}

void MainWindow::replay()
{
    myBoard.initBoard();
    initPieces();

    if(isServer)//����������
    {
        canContinue=true;
    }
    else
    {
        canContinue=false;
    }

    initClock();
    resetClock();// ����������canContinue��ֵ����������ʱ��
}

void MainWindow::requestForReplay()
{
    string xmlMsg;
    xmlMsg=createXMLNode("Cmd", "youlose");
    QString qMsg=QString(xmlMsg.c_str());
    communicator->sendMessage(qMsg);
    
}

void MainWindow::acceptReplay()
{
    string xmlMsg;
    xmlMsg=createXMLNode("Cmd","acceptReplay");
    QString qMsg=QString(xmlMsg.c_str());
    communicator->sendMessage(qMsg);
    replay();
}

void MainWindow::opponentClosed()
{
    QMessageBox::information(this, tr("��Ϣ"), tr("�Է����˳�"));
    close();
}

void MainWindow::handleOpponentMessage(QString &msg)
{
    string xmlMsg, cmd;
    xmlMsg=msg.toStdString();
    cmd=getXMLNode(xmlMsg, "Cmd");

    if(cmd == "move")
    {
        string fromValue, toValue;
        fromValue=getXMLNode(xmlMsg, "fromPos");
        toValue=getXMLNode(xmlMsg, "toPos");
        stringstream ssfromx, ssfromy;
        int fromx, fromy;
        ssfromx<<getXMLNode(fromValue, "x");
        ssfromx>>fromx;
        ssfromy<<getXMLNode(fromValue, "y");
        ssfromy>>fromy;

        stringstream sstox, sstoy;
        int tox, toy;
        sstox<<getXMLNode(toValue, "x");
        sstox>>tox;
        sstoy<<getXMLNode(toValue, "y");
        sstoy>>toy;

        myBoard.setValue(tox, toy, myBoard.getValue(fromx, fromy));//�ƶ��߼�����
        myBoard.setValue(fromx, fromy, 0);

        //�ƶ�ͼ�����ӣ� ע��˴��ƶ����ǶԷ������ӣ����У�����Է��Ƿ����������軻����
        if(isServer)
        {
            label[fromx][fromy]->clear();
            label[tox][toy]->setPixmap(counterPiece);
        }
        else
        {
            label[3 - fromx][3 - fromy]->clear();
            label[3 - tox][3 - toy]->setPixmap(counterPiece);
        }

        oppoSound->play();
        string eatNumStr;
        int eatNum;
        eatNumStr=getXMLNode(xmlMsg, "eatNum");
        stringstream ssn;
        ssn<<eatNumStr;
        ssn>>eatNum;

        string prefix="eaten";

        for(int i=0; i<eatNum; i++)
        {
            stringstream sseaten;
            string pieceName, pieceValue;
            sseaten<<prefix<<i+1;
            sseaten>>pieceName;
            pieceValue=getXMLNode(xmlMsg, pieceName);

            stringstream ssPiecex, ssPiecey;
            int piecex, piecey;
            ssPiecex<<getXMLNode(pieceValue, "x");
            ssPiecex>>piecex;
            ssPiecey<<getXMLNode(pieceValue, "y");
            ssPiecey>>piecey;
            myBoard.setValue(piecex, piecey, 0);//�Ե��߼�����

            //�Ե�ͼ������
            if(isServer)
                label[piecex][piecey]->clear();
            else
                label[3-piecex][3-piecey]->clear();

        }
        canContinue=true;
        resetClock();
    }
    else if(cmd == "youlose")
    {
        stopClock();
        QMessageBox mb(this);       
        mb.setIcon(QMessageBox::Question);
        mb.setWindowTitle(tr("������"));
        mb.setText(tr("�����ˣ� �Ƿ�����һ��?"));
        QPushButton btnOk(&mb),btnQuit(&mb);
        btnOk.setText(tr("����һ��"));
        btnQuit.setText(tr("�˳�"));
        mb.addButton(&btnOk, QMessageBox::YesRole);
        mb.addButton(&btnQuit, QMessageBox::NoRole);
        connect(&btnOk, SIGNAL(clicked()),this, SLOT(acceptReplay()));
        connect(&btnQuit, SIGNAL(clicked()), this, SLOT(close()));
        mb.exec();
    }
    else if(cmd == "acceptReplay")
    {
        replay();
    }
    else if(cmd == "quit")
    {
        qDebug()<<"opponent left";
        QMessageBox::information(this, tr("��Ϣ"), tr( "�Է����˳�"));
        close();

    }
    else
    {
        qDebug()<<"unknown message command";
    }
}

void MainWindow::initLabels()
{
    int i,j;
    int width=myPiece.width();
    qDebug("initLabels()");

    for(i=0; i<4; i++)
    {
        for(j=0; j<4; j++)
        {
            label[i][j]=new QLabel(this);
            label[i][j]->move(70+171*j-width*1.0/2, 70+171*i-width*1.0/2);
            label[i][j]->resize(width,width);
            label[i][j]->show();
        }
    }
}

void MainWindow::initPieces()
{
    int i,j;
    qDebug("initPieces()");
    if(isServer)
    {
        for(i=0; i<4; i++)
        {
            for(j=0; j<4; j++)
            {
                if(myBoard.getValue(i,j) == myValue)
                {
                    label[i][j]->setPixmap(myPiece);
                    label[i][j]->show();
                }
                else if(myBoard.getValue(i, j)==-myValue)
                {
                    label[i][j]->setPixmap(counterPiece);
                    label[i][j]->show();
                }
                else
                {
                    label[i][j]->clear();
                    label[i][j]->show();
                }
            }
        }
    }
    else
    {
        for(i=0; i<4; i++)
        {
            for(j=0; j<4; j++)
            {
                if(myBoard.getValue(i, j) == myValue)
                {
                    label[3-i][3-j]->setPixmap(myPiece);
                    label[3-i][3-j]->show();
                }
                else if(myBoard.getValue(i, j) == -myValue)
                {
                    label[3-i][3-j]->setPixmap(counterPiece);
                    label[3-i][3-j]->show();
                }
                else
                {
                    label[3-i][3-j]->clear();
                    label[3-i][3-j]->show();
                }
            }
        }
    }

    qDebug()<<"initpiece ok";
}


void MainWindow::initClock()
{
    QString oppoText=QString(tr("�Է�"));
    oppoLabel->setText(oppoText);
    oppoLabel->setFont(QFont(tr("����"),15));
    oppoLabel->move(boardWidth+40, 100);
    oppoLabel->show();

    QString oppoIp=QString(tr("IP:"))+communicator->getOpponentIp();
    oppoIpLabel->setText(oppoIp);
    oppoIpLabel->move(boardWidth+15, 120);
    oppoLabel->show();

    QString myText=QString(tr("�ҷ�"));
    myLabel->setText(myText);
    myLabel->setFont(QFont(tr("����"),15));
    myLabel->move(boardWidth+40, windowHeight-200);
    myLabel->show();

    oppoClockLabel->move(boardWidth+20, 150);
    myClockLabel->move(boardWidth+20, windowHeight-170);
    targetClockLabel=0;

}

void MainWindow::stopClock()
{
    timer->stop();
    targetClockLabel->clear();
}

void MainWindow::updateClock()
{
    sec+=1;
    if(sec == 60)
    {
        sec=0;
        min+=1;
        if(min == 60)
        {
            min=0;
            hour+=1;
        }
    }
    QString time;
    time=tr("%1:%2:%3").arg(hour,2,10,QChar('0')).arg(min, 2, 10, QChar('0')).arg(sec, 2, 10, QChar('0'));
    targetClockLabel->setText(time);
    targetClockLabel->setFont(QFont(tr("����"), 16));
    targetClockLabel->show();

}

void MainWindow::resetClock()
{
    hour=0;
    min=0;
    sec=0;

    if(targetClockLabel)
        targetClockLabel->clear();
    if(canContinue)
        targetClockLabel=myClockLabel;
    else
        targetClockLabel=oppoClockLabel;

    timer->start(1000);
}

void MainWindow::mousePressEvent(QMouseEvent *me)
{
    QPoint logicPos, viewPos;
    calcPosition(me->pos(), logicPos, viewPos);
    if(canContinue)
        processClick(logicPos, viewPos);
}


//ͨ����굱ǰ��λ��pos�����������λ������һ�����ӡ�����������ĳһ�����ӣ��ӷ���true�� ���������ǿհ״��� ����false
void MainWindow::calcPosition(const QPoint &pos, QPoint &logicPos, QPoint &viewPos)
{
    int i,j;
    for(i=0; i<4; i++)
    {
        for(j=0; j<4; j++)
        {
            int left=70+j*171-blackPiece.width()*1.0/2;
            int right=left+blackPiece.width();
            int top=70+i*171-blackPiece.width()*1.0/2;
            int bottom=top+blackPiece.width();

            if(pos.x() >= left && pos.x()<=right  && pos.y()>=top && pos.y()<=bottom)
            {
                viewPos.setX(i);
                viewPos.setY(j);

                if(isServer)//��Ϊ������һ������������ʾ���߼������ϵ�����λ��һ��
                {
                    logicPos.setX(i);
                    logicPos.setY(j);
                }
                else//��Ϊ������һ�������̵���ʾ���߼�������ת180�Ժ�Ľ��
                {
                    logicPos.setX(3-i);
                    logicPos.setY(3-j);
                }
                return;
            }

        }
    }

    logicPos.setX(-1);
    logicPos.setY(-1);

    viewPos.setX(-1);
    viewPos.setY(-1);

}


/*
��1��֮ǰδѡ���κ�����
-����ǰ���λ�ò��ǺϷ���������λ�ã�nop
-����ǰλ�����ҷ�����λ�ã���ѡ�����ӣ���������ʾ
-����ǰλ�õ����Ӳ����ҷ����ӣ��հ����� �� �Է����ӣ���nop

��2��֮ǰѡ��һ���ҷ�����
-����ǰ��������ҷ����ӣ��Ҳ�ͬ��֮ǰ�����ӣ���ȡ��ԭ�������ӣ�����ѡ�е�ǰ���ӣ���������ǰ����
                     ����֮ǰ��������ͬ����ȡ��ѡ�е�ǰ����
-����ǰ��λ�ò��ǺϷ�����������λ�û��ǶԷ�����������λ�ã���ȡ��ѡ��ԭ��������
-����ǰ��λ���ǿհ�����λ�ã�logicBoard[cruPos.x()][curPox.y()] == 0��,�����壬��ȡ��ԭ��������
*/
void MainWindow::processClick(const QPoint &logicPos, const QPoint &viewPos)
{
    if(!curSelected)
    {
        if(logicPos.x() == -1   ||  logicPos.y() == -1)
            return ;
        else if(myBoard.getValue(logicPos.x(), logicPos.y()) == myValue)
        {
            curSelected=true;
            myBoard.setCurPos(logicPos.x(), logicPos.y());
            preViewPos.setX(viewPos.x());
            preViewPos.setY(viewPos.y());
            highlightCurPiece(viewPos.x(), viewPos.y());
        }
    }
    else
    {
        if(logicPos.x() == -1 || logicPos.y() == -1
                || myBoard.getValue(logicPos.x(), logicPos.y()) == -myValue)
        {
            curSelected=false;
            deHighlightCurPiece(preViewPos.x(), preViewPos.y());
            preViewPos.setX(-1);
            preViewPos.setY(-1);
        }
        else if(myBoard.getValue(logicPos.x(), logicPos.y()) == myValue)
        {
            if(preViewPos.x() != viewPos.x() || preViewPos.y() != viewPos.y())
            {
                deHighlightCurPiece(preViewPos.x(), preViewPos.y());
                myBoard.setCurPos(logicPos.x(), logicPos.y());
                highlightCurPiece(viewPos.x(), viewPos.y());
                preViewPos.setX(viewPos.x());
                preViewPos.setY(viewPos.y());
            }
            else
            {
                curSelected = false;
                deHighlightCurPiece(preViewPos.x(), preViewPos.y());
                preViewPos.setX(-1);
                preViewPos.setY(-1);
            }
        }
        else
        {
            QPoint *eaten=0;
            int num;
            bool flag=myBoard.move(logicPos, eaten, num);
            if(flag)
            {
                //todo:�������ɹ��Ĵ��룬��Ҫ��ɵ�����
                //������ʾ�Լ������̣�����Ե��Է������ӣ��ñ��Ե���������ʧ��done
                //������Ȩ�����Է����ڴ��ڼ�Ҫ���μ������κβ���
                //�����������������ߵ����ӣ��Ե������ӣ��������֣�done
                //�ж϶Է��Ƿ��Ѿ����ˣ�����ǣ����ڱ�����������ʾ��ʾ�����ѽ�������Է�
                //
                canContinue=false;
                resetClock();
                mySound->play();
                label[preViewPos.x()][preViewPos.y()]->clear();
                label[preViewPos.x()][preViewPos.y()]->show();
                label[viewPos.x()][viewPos.y()]->setPixmap(myPiece);
                label[viewPos.x()][viewPos.y()]->show();
				eatPieces(eaten, num);
                //�������������������Է�
                QPoint originPos;
                myBoard.getCurPos(originPos);
                string xmlMsg="";
                xmlMsg+=createXMLNode("Cmd", "move");
                string fromPos, toPos;
                fromPos =createXMLNode("x", originPos.x())+createXMLNode("y", originPos.y());
                xmlMsg+=createXMLNode("fromPos", fromPos);

                toPos=createXMLNode("x", logicPos.x())+createXMLNode("y", logicPos.y());
                xmlMsg+=createXMLNode("toPos", toPos);

                xmlMsg+=createXMLNode("eatNum", num);
                string nameprefix="eaten";
                string eatenPieces="";
                for(int i=0; i<num; i++)
                {
                    stringstream ss;
                    string pieceNo, piecePos;
                    ss<<nameprefix<<i+1;
                    ss>>pieceNo;
                    piecePos=createXMLNode("x", eaten[i].x())+createXMLNode("y", eaten[i].y());
                    eatenPieces+=createXMLNode(pieceNo, piecePos);
                }
                if(num)
                {
                    xmlMsg+=createXMLNode("eatenPieces", eatenPieces);
                }

                qDebug()<<"xml message"<<QString(xmlMsg.c_str());
                QString qMsg=QString(xmlMsg.c_str());
                communicator->sendMessage(qMsg);
                if(myBoard.isOpponentLose())
                {
                    //��ʾ����Ӯ�ˡ����ɵ���һ���ڣ� Ȼ���û�����ѡ������һ��
                    stopClock();
                    QMessageBox mb(this);
                    mb.setIcon(QMessageBox::Question);
                    mb.setWindowTitle(tr("��Ӯ��"));
                    mb.setText(tr("��ϲ����Ӯ�ˡ��Ƿ�����һ�֣�"));
                    QPushButton btnOk(&mb), btnQuit(&mb);
                    btnOk.setText(tr("����һ��"));
                    btnQuit.setText(tr("�˳�"));
                    mb.addButton(&btnOk, QMessageBox::YesRole);
                    mb.addButton(&btnQuit, QMessageBox::NoRole);
                    connect(&btnOk, SIGNAL(clicked()), this, SLOT(requestForReplay()));
                    connect(&btnQuit, SIGNAL(clicked()), this, SLOT(close()));
                    mb.exec();
                }
            }
            else
            {
                deHighlightCurPiece(preViewPos.x(), preViewPos.y());
            }

            curSelected=false;
            preViewPos.setX(-1);
            preViewPos.setY(-1);
            delete[] eaten;
        }
    }

}


//������ʾ��ǰ���ӣ����������ͼ�����̵����꣬�����߼�����
void MainWindow::highlightCurPiece(int x, int y)
{
    label[x][y]->clear();
    label[x][y]->setPixmap(myhi);
    label[x][y]->show();

    qDebug("highlight");
}

void MainWindow::deHighlightCurPiece(int x, int y)
{
    label[x][y]->clear();
    label[x][y]->setPixmap(myPiece);
    label[x][y]->show();
}


//�����ͼ���������ñ���������ʧ
//eaten�Ǵ�move�������صĵ��ı������������ָ�룬���߼����꣬�ú������ڲ�ת��Ϊͼ������
void MainWindow::eatPieces(QPoint *eaten, int n)
{
    int i;
    QPoint *viewPts;

    if(isServer)
        viewPts=eaten;
    else
    {
        viewPts=new QPoint[n];
        for(i=0; i<n; i++)
        {
            viewPts[i].setX(3-eaten[i].x());
            viewPts[i].setY(3-eaten[i].y());
        }
    }

    for(i=0; i<n; i++)
    {
        label[ viewPts[i].x() ][ viewPts[i].y() ]->clear();
        label[ viewPts[i].x() ][ viewPts[i].y() ]->show();
    }
}


