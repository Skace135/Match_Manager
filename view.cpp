#include "view.h"
#include "search.h"
#include <QPropertyAnimation>
#include <QTimer>
#include <QKeyEvent>
#include <qgraphicssceneevent.h>
#include <thread>
#include <bitset>
#include <sstream>

/*TODO
 * Try out move ordering with SEE
 * Implement arrows
 * (Optional) increase arrowhead width
 * */

/*FIXES:
 * King capture while checked
 * King check pin bug
 */


/*  zvalues: tile: 0
 *           piece: 1
 *           arrow: 2 (set inside arrow class)
 */

/*  Debugging options:
 *  -show legal moves: set d_legalMoves = true
 *  -show number of pseudolegal moves: set d_pseudoMoveCount = true*/

/*  BUGS:
 *  Crashes sometimes when green mated but blues turn as move isnt set then
 *  Process crashes sometimes
 *  Castling not possible during game
 */

namespace gui {

std::chrono::duration<double> totalElapsed1=(std::chrono::duration<double>)0;
std::chrono::duration<double> totalElapsed2=(std::chrono::duration<double>)0;
std::chrono::duration<double> totalSelfPlayElapsed=(std::chrono::duration<double>)0;
int moveCounter = 0;

View::View(QWidget *parent) : QGraphicsView(parent) {
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, squareSize * 14, squareSize * 14); // Set your desired width and height
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);


    chess::Square board[14][14];
    for(int i=0; i<14; i++){
        for(int j=0; j<14; j++){
            board[i][j] = chess::standardBoard[i][j];
        }
    }
    setup(board);
}


void View::resizeEvent(QResizeEvent *event){
    QGraphicsView::resizeEvent(event);
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void View::setup(chess::Square board[boardSize][boardSize]){
    for(int i=0; i<boardSize; i++){
        for(int j=0; j<boardSize; j++){
            if(!board[i][j].valid()) {
                board_[i][j] = nullptr;
                continue;
            }
            QColor color = ((i + j) % 2 == 0) ? Qt::white : Qt::lightGray;
            gui::Tile* tile = new gui::Tile(color, this);
            tile->setPos(j*squareSize, i*squareSize);   //NOTE THAT i AND j ARE EXCHANGED AS ROWS=Y COLS=X
            tile->setZValue(0);
            scene->addItem(tile);
            board_[i][j] = tile;
            if(!board[i][j].empty()) {
                QChar pieceSymbol;
                switch (board[i][j].getPiece()) {
                case chess::PAWN:
                    pieceSymbol = u'♟';
                    break;
                case chess::KNIGHT:
                    pieceSymbol = u'♞';
                    break;
                case chess::BISHOP:
                    pieceSymbol = u'♝';
                    break;
                case chess::ROOK:
                    pieceSymbol = u'♜';
                    break;
                case chess::QUEEN:
                    pieceSymbol = u'♛';
                    break;
                case chess::KING:
                    pieceSymbol = u'♚';
                    break;
                case chess::EMPTY:
                    break;
                }
                gui::Piece* piece = new gui::Piece(pieceSymbol, board[i][j].getColor());
                piece->setZValue(1);
                piece->setPos(j*squareSize, i*squareSize);
                tile->piece_ = piece;
                scene->addItem(piece);
            }
        }
    }
}

void View::setupFromFen(const std::string& fen) {
    scene->clear();
    std::string header = fen.substr(0,36);
    std::string body = fen.substr(36, fen.size()-36);

    char to_Move = header[0];
    std::string kRights = header.substr(10, 7);
    std::string qRights = header.substr(18,7);

    switch (to_Move) {
    case 'R':
        toMove=chess::RED; break;
    case 'B':
        toMove=chess::BLUE; break;
    case 'Y':
        toMove=chess::YELLOW; break;
    case 'G':
        toMove=chess::GREEN; break;
    }


    char delimiter = '/';
    std::vector<std::string> rows;
    std::stringstream ss(body);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        rows.push_back(token);
    }

    //for (const auto& line : rows) std::cout<<line<<std::endl;

    for (int i=0; i<14; i++) {
        const std::string& row = rows[i];
        std::stringstream ss(row);
        int j=0;
        while (std::getline(ss, token, ',')) {
            if (token == "x") {
                board_[i][j] = nullptr;
                j++;
            }
            else{
                if (std::isdigit(token[0])) {
                    int num = std::stoi(token);
                    for (int k=0; k<num; k++) {
                        QColor color = ((i + j) % 2 == 0) ? Qt::white : Qt::lightGray;
                        gui::Tile* tile = new gui::Tile(color, this);
                        tile->setPos(j*squareSize, i*squareSize);   //NOTE THAT i AND j ARE EXCHANGED AS ROWS=Y COLS=X
                        tile->setZValue(0);
                        scene->addItem(tile);
                        board_[i][j] = tile;
                        j++;
                    }
                }
                else {
                    QColor color = ((i + j) % 2 == 0) ? Qt::white : Qt::lightGray;
                    gui::Tile* tile = new gui::Tile(color, this);
                    tile->setPos(j*squareSize, i*squareSize);   //NOTE THAT i AND j ARE EXCHANGED AS ROWS=Y COLS=X
                    tile->setZValue(0);
                    scene->addItem(tile);
                    board_[i][j] = tile;
                    chess::Color c;
                    switch (token[0]) {
                    case 'r': c=chess::RED; break;
                    case 'b': c=chess::BLUE; break;
                    case 'y': c=chess::YELLOW; break;
                    case 'g': c=chess::GREEN; break;
                    }
                    chess::Piece p;
                    switch (token[1]) {
                    case 'P': p=chess::PAWN; break;
                    case 'N': p=chess::KNIGHT; break;
                    case 'B': p=chess::BISHOP; break;
                    case 'R': p=chess::ROOK; break;
                    case 'Q': p=chess::QUEEN; break;
                    case 'K': p=chess::KING; break;
                    }
                    QChar pieceSymbol = piece_symbol[p];
                    gui::Piece* piece = new gui::Piece(pieceSymbol, c);
                    piece->setZValue(1);
                    piece->setPos(j*squareSize, i*squareSize);
                    tile->piece_ = piece;
                    scene->addItem(piece);
                    j++;
                }
            }
        }
    }
}

