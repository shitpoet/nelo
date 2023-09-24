#include "board.h"
#include "draw.h"
#include "eval.h"

static void countPieces(const Board *board, Count *wCount, Count *bCount);

static int getPhase(const Count *wCount, const Count *bCount);
static inline int taperedEval(const int phase, const int opening, const int endgame);

static int pieceSquareTables(const Board *board, const int phase, const int color);

static int materialCount(const Count *count);

int pieceValues[6] = {100, 325, 330, 550, 900, 10000};

/*
 * Evaluates a position that has no possible moves.
 */
int finalEval(const Board *board, const int depth) {
	return inCheck(board) ? -MAX_SCORE - depth : 0;
}

/*
 * This is the main evaluation function.
 * Scores are positive for the side about to play.
 */
int eval(const Board *board) {
	Count wCount, bCount;

	countPieces(board, &wCount, &bCount);

	const int phase = getPhase(&wCount, &bCount);

	int score = 0;

	score += materialCount(&wCount) - materialCount(&bCount);
	score += pieceSquareTables(board, phase, WHITE) - pieceSquareTables(board, phase, BLACK);

	if (board->turn == BLACK)
		score = -score;

	return score;
}

/*
 * Two structures are generated to save recomputing pop counts later on.
 */
static void countPieces(const Board *board, Count *wCount, Count *bCount) {
	wCount->nPawns   = popCount(board->pieces[WHITE][PAWN]);
	wCount->nKnights = popCount(board->pieces[WHITE][KNIGHT]);
	wCount->nBishops = popCount(board->pieces[WHITE][BISHOP]);
	wCount->nRooks   = popCount(board->pieces[WHITE][ROOK]);
	wCount->nQueens  = popCount(board->pieces[WHITE][QUEEN]);
	wCount->nTotal   = popCount(board->players[WHITE]);

	bCount->nPawns   = popCount(board->pieces[BLACK][PAWN]);
	bCount->nKnights = popCount(board->pieces[BLACK][KNIGHT]);
	bCount->nBishops = popCount(board->pieces[BLACK][BISHOP]);
	bCount->nRooks   = popCount(board->pieces[BLACK][ROOK]);
	bCount->nQueens  = popCount(board->pieces[BLACK][QUEEN]);
	bCount->nTotal   = popCount(board->players[BLACK]);
}

/*
 * Returns the fase of the game with a number ranging from 1 to 256.
 * The higher the number, the more advanced the game is.
 */
static int getPhase(const Count *wCount, const Count *bCount) {
    const int knightPhase = 1;
    const int bishopPhase = 1;
    const int rookPhase   = 2;
    const int queenPhase  = 4;

    const int totalPhase = 4 * knightPhase + 4 * bishopPhase + 4 * rookPhase + 2 * queenPhase;

    const int phase = totalPhase -
    		knightPhase * (wCount->nKnights + bCount->nKnights) -
    		bishopPhase * (wCount->nBishops + bCount->nBishops) -
			rookPhase   * (wCount->nRooks   + bCount->nRooks)   -
			queenPhase  * (wCount->nQueens  + bCount->nQueens);

    return (phase * 256 + (totalPhase / 2)) / totalPhase;
}

int isEndgame(const Board *board) {
	Count wCount, bCount;

	countPieces(board, &wCount, &bCount);

	return getPhase(&wCount, &bCount) > 150;
}

/*
 * It gives weights to the opening/middle-game evaluation and the endgame
 * based on the phase.
 */
static inline int taperedEval(const int phase, const int opening, const int endgame) {
	return ((opening * (256 - phase)) + (endgame * phase)) / 256;
}

static int materialCount(const Count *count) {
	return  pieceValues[PAWN]   * count->nPawns +
			pieceValues[KNIGHT] * count->nKnights +
			pieceValues[BISHOP] * count->nBishops +
			pieceValues[ROOK]   * count->nRooks +
			pieceValues[QUEEN]  * count->nQueens;
}

/*
 * Gives points to every piece depending on its position on the board->
 * It can be either a bonus or a penalty.
 * It also depends on the stage of the game: opening/middle-game or endgame.
 */
