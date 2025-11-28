#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsLinearLayout>
#include <QLineEdit>
#include <utility>
#include "board.h"
#include "gui.h"

namespace gui {

constexpr int boardSize = 14;

class View : public QGraphicsView {
    Q_OBJECT
public:
    View(QWidget *parent = nullptr);
    Tile* getTile(QPointF pos);
    void playMove(chess::Move move);
    std::pair<Tile*, Tile*> hl_tiles[4];
    void displayLogicalBoard();
    void setupFromFen(const std::string& fen);
    void movePieces(chess::Move move);

protected:
    void resizeEvent(QResizeEvent *event);
    //void mousePressEvent(QMouseEvent* event);

private:
    QGraphicsScene *scene;
    chess::Color toMove;
    void setup(chess::Square board[14][14]);
    void addAnimation(Tile* from, Tile* to);
    void addPromotionAnimation(Tile* from, Tile* to, chess::Piece piece, chess::Color color);
    QPointF getSquareCenter(int row, int col);
    gui::Tile* board_[boardSize][boardSize];
    std::array<chess::BoardLocation, 4> check_locations;
};

class StatsView : public QGraphicsView {
    Q_OBJECT
public:
    StatsView(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void onTimeEditFinished();
    void onGamesEditFinished();

private:
    QGraphicsScene *scene;
    QGraphicsLinearLayout* vLayout;
    QLabel* e1_label;
    QLabel* e2_label;
    QLabel* e1Score_label;
    QLabel* e2Score_label;
    QLabel* gameNumber_label;
    QLabel* maxGames_label;
    QLineEdit* timeEdit;
    QLineEdit* gamesEdit;
    double e1Score = 0;
    double e2Score = 0;
    int game_number=0;
    int max_games = 100;
    int think_time = 2500;
};

}
#endif // VIEW_H
