#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include <QtGui/QWidget>

//startwidget界面功能
namespace Ui
{
    class StartWidget;
}

class StartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StartWidget(QWidget * parent=0);
    ~StartWidget();

    //public slots:

    //private slots:

private slots:
    void displayClientMessage(QString &msg);
    void displayServerMessage(QString &msg);
    void enterMainWindow();

    void on_pushButton_startdanji_clicked();

    void on_pushButton_clicked();

    void on_serverradioButton_clicked(bool checked);

    void on_clientradioButton_clicked(bool checked);

private:
        Ui::StartWidget *ui;
};

#endif // STARTWIDGET_H
