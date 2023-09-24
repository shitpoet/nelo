#include "search.h"
#include "stats.h"
#include "../core/constants.h"
#include "../eval/eval.h"
#include "../core/clock.h"
#include "../core/hashtables.h"
#include "../core/moveGen.h"
#include "../core/position.h"
#include "../core/settings.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <utility>
#include <bits/stdc++.h>

#define DEBUG 0
#define DEBUG_TIME 0
#define DEBUG_STATS 0
#define DEBUG_ALPHABETA 0
#define DEBUG_MOVE_MAKING 0
#define DEBUG_MOVE_ORDERING 0

#define QSEARCH 1
#define NULL_WINDOW_PV_SEARCH 1
#define QSEARCH_DELTA_PRUNING 0
#define LATE_MOVE_REDUCTION 1

bool Search::FOLLOW_PV = true;


std::vector<Move> KILLER_MOVES[MAX_PV_LENGTH];
const int MAX_KILLER_MOVES_BY_DEPTH = 3;

int Search::HISTORY[2][64][64];

/**
 * Triangular PV table result is reversed.
 * This function reverses the PV_LINE object into a vector<Move>
 */
void Search::TranslatePV(PV_LINE &pvLine, std::vector<Move> &resultPV) {
    resultPV.clear();
    for(unsigned int i=0; i<pvLine.m_moveCount; i++) {
        resultPV.push_back(pvLine.m_line[i]);
    }
}


/**
 * Iterative Deepening entry point
 */
