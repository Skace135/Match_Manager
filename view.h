#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsLinearLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <utility>
#include "board.h"
#include "gui.h"

class MatchManager;

namespace gui {

constexpr int boardSize = 14;
constexpr int editWidth = 50;

class View : public QGraphicsView {
    Q_OBJECT
public:
    View(int n, QWidget *parent = nullptr);
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
    int number;
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
    std::vector<gui::View*> views;
    QWidget* gridContainer;
    QGridLayout* grid;
    void modifyEngineNames(QString e1_name, QString e2_name);
    void updateResults(double e1Points, double e2Points);
    void updateGameNumber(int gameNumber);
    void addPlayerRow(QLabel*& RY_label, QLabel*& BG_label, int viewNumber);
    int max_games = 100;
    int think_time = 2500;

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void onTimeEditFinished();
    void onGamesEditFinished();
    void onConcurrencyEditFinished();
    void onVisibilityEditFinished();
    void on_e1ButtonClicked();
    void on_e2ButtonClicked();
    void onMatchButtonClicked();

private:
    std::vector<MatchManager*> matchManagers;
    void addFileRows();
    void addMatchupRows();
    void addMatchButtonRow();
    void adjustViewNumbers(int number);
    void clearGridLayout();
    void addEditLine(const QString& text, QLineEdit*& edit, int editContent);
    void connectSlots();
    void removeUIElements();
    std::vector<QWidget*> uiElements;
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
    QLineEdit* concurrencyEdit;
    QLineEdit* visibilityEdit;
    QLabel* e1_edit;
    QLabel* e2_edit;
    QPushButton* matchButton;
    QPushButton* e1_browseButton;
    QPushButton* e2_browseButton;
    double e1Score = 0;
    double e2Score = 0;
    int game_number=0;
    int concurrent_games = 7;
    int visible_games = 7;
    bool matches_started=false;
};

}
#endif // VIEW_H
