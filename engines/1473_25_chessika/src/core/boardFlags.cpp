#include "boardFlags.h"

BoardFlags::BoardFlags(const Position& p) {
    m_castleFlags = p.m_castleFlags;
    m_enPassantSquare = p.m_enPassantSquare;
    m_plyPlayer = p.m_plyPlayer;
    m_halfMoves = p.m_halfMoves;
}


