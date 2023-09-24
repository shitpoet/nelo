#ifndef _STATS_H
#define _STATS_H

class Stats {
    friend class Search;

    static int BETA_CUTOFFS;
    const static int INSTANT_BETA_TAB_SIZE = 4;
    static int INSTANT_BETA_CUTOFFS[INSTANT_BETA_TAB_SIZE];
    static int MAX_SEARCH_REACHED;
    static int MAX_ITERATIVE_DEPTH_REACHED;
};

int Stats::BETA_CUTOFFS = 0;
int Stats::INSTANT_BETA_CUTOFFS[INSTANT_BETA_TAB_SIZE] = {0,0,0,0};
int Stats::MAX_SEARCH_REACHED = 0;
int Stats::MAX_ITERATIVE_DEPTH_REACHED = 0;


#endif
