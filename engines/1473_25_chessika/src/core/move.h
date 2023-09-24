#ifndef _ACTION_H
#define _ACTION_H

#include "constants.h"
#include "square.h"
#include "piece.h"

#include <string>
#include <vector>

class Move {
    friend class MoveGen;
    friend class Board;
    friend class Position;
    friend class Search;
    friend class Eval;
    friend class Game;
    friend class Uci;
    friend class Tests;
    friend class Web;

    public:

        Move();
        Move(Square src, Square dst);
        Move(Color srcSide, Color dstSide, PieceId srcId, PieceId dstId, Square src, Square dst);

        static bool Cmp(const Move &a, const Move &b);

        PieceId GetDstId() const;
        Square GetDstSquare() const;
        PieceId GetPawnPromotionId() const;
        Square GetSrcSquare() const;
        float GetValue() const;
        bool Equals(Move) const;
        bool IsCapture() const;
        bool IsCheck() const;
        bool IsCheckmate() const;
        bool IsIllegal() const;
        void PrintMoveTree(int depth, int maxDepth) const;
        void SetCheck(bool);
        void SetCheckmate(bool);
        void SetSrcSide(Color);
        void SetSrcId(PieceId);
        void SetValue(float val);
        std::string ToCoords() const;
        std::string ToHistory() const;
        std::string ToString() const;
        std::string Debug() const;
        Square m_srcSquare;
        Square m_dstSquare;
        bool operator==(const Move&);
        bool operator<(const Move&);
        
    private:
        Color m_srcSide;
        Color m_dstSide;
        PieceId m_srcId;
        PieceId m_dstId;
        int m_sortScore = 0;
        bool m_capture = false;
        bool m_illegal = false;
        bool m_check = false; 
        bool m_checkmate = false;
        bool m_doublePush = false;
        short m_castle = 0;
        PieceId m_pawnPromotionId = EMPTY_ID;
        bool m_enPassant = false;
        short m_depth = 0;
        float m_value;
};

#endif
