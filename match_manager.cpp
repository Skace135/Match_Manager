#include "match_manager.h"
#include <iostream>
#include <QTimer>
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
    statsView->mm = this;


    std::ifstream file("FENs_4PC_balanced.txt");
    if(!file) throw std::runtime_error("Cannot open file");

    std::string fen;
    while(std::getline(file, fen)){
        m_balanced_fens.push_back(fen);
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
        e1_name += "1";
        e2_name += "2";
        statsView->modifyEngineNames(e1_name, e2_name);
    }

    QString timeCommand = "Time " + QString::number(statsView->think_time);
    e1->send(timeCommand);
    e2->send(timeCommand);

    startGame();

}

void MatchManager::startGame(){
    std::string startPos = getRandomFen();
    view->setupFromFen(startPos);
    std::string command = "Pos " + startPos;
    e1->send(QString::fromStdString(command));
    e2->send(QString::fromStdString(command));

    m_moveCount=0;
    m_e1Eval = 0;
    m_e2Eval = 0;
    m_gamesStarted++;
    e1_toMove = !switchSides;
    switchSides ? statsView->setPlayerLabels(e2_name, e1_name) : statsView->setPlayerLabels(e1_name, e2_name);
    m_gameOver = false;

    statsView->updateGameNumber(m_gamesStarted);

    timer.start();
    e1_toMove ? e1->send("Go") : e2->send("Go");
}

std::string MatchManager::getRandomFen(){
    int randomNumber = rand() % 10000;
    while(m_usedFens[randomNumber]) randomNumber = rand() % 10000;
    m_usedFens[randomNumber] = true;

    return m_balanced_fens[randomNumber];
}

void MatchManager::testFen(){
    std::string startPos = getRandomFen();
    view->setupFromFen(startPos);
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
    view->movePieces(m);
    if(evalsDecisive(m_e1Eval, m_e2Eval)){
        processResults(m_e1Eval, m_e2Eval);
        return;
    }
    if(m_moveCount >= maxMoveCount){
        terminateGame(m_e1Eval, m_e2Eval);
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
    view->movePieces(m);
    if(evalsDecisive(m_e1Eval, m_e2Eval)){
        processResults(m_e1Eval, m_e2Eval);
        return;
    }
    if(m_moveCount >= maxMoveCount){
        terminateGame(m_e1Eval, m_e2Eval);
    }
    timer.start();
    e1->send("Go");
}

void MatchManager::processResults(int e1Eval, int e2Eval){
    if(e1Eval > 0)
        //RY win
        if(!switchSides)
            //e1 is RY -> win e1
            statsView->updateResults(1, 0);
        else
            //e2 is RY -> win e2
            statsView->updateResults(0, 1);
    else if (e1Eval < 0)
        //BG win
        if(!switchSides)
            //e2 is BG -> win e2
            statsView->updateResults(0, 1);
        else
            //e1 is BG -> win e1
            statsView->updateResults(1, 0);
    else if(e1Eval == 0)
        statsView->updateResults(0.5, 0.5);

    if(m_gamesStarted < m_games){
        switchSides = !switchSides;
        startGame();
    }
}

void MatchManager::terminateGame(int e1Eval, int e2Eval){
    if(std::min(e1Eval, e2Eval) > 500){
        //RY win
        if(!switchSides)
            //e1 is RY -> win e1
            statsView->updateResults(1, 0);
        else
            //e2 is RY -> win e2
            statsView->updateResults(0, 1);
    }
    else if (std::max(e1Eval, e2Eval) < -500){
        //BG win
        if(!switchSides)
            //e2 is BG -> win e2
            statsView->updateResults(0, 1);
        else
            //e1 is BG -> win e1
            statsView->updateResults(1, 0);
    }
    else{
        statsView->updateResults(0.5, 0.5);
    }
    if(m_gamesStarted < m_games){
        switchSides = !switchSides;
        startGame();
    }
}

bool MatchManager::evalsDecisive(int e1Eval, int e2Eval){
    return std::abs(e1Eval)>=decisiveEval && std::abs(e2Eval)>=decisiveEval
           && (e1Eval<0 && e2Eval<0 || e1Eval>0 && e2Eval>0);
}
