#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow ctor";
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow dtor";
    delete ui;
}