void Search::IterativeSearch(Position& b, int& totalNodes, std::vector<Move> &resultPV) {

    Clock::REF_TIME = Clock::Now();

    int totalCutoffs = 0;
    int totalInstantCutoffs[Stats::INSTANT_BETA_TAB_SIZE] = { 0, 0, 0, 0 };
    Stats::MAX_SEARCH_REACHED = 0;


    int depth = 1;
    int alpha, beta, score;

    std::vector<Move> bestMoves;

    while (depth <= Settings::MAX_DEPTH) {
#if DEBUG
        std::cout << "info string start depth " << depth << "-------------- " << std::endl;
#endif

#if DEBUG_STATS
        Stats::BETA_CUTOFFS = 0;
        for (unsigned int i=0; i<Stats::INSTANT_BETA_TAB_SIZE; i++)
            Stats::INSTANT_BETA_CUTOFFS[i] = 0;
#endif

        // aspiration window 
        const int aspirationOffset = PIECE_VALUES[PAWN_ID] / 2;
        if (depth >= 4) {
            alpha = score - aspirationOffset;
            beta = score + aspirationOffset;
#if DEBUG
            //std::cout << "info string aspiration window [" << alpha << "," << beta << "]" << std::endl;
#endif
        }
        else {
            alpha = -INF;
            beta = INF;
        }

        Search::FOLLOW_PV = true;
        timePoint_t depthRefTimePoint = Clock::Now();
        int nodes = 0;

        std::vector<Move> previousIterationPV(resultPV);
        Position tmpBoard(b);
        PV_LINE iterationPV;

        score = PVSearch(tmpBoard, depth, alpha, beta, nodes, iterationPV, previousIterationPV, 0, true, false);

        if (Clock::TIME_OUT) {
            std::cout << "info string search timeout at depth " << depth << std::endl;
            break;
        } 
        if (iterationPV.m_moveCount == 0) {
            std::cout << "info string no legal move found" << std::endl;
            break;
        } 

        totalNodes += nodes;
        TranslatePV(iterationPV, resultPV);
        resultPV[0].m_value = score;


        float depthElapsedTimeSecs = Clock::ElapsedTimeSecs(depthRefTimePoint);

        // Logging pv infos
        if (Settings::SHOW_MOVE_CALCULATION) {
            std::cout << "info depth " << depth
                << " score cp " << (int)score 
                << " nodes " << nodes
                << " seldepth " << Stats::MAX_SEARCH_REACHED
                << " nps " << (int)(nodes/depthElapsedTimeSecs)
                << " time " << (int)(depthElapsedTimeSecs * 1000000)
                << " pv ";
            for(const auto &move : resultPV) {
                std::cout << move.ToCoords() << " ";
            }
            std::cout << std::endl;
        }

        // storing the max depth among iterations for time management (best move N repetition)
        if (depth > Stats::MAX_ITERATIVE_DEPTH_REACHED)
            Stats::MAX_ITERATIVE_DEPTH_REACHED = depth;

        bestMoves.push_back(resultPV[0]);

#if DEBUG_STATS
        if (Stats::BETA_CUTOFFS > 0) {
            std::cout << "info string cutoffs " << Stats::BETA_CUTOFFS;
            std::cout << " [";
            for(unsigned int i=0; i<Stats::INSTANT_BETA_TAB_SIZE; i++) {
                totalInstantCutoffs[i] += Stats::INSTANT_BETA_CUTOFFS[i];
                std::cout << std::setprecision(3) << " " << ((float)Stats::INSTANT_BETA_CUTOFFS[i] / Stats::BETA_CUTOFFS );
            }
            std::cout << " ]" << std::endl;
            totalCutoffs += Stats::BETA_CUTOFFS;
        }
#endif

        // Aspiration window is very narrowed, no need to continue ?
        //if (resultPV.size() < depth) {
        //    break;
        //}


        if (std::abs(score) >= VALUE_MATE) {
            resultPV[0].m_checkmate = true;
            break;
        }

        // best move hasn't change since last 3 iterations, we interrupt the search	
        //if (Stats::MAX_ITERATIVE_DEPTH_REACHED > 3 && depth > (Stats::MAX_ITERATIVE_DEPTH_REACHED/2)) {
        //    int bestMoveRepetition = 1;
        //    for(int i=bestMoves.size()-2; i>0 && bestMoveRepetition < 3; i--) {
        //        //std::cout << bestMoves[i].ToCoords() << " " << resultPV[0].ToCoords() << std::endl;
        //        if (bestMoves[i] == resultPV[0])
        //            bestMoveRepetition++;
        //        else {
        //            break;
        //        }
        //    }

        //    if (bestMoveRepetition >= 3) {
#if DEBUG
        //        std::cout << "info string BEST MOVE REPETITION " << bestMoveRepetition << " (Stats::MAX_ITERATIVE_DEPTH_REACHED " << Stats::MAX_ITERATIVE_DEPTH_REACHED << ")" << std::endl;
#endif
        //        break;
        //    }
        //}

        // Do we have enough time to calculate next iteration ?
        // This factor is completly empirical.

        const float DEPTH_EVAL_TIME_MULTIPLIER = 7.0;
        float totalTimeSecs = Clock::ElapsedTimeSecs(Clock::REF_TIME);
#if DEBUG_TIME
		std::cout << "Depth search time " << totalTimeSecs * 1000 << "ms" << std::endl;
		std::cout << "Remaining time : " << (Clock::GIVEN_TIME_SECS - totalTimeSecs) * 1000 << "ms"<< std::endl;
#endif
        if ((depthElapsedTimeSecs * DEPTH_EVAL_TIME_MULTIPLIER) > (Clock::GIVEN_TIME_SECS - totalTimeSecs)) {
#if DEBUG_TIME
            std::cout << "info string INTERRUPT at " << totalTimeSecs << ", remaining time " << (Clock::GIVEN_TIME_SECS - totalTimeSecs) << " seconds" << std::endl;
#endif
            break;
        }
        depth++;
    }

#if DEBUG_STATS
    if (totalCutoffs > 0) {
        std::cout << "info string total cutoffs " << totalCutoffs;
        std::cout << " [";
        for(unsigned int i=0; i<Stats::INSTANT_BETA_TAB_SIZE; i++) {
            std::cout << std::setprecision(3) << " " << ((float)totalInstantCutoffs[i] / totalCutoffs );
        }
        std::cout << " ]" << std::endl;
    }
    else {
        std::cout << "info string no cutoffs" << std::endl;
    }
#endif

    // We keep this to tell the engine that even depth 1 hasn't be calculated in time
    if (Clock::TIME_OUT && depth > 1)
        Clock::TIME_OUT = false;

}


