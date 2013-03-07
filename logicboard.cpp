#include "logicboard.h"
#include <cstdlib>
#include <QDebug>
using namespace std;
/*
**  ������Ǳ�ʾ�߼��ϵ����̵��ࡣ��һ��4*4�ľ����ʾ���̣�
**  ������1��ʾ�ͻ��˵����ӣ��ð�ɫ����
**  -1��ʾ�������˵����ӣ��ú�ɫ����
**  0��ʾ�˴�û������
**  ���߼������У������������ӣ���-1��ʾ������һ��ʼλ����������
**  ��ʾ��ͼ�������ϵ�ʱ���ҷ�������һ��ʼλ����Ļ���·���ע�����߼����̵Ĳ��
*/
logicBoard::logicBoard()
{
    initBoard();
}

void logicBoard::initBoard()
{
    int j;
    for(j = 0; j < 4; j++)
    {
        board[0][j] = 1;
        board[3][j] = -1;
    }
    board[1][0] = 1;
    board[1][1] = 0;
    board[1][2] = 0;
    board[1][3] = 1;
    board[2][0] = -1;
    board[2][1] = 0;
    board[2][2] = 0;
    board[2][3] = -1;
}

logicBoard::~logicBoard()
{
}
//�ڵ���move����֮ǰ������Ҫ����setCurPos����Ҫ�ߵ�����һ����
void logicBoard::setCurPos(int x, int y)
{
    curX = x;
    curY = y;
    myValue = board[curX][curY];
}
void logicBoard::getCurPos(QPoint &cur)
{
    cur.setX(curX);
    cur.setY(curY);
}
//eaten���ڲ����䣬��Ҫ���ⲿɾ��
bool logicBoard::move(const QPoint &target, QPoint *&eaten, int &num)
{
    int i;
    eaten = new QPoint[3];  //ÿ��ֻ�ܳԲ�����3�����ӣ�
    num = 0;
    qDebug("before:");
    for(i = 0; i < 4; i++)
        qDebug() << board[i][0] << board[i][1] << board[i][2] << board[i][3];
    if(target.x() == curX && target.y() == curY) //����ͣ��ԭ�ز���
        return false;
    if((abs(target.x() - curX) + abs(target.y() - curY)) == 1 && board[target.x()][target.y()] == 0)
    {
        board[curX][curY] = 0;
        board[target.x()][target.y()] = myValue;
        int counterValue = -myValue;
        /*
        **ǹ�У�������������ҷ�������2�Ż���3�����Ӱ��ŶԷ���һ������ʱ�����ɽ��Է���һ�����ӳԵ�
        **����m�����ҷ����ӣ�c����Է����ӣ���ǹ�жԷ����ӵ�ģʽ���������֣�
        **(1) m m m c
        **(2) m m c 0
        **(3) c m m 0
        **(4) c m m m
        **(5) 0 c m m
        **(6) 0 m m c
        */
        //check by hong [7] m m c m
        //              [8] m c m m
        // 7 8Ҳ����ǹ��

        //�ȼ��target�������Ƿ����ǹ��
        if(board[target.x()][0] == myValue && board[target.x()][1] == myValue)//(1)(2)����ģʽ
        {
            if(board[target.x()][2] == myValue && board[target.x()][3] == counterValue)
            {
                eaten[num].setX(target.x());
                eaten[num].setY(3);
                num++;
                board[target.x()][3] = 0;
            }
            else if(board[target.x()][2] == counterValue && board[target.x()][3] == 0)
            {
                eaten[num].setX(target.x());
                eaten[num].setY(2);
                num++;
                board[target.x()][2] = 0;
            }
        }//(3)(4)����ģʽ
        else if(board[target.x()][0] == counterValue && board[target.x()][1] == myValue
                && board[target.x()][2] == myValue && (board[target.x()][3] == 0 || board[target.x()][3] == myValue))
        {
            eaten[num].setX(target.x());
            eaten[num].setY(0);
            num++;
            board[target.x()][0] = 0;
        }//ģʽ(5)
        else if(board[target.x()][0] == 0 && board[target.x()][1] == counterValue
                && board[target.x()][2] == myValue && board[target.x()][3] == myValue)
        {
            eaten[num].setX(target.x());
            eaten[num].setY(1);
            num++;
            board[target.x()][1] = 0;
        }//ģʽ(6)
        else if(board[target.x()][0] == 0 && board[target.x()][1] == myValue
                && board[target.x()][2] == myValue && board[target.x()][3] == counterValue)
        {
            eaten[num].setX(target.x());
            eaten[num].setY(3);
            num++;
            board[target.x()][3] = 0;
        }
        else{;}
        //�ټ��target�������Ƿ����ǹ��
        if(board[0][target.y()] == myValue && board[1][target.y()] == myValue)
        {
            if(board[2][target.y()] == myValue && board[3][target.y()] == counterValue)
            {
                eaten[num].setX(3);
                eaten[num].setY(target.y());
                num++;
                board[3][target.y()] = 0;
            }
            else if(board[2][target.y()] == counterValue && board[3][target.y()] == 0)
            {
                eaten[num].setX(2);
                eaten[num].setY(target.y());
                num++;
                board[2][target.y()] = 0;
            }
        }
        else if(board[0][target.y()] == counterValue && board[1][target.y()] == myValue
                && board[2][target.y()] == myValue && board[3][target.y()] != counterValue)
        {
            eaten[num].setX(0);
            eaten[num].setY(target.y());
            num++;
            board[0][target.y()] = 0;
        }
        else if(board[0][target.y()] == 0 && board[2][target.y()] == myValue)
        {
            if(board[1][target.y()] == counterValue && board[3][target.y()] == myValue)
            {
                eaten[num].setX(1);
                eaten[num].setY(target.y());
                num++;
                board[1][target.y()] = 0;
            }
            else if(board[3][target.y()] == counterValue && board[1][target.y()] == myValue)
            {
                eaten[num].setX(3);
                eaten[num].setY(target.y());
                num++;
                board[3][target.y()] = 0;
            }
        }
        else{;}
        /*
        **̧�ӣ����ҷ���������Է���һ������λ���ҷ����������м䣬��ͬ�л�ͬ��û�жԷ����������ӣ����ɳԵ��Է�����
        **����m�����ҷ����ӣ�c����Է����ӣ���ǹ�жԷ����ӵ�ģʽ���������֣�
        **(1) m c m 0
        **(2) 0 m c m
        **(3) m m c m
        **(4) m c m m
        */
        //�ȼ��target�������Ƿ����̧��
        if(board[target.x()][0] == myValue && board[target.x()][1] == counterValue  //(1)(4)����ģʽ
                && board[target.x()][2] == myValue && board[target.x()][3] != counterValue)
        {
            eaten[num].setX(target.x());
            eaten[num].setY(1);
            num++;
            board[target.x()][1] = 0;
        }
        else if(board[target.x()][0] != counterValue && board[target.x()][1] == myValue  //(2)(3)����ģʽ
                && board[target.x()][2] == counterValue && board[target.x()][3] == myValue)
        {
            eaten[num].setX(target.x());
            eaten[num].setY(2);
            num++;
            board[target.x()][2] = 0;
        }
        else{;}
        //���target�������Ƿ����̧��
        if(board[0][target.y()] == myValue && board[1][target.y()] == counterValue
                && board[2][target.y()] == myValue && board[3][target.y()] != counterValue)
        {
            eaten[num].setX(1);
            eaten[num].setY(target.y());
            num++;
            board[1][target.y()] = 0;
        }
        else if(board[0][target.y()] != counterValue && board[1][target.y()] == myValue
                && board[2][target.y()] == counterValue && board[3][target.y()] == myValue)
        {
            eaten[num].setX(2);
            eaten[num].setY(target.y());
            num++;
            board[2][target.y()] = 0;
        }
        else{;}
        /*
        **���ӣ��ҷ���������ҷ�����λ�ڶԷ��������ӵ��м䣬��ͬ�л�ͬ���в����ڶԷ��ĵ��������ӣ���ô���ԳԵ���ס�ҷ����ӵĶԷ���������
        **����m�����ҷ����ӣ�c����Է����ӣ���ǹ�жԷ����ӵ�ģʽ�����¶��֣�
        **(1) c m c 0
        **(2) 0 c m c
        */
        //���target�������Ƿ��������
        if(board[target.x()][0] == counterValue && board[target.x()][1] == myValue
                && board[target.x()][2] == counterValue && board[target.x()][3] == 0)
        {
            eaten[num].setX(target.x());
            eaten[num++].setY(0);
            board[target.x()][0] = 0;
            eaten[num].setX(target.x());
            eaten[num++].setY(2);
            board[target.x()][2] = 0;
        }
        else if(board[target.x()][0] == 0 && board[target.x()][1] == counterValue
                && board[target.x()][2] == myValue && board[target.x()][3] == counterValue)
        {
            eaten[num].setX(target.x());
            eaten[num++].setY(1);
            board[target.x()][1] = 0;
            eaten[num].setX(target.x());
            eaten[num++].setY(3);
            board[target.x()][3] = 0;
        }
        else{;}
        //���target�������Ƿ��������
        if(board[0][target.y()] == counterValue && board[1][target.y()] == myValue
                && board[2][target.y()] == counterValue && board[3][target.y()] == 0)
        {
            eaten[num].setX(0);
            eaten[num++].setY(target.y());
            board[0][target.y()] = 0;
            eaten[num].setX(2);
            eaten[num++].setY(target.y());
            board[2][target.y()] = 0;
        }
        else if(board[0][target.y()] == 0 && board[1][target.y()] == counterValue
                && board[2][target.y()] == myValue && board[3][target.y()] == counterValue)
        {
            eaten[num].setX(1);
            eaten[num++].setY(target.y());
            board[1][target.y()] = 0;
            eaten[num].setX(3);
            eaten[num++].setY(target.y());
            board[3][target.y()] = 0;
        }
        else{;}
        qDebug("after:");
        for(i = 0; i < 4; i++)
            qDebug() << board[i][0] << board[i][1] << board[i][2] << board[i][3];
        return true;
    }
    return false;
}

