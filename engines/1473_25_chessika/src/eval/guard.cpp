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
 * Returns the UNSIGNED sum of each capturable piece
 * We naturally ignore the king value
 * bestCapture is an out parameter
 */
int Eval::GetGuardValue(Position &p, Square sq, const std::vector<Move>& moves, const std::vector<Move>& oppMoves, Color playerColor) {

    const int CAPTURE_STRENGTH[8] = { 0, 9, 6, 5, 2, 1, 1 };

    int guardValue = 0;
    bool isAttacked = false;
    bool isDefended = false;

    for (const auto& m : moves) {
        if (m.m_dstSquare == sq && (m.m_srcId != PAWN_ID || (m.m_srcId == PAWN_ID && m.m_capture))) {
            guardValue += CAPTURE_STRENGTH[m.m_srcId];
            isAttacked = true;
        }
    }
    for (const auto& m : oppMoves) {
        if (m.m_dstSquare == sq && (m.m_srcId != PAWN_ID || (m.m_srcId == PAWN_ID && m.m_capture))) {
            guardValue -= CAPTURE_STRENGTH[m.m_srcId];
            isDefended = true;
        }
    }

    if (isAttacked && isDefended) {
        PieceId id = p.SquareId(sq);
        Color c = p.SquareColor(sq);

        if (c != playerColor)
            guardValue += CAPTURE_STRENGTH[id];
    }

    return guardValue;
}
