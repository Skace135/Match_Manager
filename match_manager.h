#ifndef MATCH_MANAGER_H
#define MATCH_MANAGER_H

#include "view.h"

class MatchManager{
public:
    MatchManager(gui::StatsView* s, gui::View* v);
    void run();
    std::string e1_path;
    std::string e2_path;

private:
    gui::StatsView* statsView;
    gui::View* view;

};

#endif // MATCH_MANAGER_H
