#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board.h"
#include "move.h"
#include "thread.h"

extern const int knight_moves_x[8];
extern const int knight_moves_y[8];
extern const int king_moves_x[8];
extern const int king_moves_y[8];

int moveGen(THREAD *thread, BOARD *pos, MOVE all_moves[256], int sort[256], int ply, int color);

#endif