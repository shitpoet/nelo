#include "clock.h"
#include <iostream>

#define DEBUG_TIME 0

bool Clock::TIME_OUT = false;
timePoint_t Clock::REF_TIME;
float Clock::GIVEN_TIME_SECS = 0;
long int Clock::MAX_DURATION_SECS = 500;

int Clock::MOVES_TO_GO = 0;
int Clock::WHITE_TIME = 0;
int Clock::BLACK_TIME = 0;
int Clock::WHITE_INC_TIME = 0;
int Clock::BLACK_INC_TIME = 0;


float Clock::ElapsedTimeSecs(timePoint_t refTime) {
    return (float)((Now() - refTime).count()) / std::chrono::high_resolution_clock::period::den;
}


/**
 * Checks if GIVEN_TIME_SECS has been passed
 */
bool Clock::IsTimeOut() {
    if (TIME_OUT)
        return true;

    if (ElapsedTimeSecs(REF_TIME) > GIVEN_TIME_SECS) {
        TIME_OUT = true;
    }
    return TIME_OUT;
}


/**
 * Returns a high_resolution_clock time point
 */
timePoint_t Clock::Now() {
    return std::chrono::high_resolution_clock::now();
}


void Clock::InitSearchTime(Color plyPlayer, int plyCount) {

    // Caution : UCI times are in milliseconds
    int playerTimeMs = 0;

    if (plyPlayer == COLOR_WHITE) {
        playerTimeMs = WHITE_TIME + WHITE_INC_TIME;
    }
    else {
        playerTimeMs = BLACK_TIME + BLACK_INC_TIME;
    }

    int remainingTimeMicroSecs = playerTimeMs * 1000;

    int givenEvalTimeMicroSecs = 0;

    int currentTurn = (plyCount/2 + 1);
    int movesToGo = MOVES_TO_GO/2;

#if DEBUG_TIME
    std::cout << "info string current turn : " << currentTurn << std::endl;
    std::cout << "info string remainingTime : " << (remainingTimeMicroSecs) << std::endl;
#endif

    if (movesToGo == 0) {
        movesToGo = (40 - currentTurn)/2;
        if (movesToGo <= 0)
            movesToGo = 1;
#if DEBUG_TIME
        std::cout << "info string [forced] moves to go/2 : " << movesToGo << std::endl;
#endif
    }
#if DEBUG_TIME
    else
        std::cout << "info string moves to go/2 : " << movesToGo << std::endl;
#endif

    bool analysisMode = (WHITE_TIME == 0 && BLACK_TIME == 0);
    if (!analysisMode)
        givenEvalTimeMicroSecs = (remainingTimeMicroSecs / ((double)movesToGo));
    else
        givenEvalTimeMicroSecs = MAX_DURATION_SECS * 1000 * 1000;

    Clock::GIVEN_TIME_SECS = givenEvalTimeMicroSecs / (float)MICRO_SECS_TO_SECS;
#if DEBUG_TIME
    std::cout << "info string given eval time (s) : " << Clock::GIVEN_TIME_SECS << std::endl;
#endif
}
