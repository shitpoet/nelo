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

/***
 * Returning 1 point by available square, we don't count same side squares.
 *
 * We ignore pawns, their mobility are quite constant, 
 * and far less important than their position, 
 *
 * Same for the king (prefer king safety eval)
 ***/
int Eval::GetMobility(Position &p, Color c) {
    int res = 0;
    U64 srcBits;
    //Color opp = Position::GetOpponent(c);

    // Knights 
    srcBits = p.m_pieces[KNIGHT_ID] & p.m_colors[c];
    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        res += PopCount(MoveGen::KnightAttacks(0, sq) & (~p.m_colors[c])); 
        ResetLS1B(srcBits);
    }

    // Bishops
    srcBits = p.m_pieces[BISHOP_ID] & p.m_colors[c];
    while (srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        res += PopCount(MoveGen::BishopAttacks(p.m_occupancy, sq) & (~p.m_colors[c])); 
        ResetLS1B(srcBits);
    }

    // Rooks
    srcBits = p.m_pieces[ROOK_ID] & p.m_colors[c];
    while (srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        res += PopCount(MoveGen::RookAttacks(p.m_occupancy, sq) & (~p.m_colors[c])); 
        ResetLS1B(srcBits);
    }

    // QueenAttacks
    srcBits = p.m_pieces[QUEEN_ID] & p.m_colors[c];
    while (srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        res += PopCount(MoveGen::QueenAttacks(p.m_occupancy, sq) & (~p.m_colors[c])); 
        ResetLS1B(srcBits);
    }

    return res;
}