bool logicBoard::isOpponentLose()
{
    int opponentValue = -myValue;
    int i, j, count;
    count = 0;
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            if(board[i][j] == opponentValue)
                count++;
    if(count <= 1)
        return true;
    bool hasAMove = false;
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
            if(board[i][j] == opponentValue && canMove(i, j))
            {
                hasAMove = true;
                break;
            }
        if(hasAMove)
            break;
    }
    if(hasAMove)
        return false;
    else
        return true;
}
//���(x,y)λ�õ������Ƿ�����ƶ�
bool logicBoard::canMove(int x, int y)
{
    int k;
    int xshift[4] = {-1, 1, 0, 0};
    int yshift[4] = {0, 0, -1, 1};
    for(k = 0; k < 4; k++)
    {
        int neighborX, neighborY;
        neighborX = xshift[k] + x;
        neighborY = yshift[k] + y;
        if(neighborX >= 0 && neighborX < 4 && neighborY >= 0 && neighborY < 4
                && board[neighborX][neighborY] == 0)
            return true;
    }
    return false;
}

int logicBoard::getValue(int x, int y)
{
    return board[x][y];
}

void logicBoard::setValue(int x, int y, int value)
{
    if(value == 0 || value == 1 || value == -1)
        board[x][y] = value;
}
