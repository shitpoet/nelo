#include "eval.h"
#include "../core/bits.h"

#include <iostream>

using namespace bits;

float Eval::GetKnightOutpost(Position &p, Color c) {
	// Knights placed in the center (opponent side), defended by a pawn
	// and not vulnerable to opponent's pawn
	const U64 whiteOutpostCenter = (1UL<<35) | (1UL<<36);
	const U64 blackOutpostCenter = (1UL<<27) | (1UL<<28);
	const U64 defenders = (1UL<<2) | 1; // 101

	int res = 0;

	U64 srcBits = p.m_pieces[KNIGHT_ID] & p.m_colors[c];

	if (c == COLOR_WHITE) {
		srcBits &= whiteOutpostCenter;
		while(srcBits) {
			int sq = BitScanForward(LS1B(srcBits));
			U64 defBits = p.m_pieces[PAWN_ID] & p.m_colors[c] & (defenders << (sq-9));
			U64 attackBits = p.m_pieces[PAWN_ID] & p.m_colors[COLOR_BLACK] & (defenders << (sq+7));
			if (defBits && !attackBits) {
                if (PopCount(defBits) == 1)
                    res += 1;
                else
                    res += 1.5;
			}
        	ResetLS1B(srcBits);
		}
	}
	else {
		srcBits &= blackOutpostCenter;
		while(srcBits) {
			int sq = BitScanForward(LS1B(srcBits));
			U64 defBits = p.m_pieces[PAWN_ID] & p.m_colors[c] & (defenders << (sq+7));
			U64 attackBits = p.m_pieces[PAWN_ID] & p.m_colors[COLOR_BLACK] & (defenders << (sq-9));
			if (defBits && !attackBits) {
                if (PopCount(defBits) == 1)
                    res += 1;
                else
                    res += 1.5;
			}
        	ResetLS1B(srcBits);
		}
	}

	return res;
}
