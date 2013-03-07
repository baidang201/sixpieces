#include "logicboard.h"
#include <cstdlib>
#include <QDebug>
using namespace std;
/*
**  这个类是表示逻辑上的棋盘的类。用一个4*4的矩阵表示棋盘：
**  矩阵中1表示客户端的棋子，用白色棋子
**  -1表示服务器端的棋子，用黑色棋子
**  0表示此处没有棋子
**  在逻辑棋盘中，服务器端棋子，即-1表示的棋子一开始位于下面两行
**  显示在图形棋盘上的时候，我方的棋子一开始位于屏幕的下方，注意于逻辑棋盘的差别
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
//在调用move行棋之前，必须要调用setCurPos设置要走的是那一颗棋
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
//eaten在内部分配，需要在外部删除
bool logicBoard::move(const QPoint &target, QPoint *&eaten, int &num)
{
    int i;
    eaten = new QPoint[3];  //每次只能吃不超过3个棋子？
    num = 0;
    qDebug("before:");
    for(i = 0; i < 4; i++)
        qDebug() << board[i][0] << board[i][1] << board[i][2] << board[i][3];
    if(target.x() == curX && target.y() == curY) //不能停在原地不动
        return false;
    if((abs(target.x() - curX) + abs(target.y() - curY)) == 1 && board[target.x()][target.y()] == 0)
    {
        board[curX][curY] = 0;
        board[target.x()][target.y()] = myValue;
        int counterValue = -myValue;
        /*
        **枪毙：当我行棋后，若我方有连续2颗或者3颗棋子挨着对方的一颗棋子时，即可将对方的一颗棋子吃掉
        **若用m代表我方棋子，c代表对方棋子，则枪毙对方棋子的模式有以下六种：
        **(1) m m m c
        **(2) m m c 0
        **(3) c m m 0
        **(4) c m m m
        **(5) 0 c m m
        **(6) 0 m m c
        */
        //check by hong [7] m m c m
        //              [8] m c m m
        // 7 8也可以枪毙

        //先检查target所在行是否可以枪毙
        if(board[target.x()][0] == myValue && board[target.x()][1] == myValue)//(1)(2)两种模式
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
        }//(3)(4)两种模式
        else if(board[target.x()][0] == counterValue && board[target.x()][1] == myValue
                && board[target.x()][2] == myValue && (board[target.x()][3] == 0 || board[target.x()][3] == myValue))
        {
            eaten[num].setX(target.x());
            eaten[num].setY(0);
            num++;
            board[target.x()][0] = 0;
        }//模式(5)
        else if(board[target.x()][0] == 0 && board[target.x()][1] == counterValue
                && board[target.x()][2] == myValue && board[target.x()][3] == myValue)
        {
            eaten[num].setX(target.x());
            eaten[num].setY(1);
            num++;
            board[target.x()][1] = 0;
        }//模式(6)
        else if(board[target.x()][0] == 0 && board[target.x()][1] == myValue
                && board[target.x()][2] == myValue && board[target.x()][3] == counterValue)
        {
            eaten[num].setX(target.x());
            eaten[num].setY(3);
            num++;
            board[target.x()][3] = 0;
        }
        else{;}
        //再检查target所在列是否可以枪毙
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
        **抬子：当我方行棋后，若对方的一颗棋子位于我方两颗棋子中间，且同行或同列没有对方的其他棋子，即可吃掉对方棋子
        **若用m代表我方棋子，c代表对方棋子，则枪毙对方棋子的模式有以下四种：
        **(1) m c m 0
        **(2) 0 m c m
        **(3) m m c m
        **(4) m c m m
        */
        //先检查target所在行是否可以抬子
        if(board[target.x()][0] == myValue && board[target.x()][1] == counterValue  //(1)(4)两种模式
                && board[target.x()][2] == myValue && board[target.x()][3] != counterValue)
        {
            eaten[num].setX(target.x());
            eaten[num].setY(1);
            num++;
            board[target.x()][1] = 0;
        }
        else if(board[target.x()][0] != counterValue && board[target.x()][1] == myValue  //(2)(3)两种模式
                && board[target.x()][2] == counterValue && board[target.x()][3] == myValue)
        {
            eaten[num].setX(target.x());
            eaten[num].setY(2);
            num++;
            board[target.x()][2] = 0;
        }
        else{;}
        //检查target所在列是否可以抬子
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
        **挑子：我方行棋后，若我方棋子位于对方两颗棋子的中间，且同行或同列中不存在对方的第三颗棋子，那么可以吃掉夹住我方棋子的对方两颗棋子
        **若用m代表我方棋子，c代表对方棋子，则枪毙对方棋子的模式有以下二种：
        **(1) c m c 0
        **(2) 0 c m c
        */
        //检查target所在行是否可以挑子
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
        //检查target所在列是否可以挑子
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
//检查(x,y)位置的棋子是否可以移动
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
