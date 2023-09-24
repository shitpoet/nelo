#ifndef __UCI_H__
#define __UCI_H__

#include "../core/game.h"

class Uci {
    public:
        static int Listen(Game&);
        static int InsertHistoryMoves(Game&, std::string input);
};

#endif
