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

int Eval::GetTropism(Position& p, Color c) {
    int res = 0;

    Color opp = c == COLOR_WHITE ? COLOR_BLACK : COLOR_WHITE;

    U64 oppKing = p.m_pieces[KING_ID] & p.m_colors[opp];

    U64 srcBits;

    // rooks (value / 2): sharing file or rank
    srcBits = p.m_pieces[ROOK_ID] & p.m_colors[c];
    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        if ((RANK_MASKS[sq] | FILE_MASKS[sq]) & oppKing) {
            //std::cout << "color " << c << "\n";
            //std::cout << p.ToString() << "\n\n";
            res += PIECE_VALUES[ROOK_ID] / 2; 
        }
        ResetLS1B(srcBits);
    }


    // bishop (value / 2): sharing diags
    srcBits = p.m_pieces[BISHOP_ID] & p.m_colors[c];
    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        if (DIAG_MASKS_EX[sq] & oppKing) {
            //std::cout << "color " << c << "\n";
            //std::cout << p.ToString() << "\n\n";
            res += PIECE_VALUES[BISHOP_ID] / 2; 
        }
        ResetLS1B(srcBits);
    }


    // queen (full value): sharing diags/file/rank
    srcBits = p.m_pieces[QUEEN_ID] & p.m_colors[c];
    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        if ((RANK_MASKS[sq] | FILE_MASKS[sq] | DIAG_MASKS_EX[sq]) & oppKing) {
            //std::cout << "color " << c << "\n";
            //std::cout << p.ToString() << "\n\n";
            res += PIECE_VALUES[QUEEN_ID]; 
        }
        ResetLS1B(srcBits);
    }

    return res;
}
