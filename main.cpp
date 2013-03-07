#include <QtGui/QApplication>
#include <QTextCodec>
#include "startwidget.h"
#include "communicator.h"
#include "mainwindow.h"

bool isServer;
Communicator * communicator;
MainWindow *w;
StartWidget * widget;
bool canContinue; //��������˫���������壬Ĭ�Ϸ���������


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //������ڳ�����ʹ�����ģ��ͱ��������������
    QTextCodec::setCodecForTr( QTextCodec::codecForLocale());

    w=new MainWindow();
    widget=new StartWidget();
    widget->show();
    //w->show();

    return a.exec();
}
