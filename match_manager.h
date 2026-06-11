#ifndef MATCH_MANAGER_H
#define MATCH_MANAGER_H

#include "view.h"
#include "engineProcess.h"
#include <QElapsedTimer>


class MatchManager : public QObject {
    Q_OBJECT
public:
    MatchManager(gui::StatsView* s, gui::View* v, QObject* parent=nullptr);
    MatchManager(gui::StatsView* s, gui::View* v, QString e1Path, QString e2Path,
                 QObject* parent=nullptr);
    void run();
    QString e1_path;
    QString e2_path;
    QString e1_name;
    QString e2_name;
    void testFen();
    void stop();

private:
    static std::vector<std::string> s_balancedFens;
    inline static bool s_usedFens[10000]{};
    inline static int s_gamesStarted = 0;
    inline static int s_gamesEnded = 0;
    static bool evalsDecisive(int e1Eval, int e2Eval);
    void setup();
    void onEngine1Output(const QString& line);
    void onEngine2Output(const QString& line);
    void processResults(int e1Eval, int e2Eval);
    void terminateGame(int e1Eval, int e2Eval);
    void terminate();
    gui::StatsView* statsView;
    gui::View* view;
    EngineProcess* e1;
    EngineProcess* e2;
    QElapsedTimer timer;
    bool m_running = false;
    bool m_switchSides = false;
    int m_moveCount;
    int m_e1Eval;
    int m_e2Eval;
    int m_games;
    void startGame();
    std::string getRandomFen();
    std::string startPos;

};

#endif // MATCH_MANAGER_H
