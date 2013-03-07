#ifndef LOGICBOARD_H
#define LOGICBOARD_H

#include <QPoint>

//棋盘
class logicBoard
{
public :
    logicBoard();
    ~logicBoard();
    void initBoard();//棋盘初始化为开局
    void setCurPos(int x, int y);
    void getCurPos(QPoint &cur);

    //移动到target，如果可以移动返回true(同时eaten数组返回吃的num个棋子)，否则返回false
    bool move(const QPoint &target, QPoint *&eaten, int &num);

    //每次move行棋后，调用isOpponentLose判断对方是否输掉了，若有以下情况则输棋
    //<1 对方只有一颗棋子
    //<2 对方没有可以走的棋（被吃光了 或 被困住了）
    bool isOpponentLose();
    void setValue(int x, int y, int value);
    int getValue(int x, int y);


private:
    bool canMove(int x, int y);
    int board[4][4];
    int curX, curY;
    int myValue;

};

#endif // LOGICBOARD_H
