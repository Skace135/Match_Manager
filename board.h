//
// Created by Leon on 01.04.2025.
//

#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

namespace chess {

constexpr uint8_t MUL_ATTACKERS = 197;
constexpr uint8_t INVALID = 196;
constexpr int pieceTypes = 6;

enum Piece {
    KING = 0, QUEEN = 1, ROOK = 2, BISHOP = 3, KNIGHT = 4, PAWN = 5, EMPTY = 6
};
enum Color {
    RED = 0, BLUE = 1, YELLOW = 2, GREEN = 3
};
inline Color operator+(Color c, int x)
{
    return static_cast<Color>((static_cast<int> (c) + x )% 4);
}
inline Color operator-(Color c, int x)
{
    int t = static_cast<int> (c) - x;
    if (t<0) t+=4;
    return static_cast<Color>(t);
}

const std::array<std::array<int, 2>, 8> kKnightMoves = {{
    {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
    {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
}};
const std::array<std::array<int, 2>, 4> kBishopDir = {{
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
}};
const std::array<std::array<int, 2>, 4> kRookDir = {{
    {1, 0}, {0, -1}, {-1, 0}, {0, 1}
}};
const std::array<std::array<int, 2>, 4> kPawnDir = {{
    {-1, 0}, {0, 1}, {1, 0}, {0, -1}
}};
const std::array<std::array<std::pair<int, int>, 2>, 4> kPawnAttackDir = {{
                                                                           {{{-1, -1}, {-1, 1}}},
                                                                           {{{-1, 1}, {1, 1}}},
                                                                           {{{1, 1}, {1, -1}}},
                                                                           {{{1, -1}, {-1, -1}}},
                                                                           }};



// NOT CONTAINED IN ORIGINAL BOARD.H FILE

const std::array<std::array<int, 2>, 4> kcastle_dir = {{
                                                        {0, 1}, {1, 0}, {0, -1}, {-1, 0}
}};

const std::array<std::array<int, 2>, 4> qcastle_dir = {{
    {0, -1}, {-1, 0}, {0, 1}, {1, 0}
}};

//CONTINUING AS USUAL

enum MoveType {
    PROMOTION = 0,
    EN_PASSANT = 1,
    CASTLING = 2,
    EN_PASSANT_PROMOTION = 3
};

class Square {
public:
    Square() : Square(false, EMPTY, RED) {}
    explicit Square(bool valid): Square(valid, EMPTY, RED) {}
    Square(bool valid, Piece piece, Color color) {
        bits_ = (((uint8_t) valid) << 7)
        | (((uint8_t) piece) << 4)
            | (((uint8_t) color) << 2);
    }
    Square(bool valid, Piece piece, Color color, bool kingSide) {
        bits_ = (((uint8_t) valid) << 7)
        | (((uint8_t) piece) << 4)
            | (((uint8_t) color) << 2)
            | (((uint8_t) kingSide) << 1);
    }
    bool valid() const {
        return bits_ & (1 << 7);
    }
    Piece getPiece() const {
        return static_cast<Piece>((bits_ & 0b01110000) >> 4);
    }
    bool empty() const    //also contains valid check because invalid fields are set to empty by default
    {
        return static_cast<Piece>((bits_ & 0b01110000) >> 4) == EMPTY;
    }
    bool isBishopType() const {return getPiece() == BISHOP || getPiece() == QUEEN;}
    bool isRookType() const {return getPiece() == ROOK || getPiece() == QUEEN;}
    bool getPieceFlag() const
    {
        return (bits_ & 0b0000010) >> 1;
    }
    Color getColor() const {
        return static_cast<Color>((bits_ & 0b00001100) >> 2);
    }
    Color getTeam() const
    {
        return static_cast<Color>(((bits_ & 0b00001100) >> 2) % 2);
    }

    bool operator==(const Square &other) const { return bits_ == other.bits_; }
    bool operator!=(const Square &other) const { return bits_ != other.bits_; }

private:
    //bit 1: rook kingside bit
    //bit 2-3: color
    //bit 4-6: piece
    //bit 7: valid square
    uint8_t bits_;
};

//NOT CONTAINED IN ORIGINAL FILE

inline Square standardBoard[14][14] = {
    {Square(),Square(),Square(),Square(true,ROOK,YELLOW,true),Square(true,KNIGHT,YELLOW),Square(true,BISHOP,YELLOW),Square(true,KING,YELLOW),Square(true,QUEEN,YELLOW),Square(true,BISHOP,YELLOW),Square(true,KNIGHT,YELLOW),Square(true,ROOK,YELLOW,false),Square(),Square(),Square()},
    {Square(),Square(),Square(),Square(true,PAWN,YELLOW),Square(true,PAWN,YELLOW),Square(true,PAWN,YELLOW),Square(true,PAWN,YELLOW),Square(true,PAWN,YELLOW),Square(true,PAWN,YELLOW),Square(true,PAWN,YELLOW),Square(true,PAWN,YELLOW),Square(),Square(),Square()},
    {Square(),Square(),Square(),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(),Square(),Square()},
    {Square(true,ROOK,BLUE,false),Square(true,PAWN,BLUE),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true,PAWN,GREEN),Square(true,ROOK,GREEN,true)},
    {Square(true,KNIGHT,BLUE),Square(true,PAWN,BLUE),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true,PAWN,GREEN),Square(true,KNIGHT,GREEN)},
    {Square(true,BISHOP,BLUE),Square(true,PAWN,BLUE),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true,PAWN,GREEN),Square(true,BISHOP,GREEN)},
    {Square(true,QUEEN,BLUE),Square(true,PAWN,BLUE),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true,PAWN,GREEN),Square(true,KING,GREEN)},
    {Square(true,KING,BLUE),Square(true,PAWN,BLUE),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true,PAWN,GREEN),Square(true,QUEEN,GREEN)},
    {Square(true,BISHOP,BLUE),Square(true,PAWN,BLUE),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true,PAWN,GREEN),Square(true,BISHOP,GREEN)},
    {Square(true,KNIGHT,BLUE),Square(true,PAWN,BLUE),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true,PAWN,GREEN),Square(true,KNIGHT,GREEN)},
    {Square(true,ROOK,BLUE,true),Square(true,PAWN,BLUE),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true,PAWN,GREEN),Square(true,ROOK,GREEN,false)},
    {Square(),Square(),Square(),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(true),Square(),Square(),Square()},
    {Square(),Square(),Square(),Square(true,PAWN,RED),Square(true,PAWN,RED),Square(true,PAWN,RED),Square(true,PAWN,RED),Square(true,PAWN,RED),Square(true,PAWN,RED),Square(true,PAWN,RED),Square(true,PAWN,RED),Square(),Square(),Square()},
    {Square(),Square(),Square(),Square(true,ROOK,RED,false),Square(true,KNIGHT,RED),Square(true,BISHOP,RED),Square(true,QUEEN,RED),Square(true,KING,RED),Square(true,BISHOP,RED),Square(true,KNIGHT,RED),Square(true,ROOK,RED,true),Square(),Square(),Square()}
};

