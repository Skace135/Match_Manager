#ifndef GUI_H
#define GUI_H

#include <QGraphicsRectItem>
#include <QObject>
#include <qbrush.h>
#include "board.h"

namespace gui{

constexpr int squareSize = 40;
constexpr int animation_duration = 0;
extern int animation_time;
const QChar piece_symbol[6] = {u'♚', u'♛', u'♜', u'♝', u'♞', u'♟'};
const QColor check_color = QColor(130, 0, 0);
const QColor piece_colors[4] = {
    Qt::red,
    QColor(0, 0, 205),
    QColor(255, 165, 0),
    QColor(0, 128, 0)
};
const QColor hl_colors[4] = {
    QColor(255,204,204),
    QColor(204,229,255),
    QColor(255,255,204),
    QColor(204,255,204)
};

class View;


class Piece : public QGraphicsObject {
    Q_OBJECT
public:
    Piece(QChar piece, chess::Color color, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;
    void setSymbol(QChar piece);
    chess::Color color_;
    QGraphicsSimpleTextItem* moveCount;
    QChar piece_;

private:
};

class Tile : public QGraphicsObject{
    Q_OBJECT
public:
    Tile(QColor color, View* view, QGraphicsItem* parent=nullptr);
    QRectF boundingRect() const override;
    void highLight(const QColor &color, int hl_type);
    void reset();   //resets highlight only
    Piece* piece_;
    chess::BoardLocation getLocation();
    QColor hl_;
    int highlighted = 0;    //0 = no highlight, 1 = hover highlight, 2 = actual highlight, 3 = check



protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    QColor color_;
    View* m_view;
};

}

#endif // GUI_H
