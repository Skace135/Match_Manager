#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qboxlayout.h>
#include <QSplitter>
#include <qlabel.h>
#include <QGroupBox>
#include <QFormLayout>
#include "match_manager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup();
}

void MainWindow::setup(){
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, centralWidget);


    view = new gui::View(splitter);
    std::string s = R"(R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,1,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,yP,yP,yP,yP,yP,yP,yP,x,x,x/x,x,x,2,yN,5,x,x,x/bR,bP,10,gP,gR/bN,bP,9,gP,1,gN/bB,bP,10,gP,gB/bQ,bP,10,gP,gK/bK,bP,10,gP,gQ/bB,bP,10,gP,gB/bN,bP,10,gP,gN/bR,2,bP,2,rP,5,gP,gR/x,x,x,8,x,x,x/x,x,x,rP,rP,rP,1,rP,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x)";
    view->setupFromFen(s);

    statsView = new gui::StatsView(splitter);

    matchManager = new MatchManager(statsView, view);


    splitter->addWidget(statsView);
    splitter->addWidget(view);
    splitter->setSizes({300, 600});

    mainLayout->addWidget(splitter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

