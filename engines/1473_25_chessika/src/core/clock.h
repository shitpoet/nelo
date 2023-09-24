#ifndef _CLOCK_H
#define _CLOCK_H

#include "constants.h"

#include <chrono>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> timePoint_t;

class Clock {
    friend class Search;

    public:
        static long int GIVEN_TIME_MICROSECS;
        static float GIVEN_TIME_SECS;
        static long int MAX_DURATION_SECS;
        static bool TIME_OUT;
        static timePoint_t REF_TIME;

        static int MOVES_TO_GO;
        static int WHITE_TIME;
        static int BLACK_TIME;
        static int WHITE_INC_TIME;
        static int BLACK_INC_TIME;

        static void InitSearchTime(Color plyPlayer, int plyCount);
        static bool IsTimeOut();
        static float ElapsedTimeSecs(timePoint_t);
        static timePoint_t Now();

};

#endif