static int pieceSquareTables(const Board *board, const int phase, const int color) {
	static const int pst[2][6][64] = {
	{
		{ 0, 0, 0, 0, 0, 0, 0, 0, -1, -7, -11, -35, -13, 5, 3, -5, 1, 1, -6, -19, -6, -7, -4, 10, 1, 14, 8, 4, 5, 4, 10, 7, 9, 30, 23, 31, 31, 23, 17, 11, 21, 54, 72, 56, 77, 95, 71, 11, 118, 121, 173, 168, 107, 82, -16, 22, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ -99, -30, -66, -64, -29, -19, -61, -81, -56, -31, -28, -1, -7, -20, -42, -11, -38, -16, 0, 14, 8, 3, 3, -42, -14, 0, 2, 3, 19, 12, 33, -7, -14, -4, 25, 33, 10, 33, 14, 43, -22, 18, 60, 64, 124, 143, 55, 6, -34, 24, 54, 74, 60, 122, 2, 29, -60, 0, 0, 0, 0, 0, 0, 0 },
		{ -7, 12, -8, -37, -31, -8, -45, -67, 15, 5, 13, -10, 1, 2, 0, 15, 5, 12, 14, 13, 10, -1, 3, 4, 1, 5, 23, 32, 21, 8, 17, 4, -1, 16, 29, 27, 37, 27, 17, 4, 7, 27, 20, 56, 91, 108, 53, 44, -24, -23, 30, 58, 65, 61, 69, 11, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ -2, -1, 3, 1, 2, 1, 4, -8, -26, -6, 2, -2, 2, -10, -1, -29, -16, 0, 3, -3, 8, -1, 12, 3, -9, -5, 8, 14, 18, -17, 13, -13, 19, 33, 46, 57, 53, 39, 53, 16, 24, 83, 54, 75, 134, 144, 85, 75, 46, 33, 64, 62, 91, 89, 70, 104, 84, 0, 0, 37, 124, 0, 0, 153 },
		{ 1, -10, -11, 3, -15, -51, -83, -13, -7, 3, 2, 5, -1, -10, -7, -2, -11, 0, 12, 2, 8, 11, 7, -6, -9, 5, 7, 9, 18, 17, 26, 4, -6, 0, 15, 25, 32, 9, 26, 12, -16, 10, 13, 25, 37, 30, 15, 26, 1, 11, 35, 0, 16, 55, 39, 57, -13, 6, -42, 0, 29, 0, 0, 102 },
		{ 0, 0, 0, -9, 0, -9, 25, 0, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9, -9 }
	},
	{
		{ 0, 0, 0, 0, 0, 0, 0, 0, -17, -17, -17, -17, -17, -17, -17, -17, -11, -11, -11, -11, -11, -11, -11, -11, -7, -7, -7, -7, -7, -7, -7, -7, 16, 16, 16, 16, 16, 16, 16, 16, 55, 55, 55, 55, 55, 55, 55, 55, 82, 82, 82, 82, 82, 82, 82, 82, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ -99, -99, -94, -88, -88, -94, -99, -99, -81, -62, -49, -43, -43, -49, -62, -81, -46, -27, -15, -9, -9, -15, -27, -46, -22, -3, 10, 16, 16, 10, -3, -22, -7, 12, 25, 31, 31, 25, 12, -7, -2, 17, 30, 36, 36, 30, 17, -2, -7, 12, 25, 31, 31, 25, 12, -7, -21, -3, 10, 16, 16, 10, -3, -21 },
		{ -27, -21, -17, -15, -15, -17, -21, -27, -10, -4, 0, 2, 2, 0, -4, -10, 2, 8, 12, 14, 14, 12, 8, 2, 11, 17, 21, 23, 23, 21, 17, 11, 14, 20, 24, 26, 26, 24, 20, 14, 13, 19, 23, 25, 25, 23, 19, 13, 8, 14, 18, 20, 20, 18, 14, 8, -2, 4, 8, 10, 10, 8, 4, -2 },
		{ -32, -31, -30, -29, -29, -30, -31, -32, -27, -25, -24, -24, -24, -24, -25, -27, -15, -13, -12, -12, -12, -12, -13, -15, 1, 2, 3, 4, 4, 3, 2, 1, 15, 17, 18, 18, 18, 18, 17, 15, 25, 27, 28, 28, 28, 28, 27, 25, 27, 28, 29, 30, 30, 29, 28, 27, 16, 17, 18, 19, 19, 18, 17, 16 },
		{ -61, -55, -52, -50, -50, -52, -55, -61, -31, -26, -22, -21, -21, -22, -26, -31, -8, -3, 1, 3, 3, 1, -3, -8, 9, 14, 17, 19, 19, 17, 14, 9, 19, 24, 28, 30, 30, 28, 24, 19, 23, 28, 32, 34, 34, 32, 28, 23, 21, 26, 30, 31, 31, 30, 26, 21, 12, 17, 21, 23, 23, 21, 17, 12 },
		{ -34, -30, -28, -27, -27, -28, -30, -34, -17, -13, -11, -10, -10, -11, -13, -17, -2, 2, 4, 5, 5, 4, 2, -2, 11, 15, 17, 18, 18, 17, 15, 11, 22, 26, 28, 29, 29, 28, 26, 22, 31, 34, 37, 38, 38, 37, 34, 31, 38, 41, 44, 45, 45, 44, 41, 38, 42, 46, 48, 50, 50, 48, 46, 42 }
	}};

	int (*indexFunc)(uint64_t) = (color == WHITE) ? bitScanForward : mirrorLSB;

	int opening = 0, endgame = 0;

	for (int piece = PAWN; piece <= KING; piece++) {
		uint64_t bb = board->pieces[color][piece];

		if (bb) do {
			const int index = indexFunc(bb);
			opening += pst[OPENING][piece][index];
			endgame += pst[ENDGAME][piece][index];
		} while (unsetLSB(bb));
	}

	return taperedEval(phase, opening, endgame);
}
