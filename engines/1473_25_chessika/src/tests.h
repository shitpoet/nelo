#ifndef _TESTS_H
#define _TESTS_H

#include "core/game.h"
#include "core/position.h"

class Tests {
    public:
        static void Perft(Position&, int depth, int maxDepth, int& nodes);
        static int SplitPerft(Position&, int maxDepth, bool displayMoves);
        static void TestZobristHashing();
        static void Test3FoldScore();
		static void EvalMaterial();
        static void RunPerft(Game &, int maxDepth);
};

#endif
