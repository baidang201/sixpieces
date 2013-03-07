/********************************************************************************
** Form generated from reading UI file 'startwidget.ui'
**
** Created: Fri Feb 22 22:36:20 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STARTWIDGET_H
#define UI_STARTWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StartWidget
{
public:
    QFrame *line;
    QGroupBox *groupBox_2;
    QPushButton *pushButton_startdanji;
    QGroupBox *groupBox;
    QRadioButton *serverradioButton;
    QRadioButton *clientradioButton;
    QLabel *serverLabel;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *iplineEdit;
    QLineEdit *portLineEdit;
    QPushButton *pushButton;
    QFrame *line_2;
    QLabel *clientLabel;

    void setupUi(QWidget *StartWidget)
    {
        if (StartWidget->objectName().isEmpty())
            StartWidget->setObjectName(QString::fromUtf8("StartWidget"));
        StartWidget->resize(480, 301);
        line = new QFrame(StartWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(220, 0, 20, 291));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        groupBox_2 = new QGroupBox(StartWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(240, 10, 211, 281));
        pushButton_startdanji = new QPushButton(groupBox_2);
        pushButton_startdanji->setObjectName(QString::fromUtf8("pushButton_startdanji"));
        pushButton_startdanji->setGeometry(QRect(70, 120, 75, 23));
        groupBox = new QGroupBox(StartWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 211, 281));
        serverradioButton = new QRadioButton(groupBox);
        serverradioButton->setObjectName(QString::fromUtf8("serverradioButton"));
        serverradioButton->setGeometry(QRect(20, 30, 121, 16));
        clientradioButton = new QRadioButton(groupBox);
        clientradioButton->setObjectName(QString::fromUtf8("clientradioButton"));
        clientradioButton->setGeometry(QRect(20, 120, 111, 16));
        serverLabel = new QLabel(groupBox);
        serverLabel->setObjectName(QString::fromUtf8("serverLabel"));
        serverLabel->setGeometry(QRect(10, 60, 191, 16));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(30, 150, 54, 12));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 180, 61, 21));
        iplineEdit = new QLineEdit(groupBox);
        iplineEdit->setObjectName(QString::fromUtf8("iplineEdit"));
        iplineEdit->setGeometry(QRect(80, 150, 113, 20));
        portLineEdit = new QLineEdit(groupBox);
        portLineEdit->setObjectName(QString::fromUtf8("portLineEdit"));
        portLineEdit->setGeometry(QRect(80, 180, 113, 20));
        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(40, 210, 111, 23));
        line_2 = new QFrame(groupBox);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(0, 100, 211, 16));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        clientLabel = new QLabel(groupBox);
        clientLabel->setObjectName(QString::fromUtf8("clientLabel"));
        clientLabel->setGeometry(QRect(10, 250, 191, 16));

        retranslateUi(StartWidget);

        QMetaObject::connectSlotsByName(StartWidget);
    } // setupUi

    void retranslateUi(QWidget *StartWidget)
    {
        StartWidget->setWindowTitle(QApplication::translate("StartWidget", "StartWidget", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("StartWidget", "\344\272\272\346\234\272\345\257\271\346\210\230", 0, QApplication::UnicodeUTF8));
        pushButton_startdanji->setText(QApplication::translate("StartWidget", "\344\273\216\345\215\225\346\234\272\345\220\257\345\212\250", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("StartWidget", "\345\261\200\345\237\237\347\275\221\346\270\270\346\210\217", 0, QApplication::UnicodeUTF8));
        serverradioButton->setText(QApplication::translate("StartWidget", "\344\275\234\344\270\272\346\234\215\345\212\241\347\253\257\345\220\257\345\212\250", 0, QApplication::UnicodeUTF8));
        clientradioButton->setText(QApplication::translate("StartWidget", "\344\275\234\344\270\272\345\256\242\346\210\267\347\253\257\345\220\257\345\212\250", 0, QApplication::UnicodeUTF8));
        serverLabel->setText(QApplication::translate("StartWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("StartWidget", "\346\234\215\345\212\241\345\231\250IP", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("StartWidget", "\346\234\215\345\212\241\345\231\250\347\253\257\345\217\243", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("StartWidget", "\350\277\236\346\216\245\346\234\215\345\212\241\347\253\257\345\220\257\345\212\250", 0, QApplication::UnicodeUTF8));
        clientLabel->setText(QApplication::translate("StartWidget", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class StartWidget: public Ui_StartWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STARTWIDGET_H
