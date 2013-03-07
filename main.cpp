#include <QtGui/QApplication>
#include <QTextCodec>
#include "startwidget.h"
#include "communicator.h"
#include "mainwindow.h"

bool isServer;
Communicator * communicator;
MainWindow *w;
StartWidget * widget;
bool canContinue; //用来控制双方交替走棋，默认服务器先走


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //如果想在程序当中使用中文，就必须加上下面的语句
    QTextCodec::setCodecForTr( QTextCodec::codecForLocale());

    w=new MainWindow();
    widget=new StartWidget();
    widget->show();
    //w->show();

    return a.exec();
}