int Search::PVSearch(Position& p, unsigned int depth, 
        int alpha, int beta, int& nodes, 
        PV_LINE& pvLine, std::vector<Move>& previousPV, unsigned int currentDepth, 
        bool nullMoveAllowed, bool lastMoveIsCapture) {

#if DEBUG_ALPHABETA
    std::cout << "IN : [" << currentDepth << "] PVSearch alpha:" << alpha << ", beta:" << beta << std::endl;
#endif

    assert(alpha < beta);

    if (Clock::IsTimeOut()) {
        return -INF;
    }

    if (currentDepth > (unsigned int)Stats::MAX_SEARCH_REACHED) 
        Stats::MAX_SEARCH_REACHED = currentDepth;

    pvLine.m_moveCount = 0;

    // Draw game ?
    if (currentDepth > 0) {
        if (p.IsThreeFold() || p.m_halfMoves > 100) {
            return 0;
        }
    }

    // Check extension
    bool check = p.IsPlayerUnderCheck(p.m_plyPlayer);
    if (check) {
        depth++;
    }


    if (depth == 0) {
        nodes++;
#if QSEARCH
        if (lastMoveIsCapture) {
            int v = QSearch(p, alpha, beta, currentDepth);
#if DEBUG_ALPHABETA
            std::cout << "qsearch depth " << currentDepth << " v " << v << std::endl;
#endif
            return v;
        }
#endif

        return Eval::GetValue(p);
    }

    BoardFlags bFlags = BoardFlags(p);

    bool pvNode = beta - alpha > 1;

    // Null-move pruning
    if (!pvNode) {
        if (nullMoveAllowed && !p.IsPlayerUnderCheck(p.m_plyPlayer) && depth > 3) {
            const int R = 2;
            PV_LINE nullDepthLine;

            p.MakeNullMove();
            float nullScore = -PVSearch(p, depth-1 - R, -beta, -beta+1, nodes, nullDepthLine, previousPV, currentDepth+2, false, lastMoveIsCapture);
            p.UnmakeNullMove(bFlags);

            if (nullScore >= beta) {
                return nullScore;
            }
        }
    }

    // Generate pseudo legal moves and sort them
    std::vector<Move> nextMoves;
    p.GetPseudoLegalMoves(p.m_plyPlayer, nextMoves);
    SortMoves(p, nextMoves, previousPV, currentDepth);

#if DEBUG_MOVE_ORDERING
    // Displaying move ordering result
    if (Settings::SHOW_MOVE_ORDERING) {
        if (pvNode && currentDepth < previousPV.size()) {
            std::cout << "[" << currentDepth << "]\n"; 
            int j = 0;
            for (auto m : nextMoves) {
                if (j>10)
                    break;
                //std::cout << "  " << m.ToCoords() << ":" << m.m_sortScore;
                std::cout << "  " << m.ToString() << ":" << m.m_sortScore;
                j++;
            }
            std::cout << " [...]" << std::endl;
            //std::cout << std::endl;
        }
    }
#endif


    PV_LINE depthLine;

    int bestScore = -INF;
    int playedMove = -1;

    for (auto& move : nextMoves) {
        if (Clock::IsTimeOut()) {
            pvLine.m_moveCount = 0;
            return 0;
        }



#if DEBUG_MOVE_MAKING
        Position refTestPosition(p);
#endif

        p.Make(move);

        if (p.IsIllegal()) {
            move.m_illegal = true;
            p.Unmake(move, bFlags);
#if DEBUG_MOVE_MAKING
            assert(Position::Identical(p, refTestPosition));
#endif
            continue;
        }

        playedMove++;

#if LATE_MOVE_REDUCTION
        int lmr = 0;
        // Reducing search depth on "late moves" (move index > 4)
        if (!pvNode && !check && currentDepth > 3  && !move.m_capture && move.m_pawnPromotionId == EMPTY_ID) {
            if (playedMove > 9) {
                //std::cout << "LMR " << currentDepth / 3 << ", curDepth " << currentDepth << ", depth " << depth << ", redux at " << playedMove << "/" << nextMoves.size() << "\n";
                lmr = depth / 2;
            }
            else {
                //std::cout << "LMR 1, curDepth " << currentDepth << ", redux at " << playedMove << "/" << nextMoves.size() << "\n";
                lmr = 1;
            }
        }
#endif

        int score = 0;

#if DEBUG_ALPHABETA
        std::cout << "Make > " << move.ToString() << " (score:" << move.m_sortScore << ", a:" << alpha << ", b:" << beta << ")" << std::endl;
#endif
        if (playedMove == 0) {
            score = -PVSearch(p, depth-1, -beta, -alpha, nodes, depthLine, previousPV, currentDepth+1, false, move.m_capture);
        }
        else {
#if NULL_WINDOW_PV_SEARCH
            score = -PVSearch(p, depth-1, -alpha -1, -alpha, nodes, depthLine, previousPV, currentDepth+1, nullMoveAllowed, move.m_capture);
#endif
#if NULL_WINDOW_PV_SEARCH || LATE_MOVE_REDUCTION
            // Full depth search if necessary, caution : code is valid for both null window search and late move reduction 
            if (alpha < score && score < beta)
                score = -PVSearch(p, depth-1-lmr, -beta, -alpha, nodes, depthLine, previousPV, currentDepth+1, nullMoveAllowed, move.m_capture);
#endif 
#if !(NULL_WINDOW_PV_SEARCH && LATE_MOVE_REDUCTION)
            score = -PVSearch(p, depth-1-lmr, -beta, -alpha, nodes, depthLine, previousPV, currentDepth+1, nullMoveAllowed, move.m_capture);
#endif
        }


#if DEBUG_ALPHABETA
        std::cout << "OUT (" << currentDepth << ") " << move.ToString() << " score:" << score << ", beta:" << beta << ", best:" << bestScore << std::endl;
#endif

        p.Unmake(move, bFlags);

#if DEBUG_MOVE_MAKING
        assert(Position::Identical(p, refTestPosition));
#endif

        if (score >= beta) {
#if DEBUG_STATS
            if (playedMove < Stats::INSTANT_BETA_TAB_SIZE) {
                //std::cout << "instant cutoff : " << score << std::endl;
                Stats::INSTANT_BETA_CUTOFFS[playedMove]++;
            }

            Stats::BETA_CUTOFFS++;
#endif

            // Storing move for Killer Heuristic
            if (!move.m_capture) {
                if (KILLER_MOVES[currentDepth].size() < MAX_KILLER_MOVES_BY_DEPTH) {
                    auto killerMoveIt = std::find (KILLER_MOVES[currentDepth].begin(), KILLER_MOVES[currentDepth].end(), move);
                    if (killerMoveIt == KILLER_MOVES[currentDepth].end()) {
                        KILLER_MOVES[currentDepth].push_back(move);
                    }
                }

                HISTORY[move.m_srcSide][move.m_srcSquare][move.m_dstSquare] += depth * depth;
            }

            // When PVSearch has been initialized with narrowed alpha/beta window
            // if beta cutoff occurs at depth 0, we need to store the pv line as well
            // or it will end by just a return of beta value, with no pv 
            // ending by the err msg : no legal move found
            if (playedMove == 0) {
                pvLine.m_line[0] = move;

                for(unsigned int j=0; j<depthLine.m_moveCount; j++) {
                    pvLine.m_line[1+j] = depthLine.m_line[j];
                }
                pvLine.m_moveCount = depthLine.m_moveCount + 1;
            }

            return beta;
        }

        if (score > bestScore) {
            bestScore = score;

            pvLine.m_line[0] = move;

            for(unsigned int j=0; j<depthLine.m_moveCount; j++) {
                pvLine.m_line[1+j] = depthLine.m_line[j];
            }
            pvLine.m_moveCount = depthLine.m_moveCount + 1;

            if (score > alpha) {
                alpha = score;
#if DEBUG_ALPHABETA
                std::cout << "      " << move.ToString() << " raised alpha : " << alpha << std::endl;
#endif
            }
        }
    }

    if (bestScore == -INF) {
        pvLine.m_moveCount = 0;
        if (p.IsPlayerUnderCheck(p.m_plyPlayer)) {
            return -(VALUE_MATE+depth);
        }
        else {
            // Stalemate
            return 0;
        }
    }

    return bestScore;
}


