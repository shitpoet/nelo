#ifndef _HASHTABLES_H_
#define _HASHTABLES_H_

#include "position.h"

enum NODE_TYPE { EXACT, UPPER_BOUND, LOWER_BOUND };

class Zobrist {
    public:
        static void Init();
        static U64 GetHashCode(Position&);
    private:
};


class TTEntry {
    friend class Search;

    public:
        TTEntry();
        TTEntry(int value, unsigned int depth, NODE_TYPE);

    private:
        NODE_TYPE m_nodeType;
        unsigned int m_depth = 0;
        int m_value = 0;
};

#endif