QPointF View::getSquareCenter(int row, int col){
    return QPointF(col*squareSize + squareSize/2, row*squareSize + squareSize/2);
}

Tile* View::getTile(QPointF pos){
    //row and column need to be reverted again
    int c = pos.x()/squareSize, r = pos.y()/squareSize;
    if(r<0||r>=boardSize||c<0||c>=boardSize) return nullptr;
    return board_[r][c];
}


void View::playMove(chess::Move move){

    movePieces(move);


    chess::Color toHighLight = toMove;

    toMove = toMove + 1;

    if(hl_tiles[toMove].first){
        if(hl_tiles[toMove].first->highlighted!=3)hl_tiles[toMove].first->reset();
        if(hl_tiles[toMove].second->highlighted!=3)hl_tiles[toMove].second->reset();
    }

    Tile* h1 = board_[move.from.row()][move.from.col()];
    Tile* h2 = board_[move.to.row()][move.to.col()];
    h1->highLight(hl_colors[toHighLight], 2);
    h2->highLight(hl_colors[toHighLight], 2);
    hl_tiles[toHighLight] = std::make_pair(h1, h2);
}

void View::movePieces(chess::Move move){
    int fr = move.from.row(), fc = move.from.col();
    int tr = move.to.row(), tc = move.to.col();
    if (!move.is_special()) {
        addAnimation(board_[fr][fc], board_[tr][tc]);
    }

    else {
        switch (move.get_move_type()) {
        case chess::PROMOTION:
        {
            addPromotionAnimation(board_[fr][fc], board_[tr][tc], move.promotion_type(), toMove);
            break;
        }
        case chess::EN_PASSANT:
        {
            addAnimation(board_[fr][fc], board_[tr][tc]);
            auto [dr, dc] = chess::kPawnDir[toMove];
            delete board_[fr+dr][fc+dc]->piece_;
            board_[fr+dr][fc+dc]->piece_ = nullptr;
            //board[fr+dr][fc+dc] = Square(true);
            break;
        }
        case chess::CASTLING:
        {
            addAnimation(board_[fr][fc], board_[tr][tc]);
            auto [kr, kc] = chess::kcastle_dir[toMove];
            if (tr==fr+2*kr && tc==fc+2*kc) {   //kingside castling
                addAnimation(board_[fr+3*kr][fc+3*kc], board_[(fr+tr)/2][(fc+tc)/2]);
                //board[(fr+tr)/2][(fc+tc)/2] = board[fr+3*kr][fc+3*kc];
                //board[fr+3*kr][fc+3*kc] = Square(true);
            }
            else {
                auto[qr, qc] = chess::qcastle_dir[toMove];
                addAnimation(board_[fr+4*qr][fc+4*qc], board_[(fr+tr)/2][(fc+tc)/2]);
                //board[(fr+tr)/2][(fc+tc)/2] = board[fr+4*qr][fc+4*qc];
                //board[fr+4*qr][fc+4*qc] = Square(true);
            }
            break;
        }
        case chess::EN_PASSANT_PROMOTION:
        {
            addPromotionAnimation(board_[fr][fc], board_[tr][tc], move.promotion_type(), toMove);
            //board[tr][tc] = Square(true, move.promotion_type(), to_move);
            //board[fr][fc] = Square(true);
            auto [dr, dc] = chess::kPawnDir[toMove];
            delete board_[fr+dr][fc+dc]->piece_;
            board_[fr+dr][fc+dc]->piece_ = nullptr;
            //board[fr+dr][fc+dc] = Square(true);
            break;
        }
        }
    }
}

void View::addAnimation(Tile* from, Tile* to){
    QPropertyAnimation *a = new QPropertyAnimation(from->piece_, "pos");
    a->setStartValue(from->piece_->pos());
    a->setEndValue(to->pos());
    a->setDuration(animation_time);
    a->start();

    Piece* t = to->piece_;
    to->piece_ = from->piece_;
    from->piece_ = nullptr;
    QTimer::singleShot(animation_time, this, [=](){
        if(t)delete t;
    });
}

void View::addPromotionAnimation(Tile* from, Tile* to, chess::Piece piece, chess::Color color){
    QPropertyAnimation *a = new QPropertyAnimation(from->piece_, "pos");
    a->setStartValue(from->piece_->pos());
    a->setEndValue(to->pos());
    a->setDuration(animation_time);
    a->start();
    Piece* t = to->piece_;
    to->piece_ = from->piece_;
    from->piece_ = nullptr;
    QTimer::singleShot(animation_time, this, [=]() {
        if(t) delete t;
        if(to->piece_)to->piece_->setSymbol(piece_symbol[piece]);
    });
}


}
