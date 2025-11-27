#include "gui.h"
#include "view.h"
#include <qapplication.h>
#include <qfont.h>
#include <QPainter>
#include <QPropertyAnimation>
#include <QGraphicsSceneMouseEvent>
#include <algorithm>


namespace gui{

int animation_time = animation_duration;

Tile::Tile(QColor color, View* view, QGraphicsItem* parent)
    : color_(color), piece_(nullptr), m_view(view) {
    //setAcceptHoverEvents(true);
}

QRectF Tile::boundingRect() const{
    return QRectF(0,0,squareSize, squareSize);
}

void Tile::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget){
    painter->setBrush(color_);
    painter->setPen(Qt::black);
    painter->drawRect(0,0, squareSize, squareSize);
    //painter->drawRect(rect());

    if(highlighted){
        painter->setBrush(hl_);
        //painter->drawRect(rect());
        painter->drawRect(0,0,squareSize, squareSize);
    }
}

void Tile::highLight(const QColor &color, int hl_type){
    hl_ = color;
    highlighted = hl_type;
    update();
}

void Tile::reset(){
    highlighted = false;
    update();
}

chess::BoardLocation Tile::getLocation(){
    return chess::BoardLocation(pos().y() / squareSize, pos().x() / squareSize);
}



Piece::Piece(QChar piece, chess::Color color, QGraphicsItem* parent)
    : QGraphicsObject(parent), piece_(piece), color_(color){
    setZValue(1);
    //setCursor(Qt::OpenHandCursor);
    moveCount = new QGraphicsSimpleTextItem(this);
    QFont font = moveCount->font();
    font.setPointSize(20);  // Set to your desired size
    moveCount->setFont(font);
    moveCount->setBrush(Qt::green);
    moveCount->setZValue(4);
}



QRectF Piece::boundingRect() const{
    return QRectF(0, 0, squareSize, squareSize);
};

void Piece::setSymbol(QChar piece) {
    piece_ = piece;
    update();
}

void Piece::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*){
    int fontSize = squareSize / 1.65;
    int borderSize = fontSize * 1.16;
    QFont font = painter->font();
    //QFont font = QFont("Arial", borderSize);
    font.setPointSize(borderSize);
    painter->setFont(font);
    painter->setPen(Qt::black);

    painter->drawText(boundingRect(), Qt::AlignCenter, piece_);

    painter->setPen(piece_colors[color_]);
    font.setPointSize(fontSize);
    painter->setFont(font);

    painter->drawText(boundingRect(), Qt::AlignCenter, piece_);
}


}