class BoardLocation
{
public:
    BoardLocation(): loc_(196) {}
    BoardLocation(uint8_t loc): loc_(loc) {}
    BoardLocation(int row, int col)
    {
        loc_ = (row<0 ||row>13 || col<0||col>13)? 196 : 14 * row + col;
    }

    bool present() const { return loc_ < 196; }
    bool invalid() const { return loc_ == 196; }
    int row() const { return loc_ / 14; }
    int col() const { return loc_ % 14; }
    bool multipleAttackers() const { return loc_ == 197; }
    void setLoc(const uint8_t loc) { loc_ = loc; }
    void setLoc(const int row, const int col) { loc_ = row * 14 + col; }

    bool operator==(const BoardLocation& other) const { return loc_ == other.loc_; }
    bool operator!=(const BoardLocation& other) const { return loc_ != other.loc_; }

private:
    //0-195: valid locations
    //196: invalid
    //197: multiple attackers
    uint8_t loc_;
};

class Move
{
public:
    Move(): from(BoardLocation()), to(BoardLocation()), data(0){}
    Move(BoardLocation from_, BoardLocation to_): from(from_), to(to_), data(0) {}
    Move(BoardLocation from_, BoardLocation to_, MoveType m): from(from_), to(to_) {
        data = (((uint8_t) m)<<1) + 1;
    }
    //Note that the resulting piece will be p + QUEEN
    Move(BoardLocation f, BoardLocation t, MoveType m, Piece p): from(f), to(t) {
        data = ((((uint8_t) m) << 1)
                | (((uint8_t) p) << 3)) + 1;
    }
    Move(int fr, int fc, int tr, int tc, uint8_t data_): from(BoardLocation(fr, fc)), to(BoardLocation(tr, tc)), data(data_) {}
    Move(BoardLocation f, BoardLocation t, bool earlyCheck, bool lateCheck): from(f), to(t) {
        data = ((((uint8_t) earlyCheck)<<5)
                | ((uint8_t) lateCheck<<6));
    }
    BoardLocation from;
    BoardLocation to;
    //bit 0: special move flag:
    //bit 1-2: promotion(0), en passant(1), castling(2), en passant promotion(3)
    //bit 3-4: promotion piece type (from QUEEN-1 to KNIGHT-1)
    //bit 5: delivers early king check (=check to king of to_move+3)
    //bit 6: delivers late king check (= check to king of to_move+1)

    bool is_special() const { return data & 1; }
    Piece promotion_type() const { return static_cast<Piece>(((data >> 3) & 3) + QUEEN); }
    MoveType get_move_type() const {return static_cast<MoveType> ((data>>1) & 3);}
    bool givesLateCheck() const{return (data>>6) & 1; }
    bool givesEarlyCheck() const{return (data>>5) & 1; }
    bool operator==(const Move& m) const { return from==m.from && to==m.to && data==m.data; }
    bool operator!=(const Move& m) const { return from!=m.from || to!=m.to || data != m.data; }
    uint8_t data;

protected:
};

} // namespace chess

#endif //BOARD_H
