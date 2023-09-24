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

int Eval::OpenFileRooksCount(Position &p, Color c) {
    U64 srcBits = p.m_pieces[ROOK_ID] & p.m_colors[c];
    short pieceCount = 0;

    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        if ((p.m_pieces[PAWN_ID] & FILE_MASKS[sq]) == 0) {
            pieceCount++;
        }
        ResetLS1B(srcBits);
    }

    return pieceCount;
}

int Eval::HalfOpenFileRooksCount(Position &p, Color c) {
    U64 srcBits = p.m_pieces[ROOK_ID] & p.m_colors[c];
    short pieceCount = 0;

    Color opp = Position::GetOpponent(c);

    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        U64 filePawns = p.m_pieces[PAWN_ID] & FILE_MASKS[sq];
        if (((filePawns & p.m_colors[c]) == 0) && (filePawns & p.m_colors[opp])) {
            pieceCount++;
        }
        ResetLS1B(srcBits);
    }

    return pieceCount;
}
