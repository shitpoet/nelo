#include "move.h" 

#include <sstream>
#include <iostream>

// Used for invalid moves
Move::Move() : m_illegal(true) {
}

Move::Move(Square srcSquare, Square dstSquare) {
    m_srcSquare = srcSquare;
    m_dstSquare = dstSquare;
}


Move::Move(Color srcSide, Color dstSide, PieceId srcId, PieceId dstId, Square srcSquare, Square dstSquare) : 
    m_srcSide(srcSide),
    m_dstSide(dstSide),
    m_srcId(srcId), 
    m_dstId(dstId),
    m_srcSquare(srcSquare),
    m_dstSquare(dstSquare),
    m_illegal(false),
    m_check(false),
    m_checkmate(false)
    {
    m_castle = 0;
    m_value = 0;
    m_capture = (m_dstId != EMPTY_ID && (srcSide != dstSide)) || m_enPassant;
}


void Move::PrintMoveTree(int depth, int maxDepth) const {
    if (m_illegal || depth > maxDepth) {
        return;
    }

    for (int i=0; i<depth; i++)
        std::cout << "    ";
    std::cout << "[" << m_depth << "] " << ToCoords() << " " << m_value << std::endl;
}

std::string Move::ToCoords() const {
    std::ostringstream s;
    s << SquareTool::ToString(m_srcSquare);
    s << SquareTool::ToString(m_dstSquare);
    if (m_pawnPromotionId != EMPTY_ID) {
        s << Piece::GetPieceName(m_pawnPromotionId, m_srcSide);
    }
    return s.str();
}


std::string Move::ToString() const {
    std::ostringstream s;
    // king side mask : 1010 (10)
    if (m_castle & 10) {
        s << "0-0";
    }
    // queen side mask : 101 (5)
    else if (m_castle & 5) {
        s << "0-0-0";
    }
    else {
        char pieceName = Piece::GetPieceName(m_srcId, m_srcSide)[0];
        if (m_srcId != PAWN_ID)
            s << pieceName;
        

        if (m_capture) {
            if (m_srcId == PAWN_ID)
                s << SquareTool::ToString(m_srcSquare)[0];
            s << 'x';
        }


        s << SquareTool::ToString(m_dstSquare);

        if (m_enPassant) 
            s << " e.p.";

        if (m_pawnPromotionId > 0)
            s << "=" << Piece::GetPieceName(m_pawnPromotionId, m_srcSide);

        if (m_checkmate)
            s << '#';
        else {
            if (m_check)
                s << '+';
        }
    }

    return s.str();
}


std::string Move::ToHistory() const {
    std::ostringstream s;
    s << SquareTool::ToString(m_srcSquare);
    s << SquareTool::ToString(m_dstSquare);
    return s.str();
}


void Move::SetValue(float val) {
    m_value = val;
}


void Move::SetSrcSide(Color c) {
    m_srcSide = c;
}

void Move::SetSrcId(PieceId id) {
    m_srcId = id;
}

PieceId Move::GetDstId() const {
    return m_dstId;
}

Square Move::GetDstSquare() const {
    return m_dstSquare;
}

PieceId Move::GetPawnPromotionId() const {
    return m_pawnPromotionId;
}

Square Move::GetSrcSquare() const {
    return m_srcSquare;
}

float Move::GetValue() const {
    return m_value;
}

bool Move::IsIllegal() const {
    return m_illegal;
}

bool Move::IsCapture() const {
    return m_capture;
}

bool Move::IsCheck() const {
    return m_check;
}

bool Move::IsCheckmate() const {
    return m_checkmate;
}

void Move::SetCheck(bool b) {
    m_check = b;
}

void Move::SetCheckmate(bool b) {
    m_checkmate = b;
}

std::string Move::Debug() const {
    std::ostringstream s;
    s << SquareTool::ToString(m_srcSquare);
    s << SquareTool::ToString(m_dstSquare);
    s << " ";
    s << "srcId:" << m_srcId << ", dstId:" << m_dstId << ", ";
    s << "srcSide:" << m_srcSide << ", dstSide:" << m_dstSide << ", ";
    s << "promo:" << m_pawnPromotionId << std::endl;
    return s.str();
}

bool Move::Equals(Move o) const {
    return m_srcSquare == o.m_srcSquare && m_dstSquare == o.m_dstSquare;
}

bool Move::operator==(const Move& b) { 
    return (m_srcSquare == b.m_srcSquare) && 
           (m_dstSquare == b.m_dstSquare) && 
           (m_dstId == b.m_dstId) && 
           (m_dstSide == b.m_dstSide) &&
           (m_srcId == b.m_srcId) && 
           (m_srcSide == b.m_srcSide);
}

bool Move::operator<(const Move& b) { 
    return m_sortScore > b.m_sortScore;
}

bool Move::Cmp(const Move &a, const Move &b) {
    return a.m_sortScore > b.m_sortScore;
}