void Search::Stop() {
    std::cout << "info string STOP invoked" << std::endl;
    Clock::TIME_OUT = true;
}


/***
 * By default, all scores are set to 0
 *
 * For now, the only sort we have are : 
 * Previous pv first
 * SEE on captures 
 * Killer heuristic
 *
 */
void Search::SortMoves(Position &/*p*/, std::vector<Move>& moves, std::vector<Move>& previousPV, unsigned int currentDepth) {

    MVV_LVA(moves);
    //std::vector<Move> oppMoves;
    //p.GetPseudoLegalMoves(Position::GetOpponent(p.m_plyPlayer), oppMoves);

    for(auto &m: moves) {
        if (!m.m_capture) {
            // History heuristic
            m.m_sortScore += HISTORY[m.m_srcSide][m.m_srcSquare][m.m_dstSquare];

            // Guard Heuristic
            //int gv = Eval::GetGuardValue(p, m.m_dstSquare, moves, oppMoves, p.m_plyPlayer);
            //std::cout << m.ToString() << ", gv:" <<gv << std::endl;
            //m.m_sortScore += gv;

            // PST Heuristic
            //m.m_sortScore += (Eval::GetPositionalValue(m.m_dstSquare, m.m_srcSide, m.m_srcId) - Eval::GetPositionalValue(m.m_srcSquare, m.m_srcSide, m.m_srcId));
        }

        if (m.m_pawnPromotionId != EMPTY_ID) 
            m.m_sortScore += PIECE_VALUES[m.m_pawnPromotionId];
    }

    // Killer Heuristic 
    if (currentDepth > 2) { 
        for(auto &m : KILLER_MOVES[currentDepth-2]) {
            auto it = std::find(moves.begin(), moves.end(), m);
            if (it != moves.end()) {
                it->m_sortScore += 5000;
            }
        }
    }


    // Previous pv first
    if (Search::FOLLOW_PV && previousPV.size() > 0 && currentDepth < previousPV.size()) {
        Search::FOLLOW_PV = false;
        Move &m = previousPV[currentDepth];
        auto it = std::find(moves.begin(), moves.end(), m);
        if (it != moves.end()) {
            it->m_sortScore = 100000000;
            Search::FOLLOW_PV = true;
        }
    }

    std::sort(moves.begin(), moves.end());

    //for (auto it = moves.begin(); it->m_sortScore >= 0 && it != moves.end(); it++) {         
    //    // Searching the first element greater than *it from beginning 
    //    auto const insertIdx = std::upper_bound(moves.begin(), it, *it, Move::Cmp); 
    //      
    //    // Shifting the unsorted part 
    //    std::rotate(insertIdx, it, it+1);         
    //}
}


