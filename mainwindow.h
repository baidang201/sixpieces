#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMouseEvent>
#include <QSound>
#include <QTimer>

#include "logicboard.h"
//mainwindow界面
extern bool isServer;

namespace Ui{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget * parent=0);
    void setServer(bool iamServer);
    void paintEvent(QPaintEvent * pe);
    void mousePressEvent(QMouseEvent * me);
    void calcPosition(const QPoint &pos, QPoint &logicPos, QPoint &viewPos );
    void highlightCurPiece(int x, int y);
    void deHighlightCurPiece(int x, int y);
    void eatPieces(QPoint *eaten, int n);
    void initLabels();
    void initPieces();
    void initClock();
    void resetClock();
    void stopClock();
    ~MainWindow();

public slots:
    void replay();
    void requestForReplay();
    void acceptReplay();
    void opponentClosed();
    void handleOpponentMessage(QString &msg);
    void updateClock();

private:
    void processClick(const QPoint &logicPos, const QPoint & viewPos);
    Ui::MainWindow * ui;
    QLabel * label[4][4];
    QPixmap boardPic;
    QPixmap blackPiece, blackhi, whitehi, whitePiece,myPiece, myhi, counterhi, counterPiece;
    int myValue;
    logicBoard myBoard;
    bool curSelected;//当前是否选中棋子
    QPoint preViewPos;//选中棋子在棋盘中的位置
    QSound *mySound, *oppoSound;
    const static int windowWidth=773;
    const static int boardWidth=653;
    const static int windowHeight=653;
    QLabel * oppoLabel, *oppoIpLabel, *myLabel, *oppoClockLabel, *myClockLabel, *targetClockLabel;
    QTimer *timer;
    int hour, min, sec;
};

#endif // MAINWINDOW_H
