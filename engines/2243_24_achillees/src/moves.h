#ifndef SRC_MOVES_H_
#define SRC_MOVES_H_

#include "board.h"

#define MAX_MOVES 218

enum {NORT, NOEA, EAST, SOEA, SOUT, SOWE, WEST, NOWE};
enum {QUIET, CAPTURE};

typedef struct {
	int from;
	int to;

	int piece;
	int color;

	int castle;
	int promotion;
	int enPassant;

	int score;
	int type;
} Move;

typedef struct {
	int castling;
	int enPassant;
	int capture;
	int fiftyMoves;
} History;

extern const uint64_t kingLookup[64];

uint64_t perft(Board *board, int depth);

int legalMoves(Board *board, Move *moves);

int kingAttacked(const Board *board, const int color);

static inline int inCheck(const Board *board) {
	return kingAttacked(board, board->turn);
}

int isLegalMove(Board *board, const Move *move);
int givesCheck(const Board *board, const Move *move);
int getSmallestAttacker(Board *board, const int sqr, const int color);

static inline int compareMoves(const Move *moveA, const Move *moveB) {
	return moveA->from == moveB->from && moveA->to == moveB->to && moveA->promotion == moveB->promotion;
}

void moveToText(Move move, char *text);
Move textToMove(const Board *board, char *text);

void printMoves(Move *moves, int n);
void printMove (Move move, int nodes);

#endif /* SRC_MOVES_H_ */
