#include "search.h"

/**
 * Static exchange evaluation (SEE : move ordering on non-capturing moves)
 * we only focus on the destination square
 * and simulate some plies by attacking the same square with the least valuable attacker)
 * if there is no attacker, score is 0
 * if there's one, we return the value of the current piece minus the SEE score one step deeper
 */

int Search::CaptureAndSee(Position &p, Move &m) {
    BoardFlags bFlags = BoardFlags(p);
#if DEBUG_MOVE_MAKING
    Position refTestPosition(p);
#endif

    p.Make(m);

    int score = 0;

    if (!p.IsIllegal()) {
        score = PIECE_VALUES[m.m_dstId] - See(p, m.m_dstSquare);
    }

    p.Unmake(m, bFlags);

#if DEBUG_MOVE_MAKING
    assert(Position::Identical(p, refTestPosition));
#endif

    return score;
}


int Search::See(Position &p, const Square& dstSq) {
    int srcSq = p.GetSmallestAttacker(dstSq);

    if (srcSq >= 0) {
        PieceId dstId = p.SquareId(dstSq);

        if (dstId == KING_ID)
            return 0;
        PieceId srcId = p.SquareId(srcSq);
        Color opp = Position::GetOpponent(p.m_plyPlayer);

        Move move(p.m_plyPlayer, opp, srcId, dstId, srcSq, dstSq);

        BoardFlags bFlags = BoardFlags(p);

#if DEBUG_MOVE_MAKING
        Position refTestPosition(p);
#endif

        int score = 0;
        p.Make(move);

        if (!p.IsIllegal()) {
            score = PIECE_VALUES[dstId] - See(p, dstSq);
        }

        p.Unmake(move, bFlags);

#if DEBUG_MOVE_MAKING
        assert(Position::Identical(p, refTestPosition));
#endif

        return score;

    }

    return 0;
}

