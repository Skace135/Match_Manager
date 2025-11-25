#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <utility>
#include "board.h"
#include "gui.h"

#endif // VIEW_H
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

protected:
    void resizeEvent(QResizeEvent *event);
    //void mousePressEvent(QMouseEvent* event);

private:
    QGraphicsScene *scene;
    chess::Color toMove;
    void setup(chess::Square board[14][14]);
    void movePieces(chess::Move move);
    void addAnimation(Tile* from, Tile* to);
    void addPromotionAnimation(Tile* from, Tile* to, chess::Piece piece, chess::Color color);
    QPointF getSquareCenter(int row, int col);
    gui::Tile* board_[boardSize][boardSize];
    std::array<chess::BoardLocation, 4> check_locations;
};

}
