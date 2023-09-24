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

int Eval::GetBrokenPawnShield(Position &p, Color c) {
    // identify if king is castled
    // if so, check if pawns are protecting him up to two ranks ahead
    // increase malus if open files are present next to king
    //
    // basic value of 2, +1 by open file
    U64 king = p.m_pieces[KING_ID] & p.m_colors[c];
    U64 rooks = p.m_pieces[ROOK_ID] & p.m_colors[c];
    U64 pawns = p.m_pieces[PAWN_ID] & p.m_colors[c];

    int res = 0;
    U64 srcBits;
    int kingSq = BitScanForward(LS1B(king));

    if (c == COLOR_WHITE && 
            (((king & (1UL<<6)) && (rooks & (1UL<<5))) ||
             ((king & (1UL<<2)) && (rooks & (1UL<<3))))) {
        
        srcBits = pawns & KING_MOVES[kingSq+8];
        res = 2*(PopCount(srcBits) > 0);
        if (res) {
            res += ((p.m_pieces[PAWN_ID] & (FILE_MASKS[kingSq+1])) == 0);
            res += ((p.m_pieces[PAWN_ID] & (FILE_MASKS[kingSq-1])) == 0);
        }
    }

    if (c == COLOR_BLACK && 
            (((king & (1UL<<62)) && (rooks & (1UL<<61))) ||
             ((king & (1UL<<58)) && (rooks & (1UL<<59))))) {
        
        srcBits = pawns & KING_MOVES[kingSq-8];
        res = 2*(PopCount(srcBits) > 0);
        if (res) {
            res += ((p.m_pieces[PAWN_ID] & (FILE_MASKS[kingSq+1])) == 0);
            res += ((p.m_pieces[PAWN_ID] & (FILE_MASKS[kingSq-1])) == 0);
        }
    }	

    //if (res > 0) {
    //    std::cout << "\n\n";
    //    std::cout << "broken shield color " << c << ", malus of " << res << "\n";
    //    std::cout << p.ToString();
    //}

    return res;
}


int Eval::GetPassedPawn(Position& p, Color c) {
    int res = 0;
    U64 srcBits = p.m_pieces[PAWN_ID] & p.m_colors[c];
    U64 oppPawns = p.m_pieces[PAWN_ID] & p.m_colors[Position::GetOpponent(c)];

    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));

        U64 fileMask = FILE_MASKS[sq];
        if (sq%8 > 0) {
            fileMask |= FILE_MASKS[sq-1];
        }
        if (sq%8 < 7) {
            fileMask |= FILE_MASKS[sq+1];
        }

        U64 rankMask = 0;
        if (c == COLOR_WHITE) {
            for(int i=7; i > sq/8; i--) {
                rankMask |= RANK_MASKS[i*8];
            }
            if ((oppPawns & fileMask & rankMask) == 0) {
                //std::cout << p.ToString();
                //std::cout << sq/8 << std::endl;
                res += sq/8 - 4;
            }
        }
        else {
            for(int i=0; i < sq/8; i++) {
                rankMask |= RANK_MASKS[i*8];
            }
            if ((oppPawns & fileMask & rankMask) == 0) {
                res += 7 - sq/8 + 4;
            }
        }

        ResetLS1B(srcBits);
    }

    return res;
}


int Eval::GetIsolatedPawns(Position &p, Color c) {
    int res = 0;
    U64 srcBits = p.m_pieces[PAWN_ID] & p.m_colors[c];
    U64 sameSidePawns = srcBits;

    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));

        U64 fileMask = 0;
        if (sq%8 > 0) {
            fileMask |= FILE_MASKS[sq-1];
        }
        if (sq%8 < 7) {
            fileMask |= FILE_MASKS[sq+1];
        }

        if ((sameSidePawns & fileMask) == 0) {
            if (c == COLOR_WHITE) {
                res += sq/8;
            }
            else {
                res += 7 - sq/8;
            }
        }

        ResetLS1B(srcBits);
    }

    return res;

}


int Eval::GetDoublePawnFileCount(Position &p, Color c) {
    int res = 0;

    U64 srcBits = p.m_pieces[PAWN_ID] & p.m_colors[c];

    short pawnCount[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    while(srcBits) {
        int sq = BitScanForward(LS1B(srcBits));
        pawnCount[sq%8]++;
        ResetLS1B(srcBits);
    }

    for (unsigned int i=0; i<8; i++) {
        if (pawnCount[i] > 1)
            res++;
    }
    return res;
}
