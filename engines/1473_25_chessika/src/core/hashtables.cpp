#include "constants.h"
#include "bits.h"
#include "position.h"
#include "hashtables.h"

#include <iostream>
#include <random>
#include <stdlib.h>
#include <time.h>

using namespace bits;

void Zobrist::Init() {

    std::default_random_engine generator;
    std::uniform_int_distribution<U64> distribution(1, 0xffffffffffffffff);

    // Pieces hash codes
    for (int i=0; i<12; i++) {
        for (int j=0; j<64; j++) {
            Z_KEYS_PIECES[i][j] = distribution(generator);
            //std::cout << Z_KEYS_PIECES[i][j] << std::endl;
        }
    }

    Z_KEYS_BLACK_TO_MOVE = rand();
    //std::cout << Z_KEYS_BLACK_TO_MOVE << std::endl;

    for (int i=0; i<4; i++) {
        Z_KEYS_CASTLING_RIGHTS[i] = distribution(generator);
        //std::cout << Z_KEYS_CASTLING_RIGHTS[i] << std::endl;
    }

    // En-Passant files
    for (int i=0; i<8; i++) {
        Z_KEYS_EN_PASSANT[i] = distribution(generator);
        //std::cout << Z_KEYS_EN_PASSANT[i] << std::endl;
    }
}


U64 Zobrist::GetHashCode(Position &p) {
   
    U64 key = 0;

    for (unsigned int c=0; c<=1; c++) {
        int offset = c*6;
        for(unsigned int id=PAWN_ID-1; id<KING_ID; id++) {
            U64 bb = p.m_pieces[id] & p.m_colors[c];

            while(bb) {
                U64 ls1b = LS1B(bb);
                int bitIdx = BitScanForward(ls1b);
                key ^= Z_KEYS_PIECES[offset+id][bitIdx];
                ResetLS1B(bb);
            }
        }
    }

    if (p.m_castleFlags & (1<<3))
        key ^= Z_KEYS_CASTLING_RIGHTS[3];
    if (p.m_castleFlags & (1<<2))
        key ^= Z_KEYS_CASTLING_RIGHTS[2];
    if (p.m_castleFlags & (1<<1))
        key ^= Z_KEYS_CASTLING_RIGHTS[1];
    if (p.m_castleFlags & 1)
        key ^= Z_KEYS_CASTLING_RIGHTS[0];

    if (p.m_enPassantSquare != NO_EP) {
        key ^= Z_KEYS_EN_PASSANT[p.m_enPassantSquare%8];
    }

    if (p.m_plyPlayer == COLOR_BLACK)
        key ^= Z_KEYS_BLACK_TO_MOVE;

    return key;
}

TTEntry::TTEntry() {
}

TTEntry::TTEntry(int value, unsigned int depth, NODE_TYPE nodeType) {
    m_value = value;
    m_depth = depth;
    m_nodeType = nodeType;
}
