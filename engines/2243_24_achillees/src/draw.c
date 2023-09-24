#include "board.h"
#include "draw.h"


static int threeFoldRepetition(const Board *board);
static int insufficientMaterial(const Board *board);

Memory memory = {.size = 0};

int isDraw(const Board *board) {
	if (threeFoldRepetition(board)) {
		//printf("3fold repetition\n");
		return 1;
	}

	if (insufficientMaterial(board)) {
		//printf("Insufficient material\n");
		return 1;
	}

	if (board->fiftyMoves >= 100) {
		//printf("50 moves draw\n");
		return 1;
	}

	return 0;
}

/*
 * A draw by insufficient material is declared on:
 *  - King vs King
 *  - King and Knight vs King
 *  - King and Bishop vs King
 *  - King and Bishop vs King and Bishop (of the same color)
 */
static int insufficientMaterial(const Board *board) {
	switch (popCount(board->players[WHITE])) {
	case 1:
		switch (popCount(board->players[BLACK])) {
		case 1:
			return 1;
		case 2:
			if (board->pieces[BLACK][KNIGHT] || board->pieces[BLACK][BISHOP])
				return 1;
			return 0;
		}
		break;
	case 2:
		switch (popCount(board->players[BLACK])) {
		case 1:
			if (board->pieces[WHITE][KNIGHT] || board->pieces[WHITE][BISHOP])
				return 1;
			break;
		case 2:
			if 	(board->pieces[WHITE][BISHOP] &&
				 board->pieces[BLACK][BISHOP] &&
				 sameParity(
						bitScanForward(board->pieces[WHITE][BISHOP]),
						bitScanForward(board->pieces[BLACK][BISHOP])))
			{
				return 1;
			}
			break;
		}
		break;
	}

	return 0;
}


static int threeFoldRepetition(const Board *board) {
	int counter = 0;

	ASSERT(memory.size == board->ply);
	ASSERT(memory.size - board->fiftyMoves - 1 >= 0)

	for (int i = 0; i < memory.size; ++i) {
		if (board->key == memory.keys[i]) {
			if (++counter == 3)
				return 1;
		}
	}

	return 0;
}

void saveKeyToMemory(const uint64_t key) {
	ASSERT(memory.size >= 0 && memory.size < MAX_GAME_LENGTH);

	memory.keys[memory.size] = key;
	++(memory.size);
}

void freeKeyFromMemory(void) {
	--(memory.size);
	ASSERT(memory.size >= 0);
}

void clearKeys(void) {
	memory.size = 0;
}