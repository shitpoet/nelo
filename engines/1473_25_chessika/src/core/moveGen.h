#ifndef MOVE_CALCULATOR_H
#define MOVE_CALCULATOR_H

#include "square.h"
#include "position.h"

#include <map>
#include <vector>

class MoveGen {
    public:
        static void InitFillUpAttacks();

        static U64 FirstRankAttacks(U64 occupancy, int file);
        static U64 DiagAttacks(U64 occupancy, Square sq);
        static U64 AntiDiagAttacks(U64 occupancy, Square sq);
        static U64 RankAttacks(U64 occupancy, Square sq);
        static U64 FileAttacks(U64 occupancy, Square sq);
        static U64 BishopAttacks(U64 occ, Square sq);
        static U64 QueenAttacks(U64 occ, Square sq);
        static U64 RookAttacks(U64 occ, Square sq);
        static U64 KnightAttacks(U64, Square sq);
        static U64 KingAttacks(U64, Square sq);

        static void AddPromotionMoves(Color srcColor, Color dstColor, PieceId dstId, const Square& srcSq, const Square& targetSq, std::vector<Move>&);
};

#endif
