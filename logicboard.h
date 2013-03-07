#ifndef LOGICBOARD_H
#define LOGICBOARD_H

#include <QPoint>

//����
class logicBoard
{
public :
    logicBoard();
    ~logicBoard();
    void initBoard();//���̳�ʼ��Ϊ����
    void setCurPos(int x, int y);
    void getCurPos(QPoint &cur);

    //�ƶ���target����������ƶ�����true(ͬʱeaten���鷵�سԵ�num������)�����򷵻�false
    bool move(const QPoint &target, QPoint *&eaten, int &num);

    //ÿ��move����󣬵���isOpponentLose�ж϶Է��Ƿ�����ˣ������������������
    //<1 �Է�ֻ��һ������
    //<2 �Է�û�п����ߵ��壨���Թ��� �� ����ס�ˣ�
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
