#ifndef __CORE_H__
#define __CORE_H__

#include "constants.h"
#include "square.h"

namespace bits {

    inline U64 LS1B(U64 x) {
        return x & -x;
    }

    inline Square BitScanForward(U64 x) {
        assert(x > 0);
        return __builtin_ctzll(x);
    }

    inline int PopCount(U64 x) {
        return __builtin_popcountll(x);
    }


    inline void ResetLS1B(U64& x) {
         x &= x-1; 
    }
}

#endif
