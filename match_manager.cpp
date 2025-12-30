#include "match_manager.h"
#include <iostream>
#include <QTimer>
#include <QFileInfo>
#include <sstream>
#include <fstream>
#include "qdebug.h"
#include <cstdlib>

/* Commands available:
 * Time n : set time to n
 * Pos fen : set position to fen
 * Go: start search
 */

int decisiveEval = 1400;
constexpr int maxMoveCount = 300;

std::vector<std::string> MatchManager::s_balancedFens;


namespace {
std::vector<std::string> split(const std::string& s) {
    char delimiter = ' ';
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}
}

MatchManager::MatchManager(gui::StatsView* s, gui::View* v, QObject* parent) : QObject(parent){
    statsView = s;
    view = v;

    setup();
}

MatchManager::MatchManager(gui::StatsView* s, gui::View* v, QString e1Path, QString e2Path,
                           QLabel* ry, QLabel* bg, QObject* parent) : QObject(parent){
    statsView = s;
    view = v;

    e1_path = e1Path;
    e2_path = e2Path;

    e1_name = QFileInfo(e1Path).baseName();
    e2_name = QFileInfo(e1Path).baseName();

    ry_label = ry;
    bg_label = bg;

    setup();
}

void MatchManager::setup(){
    std::ifstream file("FENs_4PC_balanced.txt");
    if(!file) throw std::runtime_error("Cannot open file");

    if(s_balancedFens.empty()){
        std::string fen;
        while(std::getline(file, fen)){
            s_balancedFens.push_back(fen);
        }
    }

    e1 = new EngineProcess(this);
    e2 = new EngineProcess(this);

    connect(e1, &EngineProcess::outputReady,
            this, &MatchManager::onEngine1Output);
    connect(e2, &EngineProcess::outputReady,
            this, &MatchManager::onEngine2Output);
}


void MatchManager::run(){
    if(m_running) return;
    m_running = true;
    qDebug()<<"Starting engines";
    e1->start(e1_path);
    e2->start(e2_path);
    qDebug()<<"Engines started";
    m_games = statsView->max_games;

    if(e1_name == e2_name){
        e1_name += "_1";
        e2_name += "_2";
        statsView->modifyEngineNames(e1_name, e2_name);
    }

    QString timeCommand = "Time " + QString::number(statsView->think_time);
    e1->send(timeCommand);
    e2->send(timeCommand);

    startGame();

}

void MatchManager::startGame(){
    std::string startPos = getRandomFen();
    if(view) view->setupFromFen(startPos);
    std::string command = "Pos " + startPos;
    e1->send(QString::fromStdString(command));
    e2->send(QString::fromStdString(command));

    m_moveCount=0;
    m_e1Eval = 0;
    m_e2Eval = 0;
    s_gamesStarted++;
    m_switchSides = s_switchSides;
    s_switchSides = !s_switchSides;
    if(view){
        if(m_switchSides){
            ry_label->setText(e2_name);
            bg_label->setText(e1_name);
        }
        else{
            ry_label->setText(e1_name);
            bg_label->setText(e2_name);
        }
    }

    statsView->updateGameNumber(s_gamesStarted);

    timer.start();
    !m_switchSides ? e1->send("Go") : e2->send("Go");
}

std::string MatchManager::getRandomFen(){
    int randomNumber = rand() % 10000;
    while(s_usedFens[randomNumber]) randomNumber = rand() % 10000;
    s_usedFens[randomNumber] = true;

    return s_balancedFens[randomNumber];
}

void MatchManager::testFen(){
    std::string startPos = getRandomFen();
    if(view) view->setupFromFen(startPos);
}

