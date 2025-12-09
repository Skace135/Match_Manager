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

    statsView = new gui::StatsView(splitter);

    matchManager = new MatchManager(statsView, view);

    splitter->addWidget(statsView);
    splitter->addWidget(statsView->gridContainer);
    splitter->setSizes({400, 600});

    mainLayout->addWidget(splitter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

