#ifndef MATCH_MANAGER_H
#define MATCH_MANAGER_H

#include "view.h"
#include "engineProcess.h"


class MatchManager : public QObject {
    Q_OBJECT
public:
    MatchManager(gui::StatsView* s, gui::View* v, QObject* parent=nullptr);
    void run();
    QString e1_path;
    QString e2_path;
    QString e1_name;
    QString e2_name;
    void testFen();

private:
    void onEngine1Output(const QString& line);
    void onEngine2Output(const QString& line);
    void processResults(int e1Eval, int e2Eval);
    void terminateGame(int e1Eval, int e2Eval);
    static bool evalsDecisive(int e1Eval, int e2Eval);
    gui::StatsView* statsView;
    gui::View* view;
    EngineProcess* e1;
    EngineProcess* e2;
    QElapsedTimer timer;
    bool e1_toMove = true;
    bool switchSides = false;
    bool m_running = false;
    int m_gamesStarted=0;
    int m_moveCount;
    bool m_gameOver;
    int m_e1Eval;
    int m_e2Eval;
    int m_games;
    std::vector<std::string> m_balanced_fens;
    bool m_usedFens[10000]{};
    void startGame();
    std::string getRandomFen();

};

#endif // MATCH_MANAGER_H
