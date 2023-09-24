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

int Eval::GetValue(Position &p) {
	if (InsufficientMaterial(p)) {
		return 0;
	}

    int res = 0;
    bool isEndGame = false;

    // Eval Tuning
    const float material = 1;
    const float position = 1;
    const float mobility = 0.6;
    const float doublePawnFileMalus = -80;
    const float isolatedPawnMalus = -1;
    const float passedPawnBonus = 20;
    const float bishopPairBonus = 30;
    const float knightPairBonus = 10;
    const float openFileRooksBonus = 60;
    const float halfOpenFileRooksBonus = 30;
	const float knightOutpostBonus = 11;
    // TC 40/1+0.01s
    // 11 : 887 - 858 - 224      5.12 +/- 14.44
    // TC 40/10+0.1s
    // 11 : 1322 - 1255 - 423    7.76 +/- 11.51
    
    //const float kingSafety = 7;
    //const float tropismBonus = 0.7;
    const float brokenPawnShieldMalus = -52000; // value is scaled by opponent's material value

    int p1material = GetBoardMaterialValue(p, COLOR_WHITE);
    int p2material = GetBoardMaterialValue(p, COLOR_BLACK);
    res += (p1material - p2material) * material;

    res += GetPositionalValue(p, COLOR_WHITE, p1material) * position;
    res -= GetPositionalValue(p, COLOR_BLACK, p2material) * position;


    res += (HasPair(p, BISHOP_ID, COLOR_WHITE) - HasPair(p, BISHOP_ID, COLOR_BLACK)) * bishopPairBonus;
    res += (HasPair(p, KNIGHT_ID, COLOR_WHITE) - HasPair(p, KNIGHT_ID, COLOR_BLACK)) * knightPairBonus;

    res += (OpenFileRooksCount(p, COLOR_WHITE) - OpenFileRooksCount(p, COLOR_BLACK)) * openFileRooksBonus;
    res += (HalfOpenFileRooksCount(p, COLOR_WHITE) - HalfOpenFileRooksCount(p, COLOR_BLACK)) * halfOpenFileRooksBonus;

    res += (GetDoublePawnFileCount(p, COLOR_WHITE) - GetDoublePawnFileCount(p, COLOR_BLACK)) * doublePawnFileMalus;
    res += (GetIsolatedPawns(p, COLOR_WHITE) - GetIsolatedPawns(p, COLOR_BLACK)) * isolatedPawnMalus;

    if (IsEndGame(p))
        res += (GetPassedPawn(p, COLOR_WHITE) - GetPassedPawn(p, COLOR_BLACK)) * passedPawnBonus;
    else
        res += 0.5*(GetPassedPawn(p, COLOR_WHITE) - GetPassedPawn(p, COLOR_BLACK)) * passedPawnBonus;

    res += (GetMobility(p, COLOR_WHITE) - GetMobility(p, COLOR_BLACK)) * mobility;

	res += (GetKnightOutpost(p, COLOR_WHITE) - GetKnightOutpost(p, COLOR_BLACK)) * knightOutpostBonus;

    //res += (HasKingSafety(p, COLOR_WHITE) - HasKingSafety(p, COLOR_BLACK)) * kingSafety;

    // Trying pawn shield...
    //float whiteBrokenPawnShield = (GetBrokenPawnShield(p, COLOR_WHITE) * brokenPawnShieldMalus / p2material); 
    //float blackBrokenPawnShield = (GetBrokenPawnShield(p, COLOR_BLACK) * brokenPawnShieldMalus / p1material); 
    //int pawnShield = (whiteBrokenPawnShield - blackBrokenPawnShield);
    //res += pawnShield;
    /* if (pawnShield != 0) */ 
    /*     std::cout << "pawnShield malus : " << pawnShield << " (" << whiteBrokenPawnShield << "/" << blackBrokenPawnShield << ") " << "mat:" << p1material << "/" << p2material << std::endl; */ 

    //// Trying tropism...
    //int tropism = (GetTropism(p, COLOR_WHITE) - GetTropism(p, COLOR_BLACK)) * tropismBonus;
    //if (tropism != 0)
    //	std::cout << tropism << std::endl;
    //res += tropism;

    int playerSide = p.m_plyPlayer == 1 ? 1 : - 1;
    return playerSide*res;
}

/**
 * Returns the sum of all present pieces values 
 */
int Eval::GetBoardMaterialValue(Position &p, Color c) {
    int res = 0;
    for(unsigned int id=PAWN_ID; id<KING_ID; id++) {
        res += PopCount(p.m_pieces[id] & p.m_colors[c]) * PIECE_VALUES[id];
    }
    return res;
}


int Eval::HasPair(Position &p, PieceId id, Color c) {
    return PopCount(p.m_pieces[id] & p.m_colors[c]) > 1;
}


int Eval::HasKingSafety(Position &p, Color c) {
    U64 kingBits = p.m_pieces[KING_ID] & p.m_colors[c];
    int kingSq = BitScanForward(LS1B(kingBits));

    U64 srcBits = p.m_colors[c] & KING_MOVES[kingSq];
    return PopCount(srcBits);
}

bool Eval::IsEndGame(Position &p) {
    // Side has <= 3 non pawns pieces (including king)
    //int pop = 0;
    Color c = p.m_plyPlayer;

    U64 srcBits = p.m_colors[c] & p.m_pieces[KING_ID] & p.m_pieces[QUEEN_ID] & p.m_pieces[ROOK_ID] & p.m_pieces[BISHOP_ID] & p.m_pieces[KNIGHT_ID];
    return PopCount(srcBits) <= 3;
}

