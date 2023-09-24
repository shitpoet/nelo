#include "eval.h"
#include "../core/bits.h"
#include "../core/moveGen.h"
#include "../core/position.h"

#include "math.h"
#include <map>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>
#include <tuple>
#include <vector>

using namespace bits;


/**
 * Returns the sum of each piece positional value
 */
int Eval::GetPositionalValue(Position &p, Color c, float materialValue) {
    int res = 0;

    U64 srcBits = 0;

    srcBits = p.m_pieces[PAWN_ID] & p.m_colors[c];
    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        res += PAWN_SQUARE_VALUES[c][sq];
        ResetLS1B(srcBits);
    }

    srcBits = p.m_pieces[KNIGHT_ID] & p.m_colors[c];
    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        res += KNIGHT_SQUARE_VALUES[c][sq];
        ResetLS1B(srcBits);
    }

    srcBits = p.m_pieces[BISHOP_ID] & p.m_colors[c];
    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        res += BISHOP_SQUARE_VALUES[c][sq];
        ResetLS1B(srcBits);
    }

    srcBits = p.m_pieces[ROOK_ID] & p.m_colors[c];
    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        res += ROOK_SQUARE_VALUES[c][sq];
        ResetLS1B(srcBits);
    }

    srcBits = p.m_pieces[QUEEN_ID] & p.m_colors[c];
    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        res += QUEEN_SQUARE_VALUES[c][sq];
        ResetLS1B(srcBits);
    }

    srcBits = p.m_pieces[KING_ID] & p.m_colors[c];
    int sq = BitScanForward(LS1B(srcBits));

    if (materialValue < END_GAME_MATERIAL_VALUE)
        res += KING_ENDGAME_SQUARE_VALUES[c][sq];
    else
        res += KING_MIDGAME_SQUARE_VALUES[c][sq];

    return res;
}

int Eval::GetPositionalValue(Square sq, Color c, PieceId id) {
    switch(id) {
        case PAWN_ID:
            return PAWN_SQUARE_VALUES[c][sq];
        case KNIGHT_ID:
            return KNIGHT_SQUARE_VALUES[c][sq];
        case BISHOP_ID:
            return BISHOP_SQUARE_VALUES[c][sq];
        case ROOK_ID:
            return ROOK_SQUARE_VALUES[c][sq];
        case QUEEN_ID:
            return QUEEN_SQUARE_VALUES[c][sq];
        case KING_ID:
            return KING_MIDGAME_SQUARE_VALUES[c][sq];
        default:
            return 0;
    }
}
