#include "constants.h"
#include "moveGen.h"

#include <cstdlib>
#include "assert.h"

void MoveGen::AddPromotionMoves(Color plyPlayer, Color dstColor, PieceId dstId, 
                                const Square& srcSquare, const Square& targetSquare, 
                                std::vector<Move>& moves) {

    moves.emplace_back(plyPlayer, dstColor,
                        PAWN_ID, dstId, srcSquare, targetSquare);
    moves.back().m_pawnPromotionId = QUEEN_ID;
    
    moves.emplace_back(plyPlayer, dstColor,
                        PAWN_ID, dstId, srcSquare, targetSquare);
    moves.back().m_pawnPromotionId = ROOK_ID;

    moves.emplace_back(plyPlayer, dstColor,
                        PAWN_ID, dstId, srcSquare, targetSquare);
    moves.back().m_pawnPromotionId = KNIGHT_ID;

    moves.emplace_back(plyPlayer, dstColor,
                        PAWN_ID, dstId, srcSquare, targetSquare);
    moves.back().m_pawnPromotionId = BISHOP_ID;
}

U64 MoveGen::FirstRankAttacks(U64 occ, int file) {

	int res = 0;
	for(int i=file+1; i<8; i++) {
		int targetSquare = 1 << i;
		res |= targetSquare;
		if (occ & targetSquare) {
			//std::cout << "bit " << i << " is occupied" << std::endl;
			break;
		}
	}

	for(int i=file-1; i>=0; i--) {
		int targetSquare = 1 << i;
		res |= targetSquare;
		if (occ & targetSquare) {
			//std::cout << "bit " << i << " is occupied" << std::endl;
			break;
		}
	}

	// Checking result
	// std::cout << MoveGen::Debug((int)res) << std::endl;
	return res;
}

void MoveGen::InitFillUpAttacks() {
	for(int file=0; file<8; file++) {
		for(int occ=0; occ<256; occ++) {
            U64 buff = FirstRankAttacks(occ, file);
            // FIXME : optimize this by mult by 8 with bit arithmetic
            U64 result = buff;
            for (int i=0; i<7; i++)
                result |= (result << 8) | buff;
            FILL_UP_ATTACKS[file][occ] = result;
		}
	}
}

U64 MoveGen::DiagAttacks(U64 occupancy, Square sq) {
    // sq&7 == sq%8 == file(sq)

    U64 diagMask = DIAG_MASKS_EX[sq];
	U64 diagOcc = diagMask & occupancy;
	int firstRankMask = ( diagOcc * A_FILE ) >> 56;
    // Developing process visually
    /*
    std::cout << "diag mask : " << std::endl << MoveGen::Debug(diagMask) << std::endl;
    std::cout << "diag occupancy : " << std::endl << MoveGen::Debug(diagOcc) << std::endl;
    std::cout << "first rank mask : " << std::endl << MoveGen::Debug((int)firstRankMask) << std::endl;
    std::cout << "fill up rank mask :" << std::endl;
    std::cout << MoveGen::Debug((FILL_UP_ATTACKS[sq&7][firstRankMask])) << std::endl;
    U64 res = (FILL_UP_ATTACKS[sq&7][firstRankMask]) & diagMask;
    std::cout << "result ?" << std::endl << MoveGen::Debug(res);
    */
    return (FILL_UP_ATTACKS[sq&7][firstRankMask]) & diagMask;
}

U64 MoveGen::AntiDiagAttacks(U64 occupancy, Square sq) {
    U64 antiDiagMask = ANTI_DIAG_MASKS_EX[sq];
	U64 diagOcc = antiDiagMask & occupancy;
	int firstRankMask = ( diagOcc * A_FILE ) >> 56;
    return (FILL_UP_ATTACKS[sq&7][firstRankMask]) & antiDiagMask;
}

U64 MoveGen::RankAttacks(U64 occupancy, Square sq) {
    U64 rankMask = RANK_MASKS_EX[sq];
	U64 rankOcc = rankMask & occupancy;
	int firstRankMask = ( rankOcc * A_FILE ) >> 56;
    return (FILL_UP_ATTACKS[sq&7][firstRankMask]) & rankMask;
}

U64 MoveGen::FileAttacks(U64 occupancy, Square sq) {
    //std::cout << Debug(occupancy) << std::endl;
    //std::cout << "sq : " << sq << std::endl;
    int file = sq&7;
    //std::cout << "file : " << file << std::endl;

	U64 fileOcc = A_FILE & (occupancy >> file);
    //std::cout << "A-File shifted file occ :" << std::endl;
    //std::cout << MoveGen::Debug(fileOcc);

    const U64 antidiag = ANTI_DIAG_MASKS[e4];
    //std::cout << "anti diag :" << std::endl;
    //std::cout << MoveGen::Debug(antidiag) << std::endl;

	U64 firstRankMask = ( fileOcc * antidiag )>> 56;
    //std::cout << "firstRankMask : "<< std::endl;
    //std::cout << MoveGen::Debug(firstRankMask) << std::endl;

    U64 attackSet = FILL_UP_ATTACKS[sq/8][firstRankMask] & 255;
    //std::cout << "north-filled attackset : " << std::endl;
    //std::cout << MoveGen::Debug(attackSet) << std::endl;

    // Sigh -_- 
    // Shift back : rewrite this asap
    // Parallel writing ? LMAO
    U64 res = ( attackSet & 1UL   )        |
              ((attackSet & 1UL<<1) << 7)  | 
              ((attackSet & 1UL<<2) << 14) | 
              ((attackSet & 1UL<<3) << 21) | // 5+2x8
              ((attackSet & 1UL<<4) << 28) | // 4+3x8
              ((attackSet & 1UL<<5) << 35) | // 3+4x8
              ((attackSet & 1UL<<6) << 42) | // 2+5x8
              ((attackSet & 1UL<<7) << 49);  // 1+6x8
              

    //std::cout << "A-File shift back : " << std::endl;
    //std::cout << Position::Debug(res) << std::endl;
    //std::cout << "rank shift back by : " << file << std::endl;
    res = res << file;
    //std::cout << "Res : " << std::endl;
    //std::cout << Position::Debug(res) << std::endl;
    return res;
}

U64 MoveGen::RookAttacks(U64 occ, Square sq) {
    return FileAttacks(occ, sq) | RankAttacks(occ, sq);
}

U64 MoveGen::BishopAttacks(U64 occ,Square sq) {
    return DiagAttacks(occ, sq) | AntiDiagAttacks(occ, sq);
}

U64 MoveGen::QueenAttacks(U64 occ, Square sq) {
    return FileAttacks(occ, sq) | RankAttacks(occ, sq) | 
           DiagAttacks(occ, sq) | AntiDiagAttacks(occ, sq);
}

U64 MoveGen::KnightAttacks(U64, Square sq) {
    return KNIGHT_MOVES[sq];
}

U64 MoveGen::KingAttacks(U64, Square sq) {
    return KING_MOVES[sq];
}
