#ifndef _SEARCH_H
#define _SEARCH_H

#include "../core/move.h"
#include "../core/position.h"

#include <map>
#include <vector>


const int MAX_PV_LENGTH = 40;

typedef struct pv_Line_t {
    unsigned int m_moveCount = 0;
    Move m_line[MAX_PV_LENGTH];
} PV_LINE;


class Search {

    public:
        static bool FOLLOW_PV;
        static int HISTORY[2][64][64];

        static int PVSearch(Position&, unsigned int depth, int alpha, int beta, 
                               int& nodes, PV_LINE &line, std::vector<Move>& prevPV, 
                               unsigned int currentDepth, bool nullMoveAllowed, bool lastMoveIsCapture);
        static int QSearch(Position&, int alpha, int beta, int currentDepth);
        static int CaptureAndSee(Position &, Move &); 
        static void IterativeSearch(Position& b, int& totalNodes, std::vector<Move> &resultPV);
        static int GetCapturable(Position &p, const Square& dstSq);
        static void MVV_LVA(std::vector<Move>&);
        static int See(Position &p, const Square& sq);
        static void SortMoves(Position&, std::vector<Move> &nextMoves, std::vector<Move> &previousPV, unsigned int currentDepth);
        static void Stop();
        static void TranslatePV(PV_LINE &pvLine, std::vector<Move> &resultPV);
};
    

#endif

