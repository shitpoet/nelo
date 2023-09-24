#include "search.h"


void Search::MVV_LVA(std::vector<Move> &moves) {
    // Empty ID has no value : no capture makes the sortScore to 0
    // Most valuable victims are simply the different pieces ids in ascending sort.
    const int MVV_SCORES[7] = { 0,  PIECE_VALUES[PAWN_ID] ,
        PIECE_VALUES[KNIGHT_ID],
        PIECE_VALUES[BISHOP_ID],
        PIECE_VALUES[ROOK_ID],
        PIECE_VALUES[QUEEN_ID], 0 };

    // Least valuable attacker is almost the invert.
    const int LVA_SCORES[7] = { 0,  PIECE_VALUES[KING_ID],
        PIECE_VALUES[QUEEN_ID], 
        PIECE_VALUES[ROOK_ID],
        PIECE_VALUES[BISHOP_ID],
        PIECE_VALUES[KNIGHT_ID],
        PIECE_VALUES[PAWN_ID] };

    int mvv = 0;
    for(auto& m : moves) {
        if (!m.m_capture) 
            continue;
        m.m_sortScore = MVV_SCORES[m.m_dstId];
        if (m.m_sortScore > mvv)
            mvv = m.m_sortScore;
    }

    for(auto& m : moves) {
        if (m.m_sortScore == mvv) {
            m.m_sortScore *= LVA_SCORES[m.m_srcId];
        }
    }
}

