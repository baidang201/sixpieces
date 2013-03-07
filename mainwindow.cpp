
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

    //使用资源路径代替原始文件系统路径
    if(!blackPiece.load(":/images/black.png"))//if(!blackPiece.load("D:\\qtwork\\sixchess\\graduationproject\\sixchessnewtesting\\images\\black.png"))
        qDebug()<<tr("加载黑棋图片失败");

    if(!blackhi.load(":/images/blackhi.png"))
        qDebug()<<tr("加载黑棋高亮图片失败");

    if(!whitePiece.load(":/images/white.png"))
        qDebug()<<tr("加载白棋图片失败");

    if(!whitehi.load(":/images/whitehi.png"))
        qDebug()<<tr("加载白棋高亮图片失败");

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
    qDebug()<<tr("mainwindow 初始化完成");

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

    //使用资源路径代替原始文件系统路径
    bool flag=boardPic.load(":/images/board.png");
    if(!flag)
        qDebug()<<tr("加载棋盘图片失败！");
    else
        qDebug()<<tr("加载棋盘图片成功！");

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

    if(isServer)//服务器先走
    {
        canContinue=true;
    }
    else
    {
        canContinue=false;
    }

    initClock();
    resetClock();// 必须设置完canContinue的值后再启动定时器
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
    QMessageBox::information(this, tr("消息"), tr("对方已退出"));
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

        myBoard.setValue(tox, toy, myBoard.getValue(fromx, fromy));//移动逻辑棋子
        myBoard.setValue(fromx, fromy, 0);

        //移动图形棋子， 注意此处移动的是对方的棋子，所有，如果对方是服务器，则需换坐标
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
            myBoard.setValue(piecex, piecey, 0);//吃掉逻辑棋子

            //吃掉图形棋子
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
        mb.setWindowTitle(tr("你输了"));
        mb.setText(tr("你输了， 是否再来一局?"));
        QPushButton btnOk(&mb),btnQuit(&mb);
        btnOk.setText(tr("再玩一局"));
        btnQuit.setText(tr("退出"));
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
        QMessageBox::information(this, tr("消息"), tr( "对方已退出"));
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
    QString oppoText=QString(tr("对方"));
    oppoLabel->setText(oppoText);
    oppoLabel->setFont(QFont(tr("黑体"),15));
    oppoLabel->move(boardWidth+40, 100);
    oppoLabel->show();

    QString oppoIp=QString(tr("IP:"))+communicator->getOpponentIp();
    oppoIpLabel->setText(oppoIp);
    oppoIpLabel->move(boardWidth+15, 120);
    oppoLabel->show();

    QString myText=QString(tr("我方"));
    myLabel->setText(myText);
    myLabel->setFont(QFont(tr("黑体"),15));
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
    targetClockLabel->setFont(QFont(tr("宋体"), 16));
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


//通过鼠标当前的位置pos，计算所点击位置是哪一颗棋子。如果点击的是某一颗棋子，子返回true， 否则点击的是空白处， 返回false
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

                if(isServer)//作为服务器一方，棋盘上显示与逻辑棋盘上得棋子位置一致
                {
                    logicPos.setX(i);
                    logicPos.setY(j);
                }
                else//作为服务器一方，棋盘的显示与逻辑棋盘旋转180以后的结果
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
《1》之前未选中任何棋子
-若当前点击位置不是合法棋子所在位置，nop
-若当前位置是我方棋子位置，则选中棋子，并高亮显示
-若当前位置的棋子不是我方棋子（空白棋子 或 对方棋子），nop

《2》之前选中一颗我方棋子
-若当前点击的是我方棋子，且不同于之前的棋子，则取消原来的棋子，重新选中当前棋子，并高亮当前棋子
                     若与之前的棋子相同，则取消选中当前棋子
-若当前的位置不是合法的棋子所在位置或是对方的棋子所在位置，则取消选中原来的棋子
-若当前的位置是空白棋子位置（logicBoard[cruPos.x()][curPox.y()] == 0）,则行棋，并取消原来的棋子
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
                //todo:添加行棋成功的代码，需要完成的事情
                //更新显示自己的棋盘，如果吃掉对方的棋子，让被吃掉的棋子消失，done
                //将行棋权交给对方，在此期间要屏蔽己方的任何操作
                //将己方的行棋结果（走的棋子，吃掉的棋子）发给对手，done
                //判断对方是否已经输了，如果是，则在本机棋盘上显示提示，并把结果发给对方
                //
                canContinue=false;
                resetClock();
                mySound->play();
                label[preViewPos.x()][preViewPos.y()]->clear();
                label[preViewPos.x()][preViewPos.y()]->show();
                label[viewPos.x()][viewPos.y()]->setPixmap(myPiece);
                label[viewPos.x()][viewPos.y()]->show();
				eatPieces(eaten, num);
                //将己方的走起结果发给对方
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
                    //提示“你赢了”，可弹出一窗口， 然后用户可以选择再来一局
                    stopClock();
                    QMessageBox mb(this);
                    mb.setIcon(QMessageBox::Question);
                    mb.setWindowTitle(tr("你赢了"));
                    mb.setText(tr("恭喜，您赢了。是否再来一局？"));
                    QPushButton btnOk(&mb), btnQuit(&mb);
                    btnOk.setText(tr("再玩一局"));
                    btnQuit.setText(tr("退出"));
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


//高亮显示当前棋子，参数是相对图形棋盘的坐标，不是逻辑棋盘
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


//完成在图形棋盘上让被吃棋子消失
//eaten是从move函数返回的到的被吃气质数组的指针，是逻辑坐标，该函数在内部转换为图形坐标
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