int Search::QSearch(Position &p, int alpha, int beta, int currentDepth) {
    int standPat = Eval::GetValue(p);

    if (currentDepth > Stats::MAX_SEARCH_REACHED) {
        Stats::MAX_SEARCH_REACHED = currentDepth;
    }

    if (standPat >= beta) {
        return beta;
    }

#if QSEARCH_DELTA_PRUNING
    const int delta = PIECE_VALUES[QUEEN_ID];
    //std::cout << "standPat:" << standPat << " " << alpha - delta << std::endl;
    if (standPat < alpha - delta) {
        //std::cout << "delta pruning\n";
        return alpha;
    }
#endif

    if (alpha < standPat) {
        alpha = standPat;
    }

    std::vector<Move> nextMoves;
    p.GetPseudoLegalMoves(p.m_plyPlayer, nextMoves);

#if DEBUG_MOVE_MAKING
    Position refTestPosition(p);
#endif

    BoardFlags bFlags = BoardFlags(p);
    int playedMove = -1;
    for(auto& move : nextMoves) {

        if (!move.m_capture)
            continue;

        if(CaptureAndSee(p, move) < 0)
            continue;


        p.Make(move);

        if (p.IsIllegal()) {
            move.m_illegal = true;
            p.Unmake(move, bFlags);
#if DEBUG_MOVE_MAKING
            assert(Position::Identical(p, refTestPosition));
#endif
            continue;
        }

        playedMove++;


        int score = -QSearch(p, -beta, -alpha, currentDepth+1);
        p.Unmake(move, bFlags);

        if (score >= beta) {
#if DEBUG_STATS
            if (playedMove < Stats::INSTANT_BETA_TAB_SIZE) {
                Stats::INSTANT_BETA_CUTOFFS[playedMove]++;
            }
            Stats::BETA_CUTOFFS++;
#endif
            return beta;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    return alpha;
}