void MatchManager::onEngine1Output(const QString& line){
    qint64 ms = timer.elapsed();
    qDebug("Elapsed time: %llims", ms);
    auto r = split(line.toStdString());
    m_moveCount++;
    m_e1Eval = std::stoi(r[5]);
    e2->send(QString::fromStdString("Move "+r[0]+" "+r[1]+" "+r[2]+" "+r[3]+" "+r[4]));
    chess::Move m = chess::Move(std::stoi(r[0]), std::stoi(r[1]), std::stoi(r[2]), std::stoi(r[3]), std::stoi(r[4]));
    qDebug("Move from: %i %i to: %i %i,  Evaluations: e1: %i, e2: %i, Move: %i",std::stoi(r[0]), std::stoi(r[1]), std::stoi(r[2]),std::stoi(r[3]), m_e1Eval, m_e2Eval, m_moveCount);
    if(view) view->playMove(m);
    if(evalsDecisive(m_e1Eval, m_e2Eval)){
        processResults(m_e1Eval, m_e2Eval);
        return;
    }
    if(m_moveCount >= maxMoveCount){
        terminateGame(m_e1Eval, m_e2Eval);
        return;
    }
    timer.start();
    e2->send("Go");
}

void MatchManager::onEngine2Output(const QString& line){
    qint64 ms = timer.elapsed();
    qDebug("Elapsed time: %ims", ms);
    auto r = split(line.toStdString());
    m_moveCount++;
    m_e2Eval = std::stoi(r[5]);
    e1->send(QString::fromStdString("Move "+r[0]+" "+r[1]+" "+r[2]+" "+r[3]+" "+r[4]));
    chess::Move m = chess::Move(std::stoi(r[0]), std::stoi(r[1]), std::stoi(r[2]), std::stoi(r[3]), std::stoi(r[4]));
    qDebug("Move from: %i %i to: %i %i,  Evaluations: e1: %i, e2: %i, Move: %i",std::stoi(r[0]), std::stoi(r[1]), std::stoi(r[2]),std::stoi(r[3]), m_e1Eval, m_e2Eval, m_moveCount);
    if(view) view->playMove(m);
    if(evalsDecisive(m_e1Eval, m_e2Eval)){
        processResults(m_e1Eval, m_e2Eval);
        return;
    }
    if(m_moveCount >= maxMoveCount){
        terminateGame(m_e1Eval, m_e2Eval);
        return;
    }
    timer.start();
    e1->send("Go");
}

void MatchManager::processResults(int e1Eval, int e2Eval){
    ++s_gamesEnded;
    if(e1Eval > 0)
        //RY win
        if(!m_switchSides)
            //e1 is RY -> win e1
            statsView->updateResults(1, 0);
        else
            //e2 is RY -> win e2
            statsView->updateResults(0, 1);
    else if (e1Eval < 0)
        //BG win
        if(!m_switchSides)
            //e2 is BG -> win e2
            statsView->updateResults(0, 1);
        else
            //e1 is BG -> win e1
            statsView->updateResults(1, 0);
    else if(e1Eval == 0)
        statsView->updateResults(0.5, 0.5);

    if(s_gamesStarted < m_games){
        startGame();
    }
    if(s_gamesEnded == m_games){
        statsView->displayResults();
    }
}

void MatchManager::terminateGame(int e1Eval, int e2Eval){
    ++s_gamesEnded;
    if(std::min(e1Eval, e2Eval) > 500){
        //RY win
        if(!m_switchSides)
            //e1 is RY -> win e1
            statsView->updateResults(1, 0);
        else
            //e2 is RY -> win e2
            statsView->updateResults(0, 1);
    }
    else if (std::max(e1Eval, e2Eval) < -500){
        //BG win
        if(!m_switchSides)
            //e2 is BG -> win e2
            statsView->updateResults(0, 1);
        else
            //e1 is BG -> win e1
            statsView->updateResults(1, 0);
    }
    else{
        statsView->updateResults(0.5, 0.5);
    }
    if(s_gamesStarted < m_games){
        startGame();
    }
    if(s_gamesEnded == m_games){
        statsView->displayResults();
    }
}

bool MatchManager::evalsDecisive(int e1Eval, int e2Eval){
    return std::abs(e1Eval)>=decisiveEval && std::abs(e2Eval)>=decisiveEval
           && (e1Eval<0 && e2Eval<0 || e1Eval>0 && e2Eval>0);
}
