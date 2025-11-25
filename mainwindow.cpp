#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "view.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gui::View* view = new gui::View(this);
    std::string s = R"(R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,1,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,yP,yP,yP,yP,yP,yP,yP,x,x,x/x,x,x,2,yN,5,x,x,x/bR,bP,10,gP,gR/bN,bP,9,gP,1,gN/bB,bP,10,gP,gB/bQ,bP,10,gP,gK/bK,bP,10,gP,gQ/bB,bP,10,gP,gB/bN,bP,10,gP,gN/bR,2,bP,2,rP,5,gP,gR/x,x,x,8,x,x,x/x,x,x,rP,rP,rP,1,rP,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x)";
    view->setupFromFen(s);
    setCentralWidget(view);

}

MainWindow::~MainWindow()
{
    delete ui;
}

