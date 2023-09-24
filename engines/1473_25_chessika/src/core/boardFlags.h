#ifndef BOARD_FLAGS_H
#define BOARD_FLAGS_H

#include "position.h"
#include "constants.h"
#include "square.h"

#include <string>
#include <vector>

class Position;

class BoardFlags {
    public:
        short m_castleFlags = 0;
        Color m_plyPlayer;
        Square m_enPassantSquare = NO_EP;
        int m_halfMoves = 0;

        BoardFlags(const Position&);
};

#endif
