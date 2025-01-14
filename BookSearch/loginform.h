#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QMainWindow>
#include "mainwindow.h"
#include <QWidget>
#include "ui_loginform.h"  // Newly added line

namespace Ui {
    class LoginForm;
}

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class LoginForm : public QMainWindow
{
    Q_OBJECT

public:
    LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

private slots:
    void on_btnLogin_clicked();

private:
    Ui::LoginForm *ui; // Change 'Ui::loginform *ui' to 'Ui::LoginForm *ui'
    MainWindow *mainwindow;
};
#endif // LOGINFORM_H
