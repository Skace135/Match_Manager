#include "match_manager.h"

MatchManager::MatchManager(gui::StatsView* s, gui::View* v){
    statsView = s;
    view = v;
    statsView->mm = this;
}

void MatchManager::run(){

}
