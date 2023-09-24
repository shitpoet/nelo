// cederchess2snapshot.cpp
// by Claus Cederberg, 27. August 2023
// g++ cederchess2snapshot.cpp -O3 -o cederchess2

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <random>
#include <chrono>
#include <functional>

// Board values/pieces
#define EMPTY 0
#define BORDER 1
#define WHITE_PAWN 100
#define WHITE_KNIGHT 320
#define WHITE_BISHOP 330
#define WHITE_ROOK 500
#define WHITE_QUEEN 900
#define WHITE_KING 10000
#define BLACK_PAWN -100
#define BLACK_KNIGHT -320
#define BLACK_BISHOP -330
#define BLACK_ROOK -500
#define BLACK_QUEEN -900
#define BLACK_KING -10000

#define FEN_STRING_SIMPLE 0
#define FEN_STRING_FULL 1

enum movetype {
	MOVE_TYPE_NORMAL, MOVE_TYPE_SHORT_CASTLING, MOVE_TYPE_LONG_CASTLING, MOVE_TYPE_QUEEN_PROMOTION, MOVE_TYPE_EN_PASSANT
};

// DEPTH_DEFAULT 6 means that engine takes move, then opponent takes move, then engine takes move, then opponent takes move,
// then engine takes move, then opponent takes move.
// Important: Keep DEPTH_DEFAULT even. The whole program is dependent on that.
// DEPTH_DEFAULT 6 is standard for blitz. The engine will automatically decrease depth if time left goes under a limit,
// and increase depth if time is over a limit or we are in an end game with limited moves.
// When best move is found, the engine always starts with depth 2, then add 2 to the depth until maximum depth is reached.
// There are two reasons for this:
// 1. The engine will detect a mate in one move for depth 2, two moves for depth 4 and so on.
// 2. On low depth the moves will be ordered to help alpha beta pruning on the higher depth.
#define DEPTH_DEFAULT 6

// When engine has less than TIME_LIMIT_DEC_DEPTH ms left, then decrease search depth by 2.
#define TIME_LIMIT_DEC_DEPTH 40000 /* 40 seconds */

// When engine has more than TIME_LIMIT_INC_DEPTH ms left or unlimited time left, then increase search depth by 2.
#define TIME_LIMIT_INC_DEPTH 600000 /* 10 minutes */

// To avoid the same game over and over again, then a random number is added to each move score.
// RANDOM_MAX is the number (0 to RANDOM_MAX - 1) to add to first moves.
// Then for each move the number is decreased by RANDOM_DEC until RANDOM_MIN is reached.
// The rest of the moves in the game will have the number (0 to RANDOM_MIN - 1) added.
// Set RANDOM_MAX 0 if no random at all.
#define RANDOM_MAX 24
#define RANDOM_DEC 2
#define RANDOM_MIN 4

// This holds the actual random max number:
int_fast16_t moverandom;

// The score of the board is SCORE_INFINITE when computer is setting mate, and -SCORE_INFINITE when opponents is setting mate.
#define SCORE_INFINITE 32000
#define SCORE_STALEMATE_ENGINE_REPETITION -50
#define SCORE_STALEMATE_ENGINE_NO_MOVES -51
#define SCORE_STALEMATE_OPPONENT_REPETITION -52
#define SCORE_STALEMATE_OPPONENT_NO_MOVES -53
#define SCORE_STALEMATE_INSUFFICIENT_MATERIAL -54

// Holds information about a move, so it can be undone.
// We can hold 128 moves on each depth, up to maximal depth 16.
struct movestruct {
	int_fast16_t from;
	int_fast16_t to;
	int_fast16_t piecetaken;
	movetype type;
	int_fast16_t score;
} moves[16][128];

int_fast16_t nmoves[16];

// Holds information about castling possibilities.
struct castlingstruct {
	bool whiteking;
	bool whitequeen;
	bool blackking;
	bool blackqueen;
} castlingpossible;

// In this program a board is 16x16. The 4 "outer" files and ranks are set to BORDER, the rest (those in the middle) are set to EMPTY or a piece.
// A BORDER is used in move generator to avoid constantly checking for out of bounds. It is easier to check for BORDER.
// 16x16 is used as we can shift some stuff for performance.
// There is only one board when finding moves. There is constantly doing and undoing moves on this board, and it is constantly flipped.
// Flipping the board helps all sort of things as we only need to see stuff from one side.
int_fast16_t board[256];

// Constantly keep track of the position of the white and the black king. This helps checking for check.
int_fast16_t whitekingposition;
int_fast16_t blackkingposition;

// Set to true if white to move, else false.
bool whitetomove;

// This is set if the prior move was en passant.
int_fast16_t enpassantposition;

// Explained in function setstate.
int_fast16_t pawnposscores[256];
int_fast16_t knightposscores[256];
int_fast16_t bishopposscores[256];
int_fast16_t rookposscores[256];
int_fast16_t queenposscores[256];
int_fast16_t kingposscores[256];
int_fast16_t pawnposscoresblack[256];
int_fast16_t knightposscoresblack[256];
int_fast16_t bishopposscoresblack[256];
int_fast16_t rookposscoresblack[256];
int_fast16_t queenposscoresblack[256];
int_fast16_t kingposscoresblack[256];

std::string infostring;

// -------------------------------------------------------------------------------------------------------
// STRING FUNCTIONS
// -------------------------------------------------------------------------------------------------------

// Returns a readable string representation of the field 'f'.
std::string fieldstring(int_fast16_t f) {
	return std::string(1, 'a' + (f & 15) - 4) + std::string(1, '1' + (f >> 4) - 4) + " (" + std::to_string(f) + ")";
}

// Returns a readable string representation of the move 'move'.
// 'boardflipped' should be true if board is flipped.
// If 'extended' is set, then a "-" or "x" is added in the middle of the string.
std::string movestring(struct movestruct move, bool boardflipped, bool extended) {
	std::string result;
	if (boardflipped) {
		result = std::string(1, 'a' + (move.from & 15) - 4) + std::string(1, '8' - ((move.from >> 4) - 4)) +
				(extended ? (move.piecetaken == 0 ? "-" : "x") : "") +
				std::string(1, 'a' + (move.to & 15) - 4) + std::string(1, '8' - ((move.to >> 4) - 4));
	} else {
		result = std::string(1, 'a' + (move.from & 15) - 4) + std::string(1, '1' + (move.from >> 4) - 4) +
				(extended ? (move.piecetaken == 0 ? "-" : "x") : "")+
				std::string(1, 'a' + (move.to & 15) - 4) + std::string(1, '1' + (move.to >> 4) - 4);
	}
	if (move.type == MOVE_TYPE_QUEEN_PROMOTION) {
		result += "q";
	}
	return result;
}

// Returns a castling string based on the global variable 'castlingpossible'.
std::string castlingstring() {
	std::string result = std::string(castlingpossible.whiteking ? "K" : "") +
					std::string(castlingpossible.whitequeen ? "Q" : "") +
					std::string(castlingpossible.blackking ? "k" : "") +
					std::string(castlingpossible.blackqueen ? "q" : "");
	return (result == "" ? std::string("-") : result);
}

// -------------------------------------------------------------------------------------------------------
// BOARD HISTORY AND HASHING
// Hash values of the boards throughout the game are stored in the globale array 'haskeys'.
// This is used to detect threefold repetition. Zobrist Hashing is used.
// See https://www.chessprogramming.org/Zobrist_Hashing
// All hash values of boards in a game are stored in the globale vector 'boardhistory'.
// -------------------------------------------------------------------------------------------------------

// Used for generation of hash keys and move random:
std::mt19937 randomgenerator;

uint_fast32_t hashkeys[12][256];

std::vector<uint_fast32_t> boardhistory;

void generatehashkeys() {
	randomgenerator.seed(33);  // use a constant seed to have same hash keys every time
	for (int_fast16_t n = 11; n >= 0; n--) {
		for (int_fast16_t row = 4; row <= 11; row++) {
			for (int_fast16_t col = 4; col <= 11; col++) {
				hashkeys[n][(row << 4) + col] = randomgenerator();
			}
		}
	}
}

uint_fast32_t gethash(bool boardflipped) {
	uint_fast32_t hash = 0;
	if (boardflipped) {
		for (int_fast16_t row = 4; row <= 11; row++) {
			for (int_fast16_t col = 4; col <= 11; col++) {
				switch (board[(row << 4) + col]) {
					case WHITE_PAWN: hash ^= hashkeys[6][((15 - row) << 4) + col]; break;
					case WHITE_KNIGHT: hash ^= hashkeys[7][((15 - row) << 4) + col]; break;
					case WHITE_BISHOP: hash ^= hashkeys[8][((15 - row) << 4) + col]; break;
					case WHITE_ROOK: hash ^= hashkeys[9][((15 - row) << 4) + col]; break;
					case WHITE_QUEEN: hash ^= hashkeys[10][((15 - row) << 4) + col]; break;
					case WHITE_KING: hash ^= hashkeys[11][((15 - row) << 4) + col]; break;
					case BLACK_PAWN: hash ^= hashkeys[0][((15 - row) << 4) + col]; break;
					case BLACK_KNIGHT: hash ^= hashkeys[1][((15 - row) << 4) + col]; break;
					case BLACK_BISHOP: hash ^= hashkeys[2][((15 - row) << 4) + col]; break;
					case BLACK_ROOK: hash ^= hashkeys[3][((15 - row) << 4) + col]; break;
					case BLACK_QUEEN: hash ^= hashkeys[4][((15 - row) << 4) + col]; break;
					case BLACK_KING: hash ^= hashkeys[5][((15 - row) << 4) + col]; break;
				}
			}
		}
	} else {
		for (int_fast16_t row = 4; row <= 11; row++) {
			for (int_fast16_t col = 4; col <= 11; col++) {
				switch (board[(row << 4) + col]) {
					case WHITE_PAWN: hash ^= hashkeys[0][(row << 4) + col]; break;
					case WHITE_KNIGHT: hash ^= hashkeys[1][(row << 4) + col]; break;
					case WHITE_BISHOP: hash ^= hashkeys[2][(row << 4) + col]; break;
					case WHITE_ROOK: hash ^= hashkeys[3][(row << 4) + col]; break;
					case WHITE_QUEEN: hash ^= hashkeys[4][(row << 4) + col]; break;
					case WHITE_KING: hash ^= hashkeys[5][(row << 4) + col]; break;
					case BLACK_PAWN: hash ^= hashkeys[6][(row << 4) + col]; break;
					case BLACK_KNIGHT: hash ^= hashkeys[7][(row << 4) + col]; break;
					case BLACK_BISHOP: hash ^= hashkeys[8][(row << 4) + col]; break;
					case BLACK_ROOK: hash ^= hashkeys[9][(row << 4) + col]; break;
					case BLACK_QUEEN: hash ^= hashkeys[10][(row << 4) + col]; break;
					case BLACK_KING: hash ^= hashkeys[11][(row << 4) + col]; break;
				}
			}
		}
	}
	return hash;
}

uint_fast16_t counthash(uint_fast32_t hash) {
	uint_fast32_t count = 0;
	for (uint_fast32_t h : boardhistory) {
		if (h == hash) {
			count++;
		}
	}
	return count;
}

// -----------------------------------------------------------------------------------------------------------------
// STATE
// A game goes through different states. A state depends on the number and types of pieces on the board.
// A state controls the positions scores, the search depth, and the evaluation function.
// The position scores contains the scores for each pieces on each position.
// The different states are listed in a kind of natural order, and it is important not to change that order.
// State is not changed during search for best move.
// The globale variable 'statemap' contains all the different states and there properties.
// -----------------------------------------------------------------------------------------------------------------

enum stateenum {
	STATE_START_GAME, STATE_MIDDLE_GAME, STATE_LATE_MIDDLE_GAME, STATE_END_GAME, STATE_PAWN_END_GAME, STATE_PAWN_ONLY_END_GAME,
	STATE_ENGINE_PAWN_VS_KING, STATE_ENGINE_KING_VS_PAWN, STATE_ENGINE_RQ_VS_KING, STATE_ENGINE_KING_VS_NOPAWNS
};

struct statestruct {
	stateenum state;
	std::string name;
	int_fast16_t defaultdepth;
	std::function<int_fast16_t(bool)> evaluate;

	statestruct(stateenum state, std::string name, int_fast16_t defaultdepth, std::function<int_fast16_t(bool)> evaluate)
		: state(state), name(name), defaultdepth(defaultdepth), evaluate(evaluate) {
	}
};

std::map<stateenum, statestruct> statemap;

statestruct getstate() {
	static int_fast16_t pawnpositionscores[8][8] = {
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{  50,  50,  50,  50,  50,  50,  50,  50 },
		{  10,  10,  20,  30,  30,  20,  10,  10 },
		{   5,   5,  10,  25,  25,  10,   5,   5 },
		{   0,   0,   0,  20,  20,   0,   0,   0 },
		{   5,  -5, -10,   0,   0, -10,  -5,   5 },
		{   5,  10,  10, -20, -20,  10,  10,   5 },
		{   0,   0,   0,   0,   0,   0,   0,   0 }
	};

	// Larger and larger gap between rank scores, so the pawns closest to promotion are preferable to move:
	static int_fast16_t pawnendpositionscores[8][8] = {
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{  80,  80,  80,  80,  80,  80,  80,  80 },
		{  50,  50,  50,  50,  50,  50,  50,  50 },
		{  30,  30,  30,  30,  30,  30,  30,  30 },
		{  15,  15,  15,  15,  15,  15,  15,  15 },
		{   5,   5,   5,   5,   5,   5,   5,   5 },
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{   0,   0,   0,   0,   0,   0,   0,   0 }
	};

	static int_fast16_t knightpositionscores[8][8] = {
		{ -50, -40, -30, -30, -30, -30, -40, -50 },
		{ -40, -20,   0,   0,   0,   0, -20, -40 },
		{ -30,   0,  10,  15,  15,  10,   0, -30 },
		{ -30,   5,  15,  20,  20,  15,   5, -30 },
		{ -30,   0,  15,  20,  20,  15,   0, -30 },
		{ -30,   5,  10,  15,  15,  10,   5, -30 },
		{ -40, -20,   0,   5,   5,   0, -20, -40 },
		{ -50, -40, -30, -30, -30, -30, -40, -50 }
	};

	static int_fast16_t bishoppositionscores[8][8] = {
		{ -20, -10, -10, -10, -10, -10, -10, -20 },
		{ -10,   0,   0,   0,   0,   0,   0, -10 },
		{ -10,   0,   5,  10,  10,   5,   0, -10 },
		{ -10,   5,   5,  10,  10,   5,   5, -10 },
		{ -10,   0,  10,  10,  10,  10,   0, -10 },
		{ -10,  10,  10,  10,  10,  10,  10, -10 },
		{ -10,   5,   0,   0,   0,   0,   5, -10 },
		{ -20, -10, -10, -10, -10, -10, -10, -20 }
	};

	static int_fast16_t rookpositionscores[8][8] = {
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{   5,  10,  10,  10,  10,  10,  10,   5 },
		{  -5,   0,   0,   0,   0,   0,   0,  -5 },
		{  -5,   0,   0,   0,   0,   0,   0,  -5 },
		{  -5,   0,   0,   0,   0,   0,   0,  -5 },
		{  -5,   0,   0,   0,   0,   0,   0,  -5 },
		{  -5,   0,   0,   0,   0,   0,   0,  -5 },
		{   0,   0,   0,   5,   5,   0,   0,   0 }
	};

	static int_fast16_t queenpositionscores[8][8] = {
		{ -20, -10, -10,  -5,  -5, -10, -10, -20 },
		{ -10,   0,   0,   0,   0,   0,   0, -10 },
		{ -10,   0,   5,   5,   5,   5,   0, -10 },
		{  -5,   0,   5,   5,   5,   5,   0,  -5 },
		{  -5,   0,   5,   5,   5,   5,   0,  -5 },
		{ -10,   0,   5,   5,   5,   5,   0, -10 },
		{ -10,   0,   0,   0,   0,   0,   0, -10 },
		{ -20, -10, -10,  -5,  -5, -10, -10, -20 }
	};

	static int_fast16_t kingpositionscores[8][8] = {
		{ -30, -40, -40, -50, -50, -40, -40, -30 },
		{ -30, -40, -40, -50, -50, -40, -40, -30 },
		{ -30, -40, -40, -50, -50, -40, -40, -30 },
		{ -30, -40, -40, -50, -50, -40, -40, -30 },
		{ -20, -30, -30, -40, -40, -30, -30, -20 },
		{ -10, -20, -20, -20, -20, -20, -20, -10 },
		{  20,  20,   0,   0,   0,   0,  20,  20 },
		{  20,  30,  10,   0,   0,  10,  30,  20 }
	};

	static int_fast16_t kingendpositionscores[8][8] = {
		{ -50, -30, -30, -30, -30, -30, -30, -50 },
		{ -30, -30,   0,   0,   0,   0, -30, -30 },
		{ -30, -10,  20,  30,  30,  20, -10, -30 },
		{ -30, -10,  30,  40,  40,  30, -10, -30 },
		{ -30, -10,  30,  40,  40,  30, -10, -30 },
		{ -30, -10,  20,  30,  30,  20, -10, -30 },
		{ -30, -20, -10,   0,   0, -10, -20, -30 },
		{ -50, -40, -30, -20, -20, -30, -40, -50 }
	};

	static int_fast16_t zeropositionscores[8][8] = {
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{   0,   0,   0,   0,   0,   0,   0,   0 },
		{   0,   0,   0,   0,   0,   0,   0,   0 }
	};

	// Count the number of pieces:
	int_fast16_t wq = 0, bq = 0;
	int_fast16_t wr = 0, br = 0;
	int_fast16_t wb = 0, bb = 0;
	int_fast16_t wk = 0, bk = 0;
	int_fast16_t wp = 0, bp = 0;
	for (int_fast16_t row = 4; row <= 11; row++) {
		for (int_fast16_t col = 4; col <= 11; col++) {
			switch (board[(row << 4) + col]) {
				case WHITE_QUEEN: wq++; break;
				case BLACK_QUEEN: bq++; break;
				case WHITE_ROOK: wr++; break;
				case BLACK_ROOK: br++; break;
				case WHITE_BISHOP: wb++; break;
				case BLACK_BISHOP: bb++; break;
				case WHITE_KNIGHT: wk++; break;
				case BLACK_KNIGHT: bk++; break;
				case WHITE_PAWN: wp++; break;
				case BLACK_PAWN: bp++; break;
			}
		}
	}

	// Set the state based on the number of pieces:
	stateenum state = STATE_MIDDLE_GAME;
	if (wq + bq + wr + br == 0 && wb + bb + wk + bk <= 1 && wp + bp == 1) {
		state = ((whitetomove && wp == 1 && bb + bk == 0 || !whitetomove && bp == 1 && wb + wk == 0) ? STATE_ENGINE_PAWN_VS_KING : STATE_ENGINE_KING_VS_PAWN);
	} else if (whitetomove && wq + wr + wb + wk + wp == 0 && bp == 0 || !whitetomove && bq + br + bb + bk + bp == 0 && wp == 0) {
		state = STATE_ENGINE_KING_VS_NOPAWNS;
	} else if (wb + bb + wk + bk + wp + bp == 0 && wr + br + wq + bq == 1) {
		state = STATE_ENGINE_RQ_VS_KING;
	} else if (wq == 0 && bq == 0 && wr == 0 && br == 0	&& wb == 0 && bb == 0 && wk == 0 && bk == 0) {
		// We are in pawn end game if both sides have no queens, rooks or officers:
		state = STATE_PAWN_ONLY_END_GAME;
	} else if (wq + bq == 0 && wb + wk + wr <= 1 && bb + bk + br <= 1) {
		// We are in pawn end game if both sides (1) have no queens and (2) maximum 1 officer or rook:
		state = STATE_PAWN_END_GAME;
	} else if (wq + bq + wr + br <= 2 && wq + bq + wr + br + wb + wk + bb + bk <= 4) {
		// We are in end game if there a maximum of two queens/rooks on the boards and a maximum of four officers on the board totally:
		state = STATE_END_GAME;
	} else if ((wq == 0 && wr + wb + wk <= 3 || wr + wb + wk <= 1) && (bq == 0 && br + bb + bk <= 3 || br + bb + bk <= 1)) {
		// We are in Late Middle Game if both sides (1) have no queens and maximum 3 officers, or (2) one queen and maximum 1 officer.
		state = STATE_LATE_MIDDLE_GAME;
	} else	if (wq + bq + wr + br + wb + bb + wk + bk + wp + bp >= 30) {
		// We are in start game if no pieces has been taken.
		state = STATE_START_GAME;
	}

	// Set the position scores based on the state:
	for (int_fast16_t r = 15; r >= 0; r--) {
		for (int_fast16_t c = 15; c >= 0; c--) {
			if (r < 4 || r > 11 || c < 4 || c > 11) {
				pawnposscores[(r << 4) + c] = BORDER;
				knightposscores[(r << 4) + c] = BORDER;
				bishopposscores[(r << 4) + c] = BORDER;
				rookposscores[(r << 4) + c] = BORDER;
				queenposscores[(r << 4) + c] = BORDER;
				kingposscores[(r << 4) + c] = BORDER;
				pawnposscoresblack[(r << 4) + c] = BORDER;
				knightposscoresblack[(r << 4) + c] = BORDER;
				bishopposscoresblack[(r << 4) + c] = BORDER;
				rookposscoresblack[(r << 4) + c] = BORDER;
				queenposscoresblack[(r << 4) + c] = BORDER;
				kingposscoresblack[(r << 4) + c] = BORDER;
			} else {
				pawnposscores[(r << 4) + c] = (state >= STATE_PAWN_END_GAME ? pawnendpositionscores[11-r][c-4] : pawnpositionscores[11-r][c-4]);
				knightposscores[(r << 4) + c] = knightpositionscores[11-r][c-4];
				bishopposscores[(r << 4) + c] = bishoppositionscores[11-r][c-4];
				rookposscores[(r << 4) + c] = rookpositionscores[11-r][c-4];
				queenposscores[(r << 4) + c] = queenpositionscores[11-r][c-4];
				kingposscores[(r << 4) + c] = (state >= STATE_LATE_MIDDLE_GAME ? kingendpositionscores[11-r][c-4] : kingpositionscores[11-r][c-4]);
				pawnposscoresblack[(r << 4) + c] = (state >= STATE_PAWN_END_GAME ? pawnendpositionscores[r-4][c-4] : pawnpositionscores[r-4][c-4]);
				knightposscoresblack[(r << 4) + c] = knightpositionscores[r-4][c-4];
				bishopposscoresblack[(r << 4) + c] = bishoppositionscores[r-4][c-4];
				rookposscoresblack[(r << 4) + c] = rookpositionscores[r-4][c-4];
				queenposscoresblack[(r << 4) + c] = queenpositionscores[r-4][c-4];
				kingposscoresblack[(r << 4) + c] = (state >= STATE_LATE_MIDDLE_GAME ? kingendpositionscores[r-4][c-4] : kingpositionscores[r-4][c-4]);
			}
		}
	}

	return statemap.find(state)->second;
}

// ----------------------------------------------------------------------------------------------------------
// EVALUATION
// There are three types of evaluation functions: base, move and node
// They work like this: Base and move are only called once per move, so they can make processor heavy
// calculations. Node is called on every node in the search algorithm, so this have to be done quickly.
// There are different variations of node evaluation function. The variant to use depends on the game state.
// ----------------------------------------------------------------------------------------------------------

// Engine is white when this base evaluation function is called.
int_fast16_t evaluatebase(bool boardflipped, statestruct state) {
	int_fast16_t score = 0;

	if (moverandom > 0) {
		score = randomgenerator() % moverandom;
	}

	if (state.state >= STATE_ENGINE_PAWN_VS_KING) {
		return score;
	}

	// Punish getting the queen early out:
	if (state.state == STATE_START_GAME && board[71] != WHITE_QUEEN) {
		score -= 50;
	}

	// Punish moving king or rook if castling hasn't been done:
	if (!boardflipped && (board[72] != WHITE_KING && (castlingpossible.whitequeen || castlingpossible.whiteking) || board[68] != WHITE_ROOK && castlingpossible.whitequeen || board[75] != WHITE_ROOK && castlingpossible.whiteking)) {
		score -= 50;
	}
	if (boardflipped && (board[72] != WHITE_KING && (castlingpossible.blackqueen || castlingpossible.blackking) || board[68] != WHITE_ROOK && castlingpossible.blackqueen || board[75] != WHITE_ROOK && castlingpossible.blackking)) {
		score -= 50;
	}

	return score;
}

// Engine is white when this move evaluation function is called.
int_fast16_t evaluatemove(struct movestruct move) {
	// Embrace castling:
	if (move.type == MOVE_TYPE_SHORT_CASTLING || move.type == MOVE_TYPE_LONG_CASTLING) {
		return 50;
	}

	// Embrace capturing moves if engine/white has a considerable material advantage:
	int_fast16_t materialscore = 0;
	for (int_fast16_t f = 68; f <= 187; ) {
		materialscore += board[f];
		if ((++f & 15) == 12) {
			f += 8;
		}
	}
	if (move.piecetaken != 0 && materialscore + move.piecetaken >= 200) {
		return 20;
	}

	return 0;
}

// NODE EVALUATION FUNCTIONS
// Notes:
// (1) the engine is black because depth is even as we flip the board throughout the search
// (2) engine is the next to move.

// Engine has a single pawn and an optional bishop or knight against opponents sole king.
// Engine goal is to get the pawn promote to a queen.
int_fast16_t evaluateenginepawnvsking(bool coutscores) {
	int_fast16_t score = 0;

	int_fast16_t blackpawnrow = 0;
	int_fast16_t blackpawncol = 0;

	for (int_fast16_t f = 68; f <= 187; ) {
		if (board[f] == BLACK_PAWN) {
			score += pawnposscoresblack[f];
			blackpawnrow = (f >> 4);
			blackpawncol = (f & 15);
		} else if (board[f] == BLACK_QUEEN) {
			// If we can see a pawn promotion (to queen) in the horizon, then we go for that:
			if (coutscores) { std::cout << "EP:" << std::to_string(WHITE_QUEEN); }
			return WHITE_QUEEN;
		}
		if ((++f & 15) == 12) {
			f += 8;
		}
	}

	// If the black pawn has been lost in the horizon, then return stale mate:
	if (blackpawnrow == 0) {
		if (coutscores) { std::cout << "EP:" << std::to_string(SCORE_STALEMATE_INSUFFICIENT_MATERIAL); }
		return SCORE_STALEMATE_INSUFFICIENT_MATERIAL;
	}

	int_fast16_t whitekingrow = (whitekingposition >> 4);
	int_fast16_t whitekingcol = (whitekingposition & 15);
	if (blackpawnrow < whitekingrow || blackpawnrow - 4 < abs(blackpawncol - whitekingcol)) {
		// The engine/black pawn can't be reached by opponent/white king, we are happy
		score += 800;
	} else {
		// The engine/black king should be as close to the pawn as possible
		int_fast16_t blackkingrow = (blackkingposition >> 4);
		int_fast16_t blackkingcol = (blackkingposition & 15);
		score += score + 700 - 100 * (abs(blackpawnrow - blackkingrow) > abs(blackpawncol - blackkingcol) ? abs(blackpawnrow - blackkingrow) : abs(blackpawncol - blackkingcol));
	}

	if (coutscores) { std::cout << "EP:" << std::to_string(score); }

	return score;
}

// Engine has nothing but a sole king. Opponent has a single pawn and an optional bishop or knight.
// Engine goal is take the pawn before it promotes.
int_fast16_t evaluateenginekingvspawn(bool coutscores) {
	int_fast16_t opponentpawnrow = 0;
	int_fast16_t opponentpawncol = 0;

	for (int_fast16_t f = 68; f <= 187; ) {
		if (board[f] == WHITE_PAWN) {
			opponentpawnrow = (f >> 4);
			opponentpawncol = (f & 15);
		} else if (board[f] == WHITE_QUEEN) {
			// If there is a opponent pawn promotion (to queen) in the horizon, then engine is doomed:
			if (coutscores) { std::cout << "OP:" << std::to_string(BLACK_QUEEN); }
			return BLACK_QUEEN;
		}
		if ((++f & 15) == 12) {
			f += 8;
		}
	}

	if (opponentpawnrow == 0) {
		// If there is no opponent pawn in the horizon, then there is a stalemate:
		if (coutscores) { std::cout << "OP:" << std::to_string(SCORE_STALEMATE_INSUFFICIENT_MATERIAL); }
		return SCORE_STALEMATE_INSUFFICIENT_MATERIAL;
	}

	// Engine king moves as close as possible to opponent pawn:
	int_fast16_t enginekingrow = (blackkingposition >> 4);
	int_fast16_t enginekingcol = (blackkingposition & 15);
	int_fast16_t score = -100 * std::max(std::abs(enginekingcol - opponentpawncol), std::abs(enginekingrow - opponentpawnrow));

	if (coutscores) { std::cout << "OP:" << std::to_string(score); }

	return score;
}

// Engine has a rook or a queen against opponents sole king.
// Engine goal is to force opponent king to the edge and keep engine king close to opponent king.
int_fast16_t evaluateenginerqvsking(bool coutscores) {
	int_fast16_t score = 0;

	// Embrace the opponent/white king is near the border and keep the engine/black king close to the white/opponent king:
	int_fast16_t whitekingrow = (whitekingposition >> 4);
	int_fast16_t whitekingcol = (whitekingposition & 15);
	int_fast16_t blackkingrow = (blackkingposition >> 4);
	int_fast16_t blackkingcol = (blackkingposition & 15);

	int_fast16_t distance = (abs(whitekingrow - blackkingrow) > abs(whitekingcol - blackkingcol) ? abs(whitekingrow - blackkingrow) : abs(whitekingcol - blackkingcol));

	if (whitekingrow == 4 || whitekingrow == 11 || whitekingcol == 4 || whitekingcol == 11) {
		score = 1000 - 10 * distance;
	} else if (whitekingrow == 5 || whitekingrow == 10 || whitekingcol == 5 || whitekingcol == 10) {
		score = 900 - 10 * distance;
	} else if (whitekingrow == 6 || whitekingrow == 9 || whitekingcol == 6 || whitekingcol == 9) {
		score = 800 - 10 * distance;
	} else {
		score = 700 - 10 * distance;
	}

	if (coutscores) { std::cout << "ERQ:" << std::to_string(score); }

	return score;
}

// Engine has nothing but a sole king. Opponent has no pawns, but have a queen, a rook, a bishop and/or a knight.
// Engine goal is take opponent pieces and keep the sole king in the center.
int_fast16_t evaluateengingkingvsnopawns(bool coutscores) {
	int score = 0;

	for (int_fast16_t f = 68; f <= 187; ) {
		score -= board[f];
		if ((++f & 15) == 12) {
			f += 8;
		}
	}

	int_fast16_t enginekingrow = (blackkingposition >> 4);
	int_fast16_t enginekingcol = (blackkingposition & 15);

	// Engine king move to the center:
	score += -50 * std::max(std::abs(std::min(enginekingcol - 7, enginekingcol - 8)), std::abs(std::min(enginekingrow - 7, enginekingrow - 8)));

	if (coutscores) { std::cout << "ONP:" << std::to_string(score); }

	return score;
}

#define SCORE_BISHOP_PAIR 20
#define SCORE_ISOLATED_PAWN -20
#define SCORE_DOUBLE_PAWN -20
#define SCORE_ROOKS_ON_OPEN_FILE 6

// The evaulator to use if not in one of the special cases above.
// Evaluation is based on two concepts: (1) material and positions, and (2) structure (bishop pair, double pawns, isolated pawns, pass pawns
// connected rooks, ...)
int_fast16_t evaluatedefault(bool coutscores) {
	int_fast16_t matposscore = 0;
	int_fast16_t structurescore = 0;

	int_fast16_t whitebishopcount = 0;
	int_fast16_t blackbishopcount = 0;
	int_fast16_t whitepawncount[8];
	int_fast16_t blackpawncount[8];
	int_fast16_t whiterookcount[8];
	int_fast16_t blackrookcount[8];

	for (int_fast16_t c = 7; c >= 0; c--) {
		whitepawncount[c] = 0;
		blackpawncount[c] = 0;
		whiterookcount[c] = 0;
		blackrookcount[c] = 0;
	}

	for (int_fast16_t f = 68; f <= 187; ) {
		matposscore -= board[f];
		switch (board[f]) {
			case EMPTY:
				break;
			case WHITE_PAWN:
				matposscore -= pawnposscores[f];
				whitepawncount[(f & 15) - 4]++;
				break;
			case BLACK_PAWN:
				matposscore += pawnposscoresblack[f];
				blackpawncount[(f & 15) - 4]++;
				break;
			case WHITE_KNIGHT:
				matposscore -= knightposscores[f];
				break;
			case BLACK_KNIGHT:
				matposscore += knightposscoresblack[f];
				break;
			case WHITE_BISHOP:
				matposscore -= bishopposscores[f];
				whitebishopcount++;
				break;
			case BLACK_BISHOP:
				matposscore += bishopposscoresblack[f];
				blackbishopcount++;
				break;
			case WHITE_ROOK:
				matposscore -= rookposscores[f];
				whiterookcount[(f & 15) - 4]++;
				break;
			case BLACK_ROOK:
				matposscore += rookposscoresblack[f];
				blackrookcount[(f & 15) - 4]++;
				break;
			case WHITE_QUEEN:
				matposscore -= queenposscores[f];
				break;
			case BLACK_QUEEN:
				matposscore += queenposscoresblack[f];
				break;
			case WHITE_KING:
				matposscore -= kingposscores[f];
				break;
			case BLACK_KING:
				matposscore += kingposscoresblack[f];
				break;
		}
		if ((++f & 15) == 12) {
			f += 8;
		}
	}

	int_fast16_t whiteisolatedpawns = 0;
	int_fast16_t blackisolatedpawns = 0;
	int_fast16_t whitedoubledpawns = 0;
	int_fast16_t blackdoubledpawns = 0;

	int_fast16_t whiterooksonopenfile = 0;
	int_fast16_t blackrooksonopenfile = 0;

	for (int_fast16_t c = 7; c >= 0; c--) {
		if (whitepawncount[c] >= 2) {
			whitedoubledpawns += whitepawncount[c] - 1;
		}
		if (blackpawncount[c] >= 2) {
			blackdoubledpawns += blackpawncount[c] - 1;
		}
		if (whitepawncount[c] >= 1
				&& (c == 0 && whitepawncount[c + 1] == 0 || c == 7 && whitepawncount[c - 1] == 0 || c > 0 && c < 7 && whitepawncount[c - 1] == 0 && whitepawncount[c + 1] == 0)) {
			whiteisolatedpawns++;
		}
		if (blackpawncount[c] >= 1
				&& (c == 0 && blackpawncount[c + 1] == 0 || c == 7 && blackpawncount[c - 1] == 0 || c > 0 && c < 7 && blackpawncount[c - 1] == 0 && blackpawncount[c + 1] == 0)) {
			blackisolatedpawns++;
		}
		if (whiterookcount[c] >= 1 && blackpawncount[c] == 0) {
			whiterooksonopenfile += whiterookcount[c] * (whitepawncount[c] == 0 ? 2 : 1);
		}
		if (blackrookcount[c] >= 1 && whitepawncount[c] == 0) {
			blackrooksonopenfile += blackrookcount[c] * (blackpawncount[c] == 0 ? 2 : 1);
		}
	}

	// https://www.chessprogramming.org/Bishop_Pair
	structurescore += SCORE_BISHOP_PAIR * ((blackbishopcount == 2 ? 1 : 0) - (whitebishopcount == 2 ? 1 : 0));

	// https://www.chessprogramming.org/Isolated_Pawn
	structurescore += SCORE_ISOLATED_PAWN * (blackisolatedpawns - whiteisolatedpawns);

	// https://www.chessprogramming.org/Doubled_Pawn
	structurescore += SCORE_DOUBLE_PAWN * (blackdoubledpawns - whitedoubledpawns);

	// https://www.chessprogramming.org/Rook_on_Open_File
	structurescore += SCORE_ROOKS_ON_OPEN_FILE * (blackrooksonopenfile - whiterooksonopenfile);

	if (coutscores) { std::cout << std::to_string(matposscore) << " / " << std::to_string(structurescore); }

	return matposscore + structurescore;
}

// ----------------------------------------------------------------------------------------------------------
// MOVE GENERATOR
// ----------------------------------------------------------------------------------------------------------

// Do a move.
void domove(int_fast16_t depth, int_fast16_t n) {
	switch (moves[depth][n].type) {
		case MOVE_TYPE_NORMAL:
			moves[depth][n].piecetaken = board[moves[depth][n].to];
			board[moves[depth][n].to] = board[moves[depth][n].from];
			board[moves[depth][n].from] = EMPTY;
			if (board[moves[depth][n].to] == WHITE_KING) {
				whitekingposition = moves[depth][n].to;
			}
			return;
		case MOVE_TYPE_SHORT_CASTLING:
			board[72] = EMPTY; board[73] = WHITE_ROOK; board[74] = WHITE_KING; board[75] = EMPTY;
			whitekingposition = 74;
			return;
		case MOVE_TYPE_LONG_CASTLING:
			board[68] = EMPTY; board[69] = EMPTY; board[70] = WHITE_KING; board[71] = WHITE_ROOK; board[72] = EMPTY;
			whitekingposition = 70;
			return;
		case MOVE_TYPE_QUEEN_PROMOTION:
			moves[depth][n].piecetaken = board[moves[depth][n].to];
			board[moves[depth][n].to] = WHITE_QUEEN;
			board[moves[depth][n].from] = 0;
			return;
		case MOVE_TYPE_EN_PASSANT:
			board[moves[depth][n].to] = WHITE_PAWN;
			board[moves[depth][n].from] = EMPTY;
			board[moves[depth][n].to - 16] = EMPTY;
			return;
	}
}

// Undo a move.
void undomove(int_fast16_t depth, int_fast16_t n) {
	switch (moves[depth][n].type) {
		case MOVE_TYPE_NORMAL:
			if (board[moves[depth][n].to] == WHITE_KING) {
				whitekingposition = moves[depth][n].from;
			}
			board[moves[depth][n].from] = board[moves[depth][n].to];
			board[moves[depth][n].to] = moves[depth][n].piecetaken;
			return;
		case MOVE_TYPE_SHORT_CASTLING:
			board[72] = WHITE_KING; board[73] = EMPTY; board[74] = EMPTY; board[75] = WHITE_ROOK;
			whitekingposition = 72;
			return;
		case MOVE_TYPE_LONG_CASTLING:
			board[68] = WHITE_ROOK; board[69] = EMPTY; board[70] = EMPTY; board[71] = EMPTY; board[72] = WHITE_KING;
			whitekingposition = 72;
			return;
		case MOVE_TYPE_QUEEN_PROMOTION:
			board[moves[depth][n].from] = WHITE_PAWN;
			board[moves[depth][n].to] = moves[depth][n].piecetaken;
			return;
		case MOVE_TYPE_EN_PASSANT:
			board[moves[depth][n].from] = WHITE_PAWN;
			board[moves[depth][n].to] = EMPTY;
			board[moves[depth][n].to - 16] = BLACK_PAWN;
			return;
	}
}

void emptyboard() {
	for (int_fast16_t r = 15; r >= 0; r--) {
		for (int_fast16_t c = 15; c >= 0; c--) {
			if (r < 4 || r > 11 || c < 4 || c > 11) {
				board[(r << 4) + c] = BORDER;
			} else {
				board[(r << 4) + c] = EMPTY;
			}
		}
	}
}

void flipboard() {
	for (int_fast16_t row = 4; row <= 7; row++) {
		for (int_fast16_t col = 4; col <= 11; col++) {
			int_fast16_t temp = board[(row << 4) + col];
			board[(row << 4) + col] = -board[((15 - row) << 4) + col];
			board[((15 - row) << 4) + col] = -temp;
			if (board[(row << 4) + col] == WHITE_KING) {
				whitekingposition = (row << 4) + col;
			} else if (board[((15 - row) << 4) + col] == WHITE_KING) {
				whitekingposition = ((15 - row) << 4) + col;
			} else if (board[(row << 4) + col] == BLACK_KING) {
				blackkingposition = (row << 4) + col;
			} else if (board[((15 - row) << 4) + col] == BLACK_KING) {
				blackkingposition = ((15 - row) << 4) + col;
			}
		}
	}
	whitetomove = !whitetomove;
}

// Sort moves.
// Selection sort is used - https://en.wikipedia.org/wiki/Selection_sort
void sortmoves(int_fast16_t depth, bool bestfirst) {
	if (nmoves[depth] > 1) {
		for (int_fast16_t n = 0; n < nmoves[depth] - 1; n++) {
			int_fast16_t imax = n;
			for (int_fast16_t i = n + 1; i < nmoves[depth]; i++) {
				if (bestfirst && moves[depth][i].score > moves[depth][imax].score || !bestfirst && moves[depth][i].score < moves[depth][imax].score) {
					imax = i;
				}
			}
			if (imax != n) {
				struct movestruct tempmove = moves[depth][n];
				moves[depth][n] = moves[depth][imax];
				moves[depth][imax] = tempmove;
			}
		}
	}
}

// Return true if and only if field identified by 'row' and 'col' is in check by a black piece.
bool ischeck(int_fast16_t f) {
	if (board[f+15] == BLACK_PAWN || board[f+17] == BLACK_PAWN) {
		return true;
	}

	if (board[f+31] == BLACK_KNIGHT || board[f+33] == BLACK_KNIGHT || board[f+14] == BLACK_KNIGHT || board[f+18] == BLACK_KNIGHT
		|| board[f-14] == BLACK_KNIGHT || board[f-18] == BLACK_KNIGHT || board[f-31] == BLACK_KNIGHT || board[f-33] == BLACK_KNIGHT) {
		return true;
	}

	int_fast16_t g = f + 16;
	while (board[g] == EMPTY) {
		g += 16;
	}
	if (board[g] == BLACK_ROOK || board[g] == BLACK_QUEEN) {
		return true;
	}
	g = f - 16;
	while (board[g] == EMPTY) {
		g -= 16;
	}
	if (board[g] == BLACK_ROOK || board[g] == BLACK_QUEEN) {
		return true;
	}
	g = f + 1;
	while (board[g] == EMPTY) {
		g++;
	}
	if (board[g] == BLACK_ROOK || board[g] == BLACK_QUEEN) {
		return true;
	}
	g = f - 1;
	while (board[g] == EMPTY) {
		g--;
	}
	if (board[g] == BLACK_ROOK || board[g] == BLACK_QUEEN) {
		return true;
	}
	g = f + 17;
	while (board[g] == EMPTY) {
		g += 17;
	}
	if (board[g] == BLACK_BISHOP || board[g] == BLACK_QUEEN) {
		return true;
	}
	g = f + 15;
	while (board[g] == EMPTY) {
		g += 15;
	}
	if (board[g] == BLACK_BISHOP || board[g] == BLACK_QUEEN) {
		return true;
	}
	g = f - 17;
	while (board[g] == EMPTY) {
		g -= 17;
	}
	if (board[g] == BLACK_BISHOP || board[g] == BLACK_QUEEN) {
		return true;
	}
	g = f - 15;
	while (board[g] == EMPTY) {
		g -= 15;
	}
	if (board[g] == BLACK_BISHOP || board[g] == BLACK_QUEEN) {
		return true;
	}

	if (board[f+15] == BLACK_KING || board[f+16] == BLACK_KING || board[f+17] == BLACK_KING || board[f-1] == BLACK_KING
		|| board[f+1] == BLACK_KING || board[f-15] == BLACK_KING || board[f-16] == BLACK_KING || board[f-17] == BLACK_KING) {
		return true;
	}

	return false;
}

// Add a move to the moves in the array moves[depth].
// There are one other addmove functions (addpawnmove) below.
// These two addmove functions could be joined with some if-statements, but it is a little
// faster to have two separate functions.
void addmove(int_fast16_t depth, int_fast16_t from, int_fast16_t to) {
	moves[depth][nmoves[depth]].from = from;
	moves[depth][nmoves[depth]].to = to;
	moves[depth][nmoves[depth]].piecetaken = board[to];
	moves[depth][nmoves[depth]].type = MOVE_TYPE_NORMAL;
	domove(depth, nmoves[depth]);
	bool check = ischeck(whitekingposition);
	undomove(depth, nmoves[depth]);
	if (!check) {
		nmoves[depth]++;
	}
}

// Add a pawn move to the moves in the array moves[depth].
void addpawnmove(int_fast16_t depth, int_fast16_t from, int_fast16_t to) {
	if (to < 176) {
		addmove(depth, from, to);
		return;
	}
	moves[depth][nmoves[depth]].from = from;
	moves[depth][nmoves[depth]].to = to;
	moves[depth][nmoves[depth]].piecetaken = board[to];
	moves[depth][nmoves[depth]].type = MOVE_TYPE_QUEEN_PROMOTION;
	domove(depth, nmoves[depth]);
	bool check = ischeck(whitekingposition);
	undomove(depth, nmoves[depth]);
	if (!check) {
		nmoves[depth]++;
	}
}

// Generate possible white moves from the board.
// Castling moves and en passant moves are not found. Use generatespecialmoves to add these.
void generatemoves(int_fast16_t depth) {
	nmoves[depth] = 0;
	int_fast16_t c;
	for (int_fast16_t r = 4; r <= 11; r++) {
		for (int_fast16_t c = 4; c <= 11; c++) {
			int_fast16_t f = (r << 4) + c;
			switch (board[f]) {
				case EMPTY:
					break;

				case WHITE_PAWN:
					if (board[f+16] == 0) {
						addpawnmove(depth, f, f+16);
						if (r == 5 && board[f+32] == 0) {
							addmove(depth, f, f+32);
						}
					}
					if (board[f+15] < 0) {
						addpawnmove(depth, f, f+15);
					}
					if (board[f+17] < 0) {
						addpawnmove(depth, f, f+17);
					}
					break;

				case WHITE_KNIGHT:
					if (board[f+31] <= 0) {
						addmove(depth, f, f+31);
					}
					if (board[f+33] <= 0) {
						addmove(depth, f, f+33);
					}
					if (board[f+14] <= 0) {
						addmove(depth, f, f+14);
					}
					if (board[f+18] <= 0) {
						addmove(depth, f, f+18);
					}
					if (board[f-18] <= 0) {
						addmove(depth, f, f-18);
					}
					if (board[f-14] <= 0) {
						addmove(depth, f, f-14);
					}
					if (board[f-33] <= 0) {
						addmove(depth, f, f-33);
					}
					if (board[f-31] <= 0) {
						addmove(depth, f, f-31);
					}
					break;

				case WHITE_BISHOP:
				case WHITE_QUEEN:
					for (int_fast16_t g = f + 17; board[g] <= 0; g += 17) {
						addmove(depth, f, g);
						if (board[g] < 0) {
							break;
						}
					}
					for (int_fast16_t g = f + 15; board[g] <= 0; g += 15) {
						addmove(depth, f, g);
						if (board[g] < 0) {
							break;
						}
					}
					for (int_fast16_t g = f - 17; board[g] <= 0; g -= 17) {
						addmove(depth, f, g);
						if (board[g] < 0) {
							break;
						}
					}
					for (int_fast16_t g = f - 15; board[g] <= 0; g -= 15) {
						addmove(depth, f, g);
						if (board[g] < 0) {
							break;
						}
					}

					if (board[f] == WHITE_BISHOP) {
						break;
					}

				case WHITE_ROOK:
					for (int_fast16_t g = f - 1; board[g] <= 0; g--) {
						addmove(depth, f, g);
						if (board[g] < 0) {
							break;
						}
					}
					for (int_fast16_t g = f + 1; board[g] <= 0; g++) {
						addmove(depth, f, g);
						if (board[g] < 0) {
							break;
						}
					}
					for (int_fast16_t g = f - 16; board[g] <= 0; g -= 16) {
						addmove(depth, f, g);
						if (board[g] < 0) {
							break;
						}
					}
					for (int_fast16_t g = f + 16; board[g] <= 0; g += 16) {
						addmove(depth, f, g);
						if (board[g] < 0) {
							break;
						}
					}
					break;

				case WHITE_KING:
					if (board[f+15] <= 0) {
						addmove(depth, f, f+15);
					}
					if (board[f+16] <= 0) {
						addmove(depth, f, f+16);
					}
					if (board[f+17] <= 0) {
						addmove(depth, f, f+17);
					}
					if (board[f+1] <= 0) {
						addmove(depth, f, f+1);
					}
					if (board[f-1] <= 0) {
						addmove(depth, f, f-1);
					}
					if (board[f-15] <= 0) {
						addmove(depth, f, f-15);
					}
					if (board[f-16] <= 0) {
						addmove(depth, f, f-16);
					}
					if (board[f-17] <= 0) {
						addmove(depth, f, f-17);
					}
					break;
			}
		}
	}
}

// Generate castling and en passant moves if possible.
// The moves are added to the end of moves[depth].
void generatespecialmoves(int_fast16_t depth, bool boardflipped) {
	if (board[72] == WHITE_KING && !ischeck(72)) {
		if ((!boardflipped && castlingpossible.whiteking || boardflipped && castlingpossible.blackking)
				&& board[73] == EMPTY && !ischeck(73) && board[74] == EMPTY && !ischeck(74) && board[75] == WHITE_ROOK) {
			addmove(depth, 72, 74);
			moves[depth][nmoves[depth]-1].type = MOVE_TYPE_SHORT_CASTLING;
		}
		if ((!boardflipped && castlingpossible.whitequeen || boardflipped && castlingpossible.blackqueen)
				&& board[71] == EMPTY && !ischeck(71) && board[70] == EMPTY && !ischeck(70) && board[69] == EMPTY && board[68] == WHITE_ROOK) {
			addmove(depth, 72, 70);
			moves[depth][nmoves[depth]-1].type = MOVE_TYPE_LONG_CASTLING;
		}
	}

	if (enpassantposition != 0) {
		int_fast16_t epposition = (boardflipped ? (15 - (enpassantposition >> 4)) * 16 + (enpassantposition & 15) : enpassantposition);
		if (board[epposition - 15] == WHITE_PAWN || board[epposition - 17] == WHITE_PAWN) {
			moves[depth][nmoves[depth]].from = epposition - (board[epposition - 15] == WHITE_PAWN ? 15 : 17);
			moves[depth][nmoves[depth]].to = epposition;
			moves[depth][nmoves[depth]].type = MOVE_TYPE_EN_PASSANT;
			domove(depth, nmoves[depth]);
			bool check = ischeck(whitekingposition);
			undomove(depth, nmoves[depth]);
			if (!check) {
				nmoves[depth]++;
			}
		}
	}
}

// -------------------------------------------------------------------------------------------------------
// MINIMAX WITH ALPHA-BETA PRUNING
// See https://www.chessprogramming.org/Alpha-Beta
// Parameter 'basescore' is added to add a score of the board or move which should not be calculated for each node.
// Parameter 'evaulate' is added to specify which evaluation function to uses.
// Function 'alphabetaentry' is a version of 'alphabetamin' where a check for threefold repetition is made.
// 'alphabetaentry' should be called when finding best move.
// Globale variable 'nodecount' will hold the number of nodes/positions evaluated.
// -------------------------------------------------------------------------------------------------------

uint_fast32_t nodecount;

int_fast16_t alphabetamin(int_fast16_t depth, int_fast16_t alpha, int_fast16_t beta, int_fast16_t basescore, std::function<int_fast16_t(bool)> evaluate);

int_fast16_t alphabetamax(int_fast16_t depth, int_fast16_t alpha, int_fast16_t beta, int_fast16_t basescore, std::function<int_fast16_t(bool)> evaluate) {
	if (depth == 0) {
		nodecount++;
		return basescore + evaluate(false);
	}
	flipboard();
	generatemoves(depth);  // Engine moves
	if (nmoves[depth] == 0) {  // Engine has no moves
		int_fast16_t score = (ischeck(whitekingposition) ? - SCORE_INFINITE : SCORE_STALEMATE_ENGINE_NO_MOVES);
		flipboard();
		return score;
	}
	int_fast16_t bestvalue = alpha;
	for (int_fast16_t n = nmoves[depth] - 1; n >= 0; n--) {
		domove(depth, n);
		int_fast16_t value = alphabetamin(depth - 1, bestvalue, beta, basescore, evaluate);
		undomove(depth, n);
		if (value > bestvalue) {
			bestvalue = value;
		}
		if (beta <= bestvalue) {
			break;
		}
	}
	flipboard();
	return bestvalue;
}

int_fast16_t alphabetamin(int_fast16_t depth, int_fast16_t alpha, int_fast16_t beta, int_fast16_t basescore, std::function<int_fast16_t(bool)> evaluate) {
	// depth will never be 0 if start depth is even, so don't waste time testing for depth is 0.
	flipboard();
	generatemoves(depth);  // Opponent moves
	if (nmoves[depth] == 0) {  // Opponent has no moves
		int_fast16_t score = (ischeck(whitekingposition) ? SCORE_INFINITE : SCORE_STALEMATE_OPPONENT_NO_MOVES);
		flipboard();
		return score;
	}
	int_fast16_t bestvalue = beta;
	for (int_fast16_t n = nmoves[depth] - 1; n >= 0; n--) {
		domove(depth, n);
		int_fast16_t value = alphabetamax(depth - 1, alpha, bestvalue, basescore, evaluate);
		undomove(depth, n);
		if (value < bestvalue) {
			bestvalue = value;
		}
		if (alpha >= bestvalue) {
			break;
		}
	}
	flipboard();
	return bestvalue;
}

int_fast16_t alphabetaentry(int_fast16_t depth, int_fast16_t alpha, int_fast16_t beta, int_fast16_t basescore, std::function<int_fast16_t(bool)> evaluate, bool boardflipped) {
	// depth will never be 0 if start depth is even, so don't waste time testing for depth is 0.
	flipboard();
	generatemoves(depth);  // Opponent moves
	if (nmoves[depth] == 0) {  // Opponent has no moves
		int_fast16_t score = (ischeck(whitekingposition) ? SCORE_INFINITE : SCORE_STALEMATE_OPPONENT_NO_MOVES);
		flipboard();
		return score;
	}
	int_fast16_t bestvalue = beta;
	for (int_fast16_t n = nmoves[depth] - 1; n >= 0; n--) {
		domove(depth, n);
		int_fast16_t value;
		if (counthash(gethash(!boardflipped)) >= 2) {
			value = SCORE_STALEMATE_OPPONENT_REPETITION;
		} else {
			value = alphabetamax(depth - 1, alpha, bestvalue, basescore, evaluate);
		}
		undomove(depth, n);
		if (value < bestvalue) {
			bestvalue = value;
		}
		if (alpha >= bestvalue) {
			break;
		}
	}
	flipboard();
	return bestvalue;
}

// --------------------------------------------------------------------------------------------------------------
// FIND BEST MOVE
// This is the function finding the best move by use of Minimax with alpha-beta pruning algorithm.
// Parameter 'depth' is the depth to search.
// Parameter 'mode' is the search mode to use. See below.
// Parameter 'state' is the game state. The evaluation function in the state is used in the Minimax algorithm.
// --------------------------------------------------------------------------------------------------------------

enum searchmode {
	SEARCH_MODE_INITIAL, // Legal engine moves are generated and a mate in deptt is searched
	SEARCH_MODE_UPDATE,  // The generated moves are sorted and a mate in depth is searched
	SEARCH_MODE_FINAL    // The final search for best move
};

// Find the best move.
int_fast16_t findbestmove(int_fast16_t depth, searchmode mode, statestruct state) {
	bool boardflipped = false;
	if (!whitetomove) {
		flipboard();
		boardflipped = true;
	}

	if (mode == SEARCH_MODE_INITIAL) {
		// Generate all engine/white moves:
		generatemoves(0);
		generatespecialmoves(0, boardflipped);
	} else {
		sortmoves(0, false);  // Worst first because the move counter starts with last move
	}

	infostring = "";

	nodecount = 0;
    auto starttime = std::chrono::high_resolution_clock::now();
    int_fast16_t alpha = - SCORE_INFINITE - 100;
	int_fast16_t bestn;

	for (int_fast16_t n = nmoves[0] - 1; n >= 0; n--) {
		domove(0, n);

		if (counthash(gethash(boardflipped)) >= 2) {
			moves[0][n].score = SCORE_STALEMATE_ENGINE_REPETITION;
		} else {
			moves[0][n].score = alphabetaentry(depth - 1, (mode == SEARCH_MODE_FINAL ? alpha : - SCORE_INFINITE), SCORE_INFINITE, evaluatebase(boardflipped, state) + evaluatemove(moves[0][n]), state.evaluate, boardflipped);
		}

		undomove(0, n);

		if (moves[0][n].score > alpha) {
			alpha = moves[0][n].score;
			bestn = n;
			if (alpha > SCORE_INFINITE - 100) {
				break;
			}
		}
	}

    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - starttime);

    std::string nps = std::to_string(static_cast<uint_fast32_t>(nodecount / (time.count() == 0 ? 0.001 : time.count() * 0.001)));

	if (alpha > SCORE_INFINITE - 100) {
		std::cout << "info depth " << std::to_string(depth) << " score mate " << std::to_string(depth / 2) << " nodes " << std::to_string(nodecount) << " nps " << nps << " pv " << movestring(moves[0][bestn], boardflipped, false) << std::endl;
	} else {
		std::cout << "info depth " << std::to_string(depth) << " score cp " << std::to_string(alpha) << " nodes " << std::to_string(nodecount) << " nps " << nps << " pv " << movestring(moves[0][bestn], boardflipped, false) << std::endl;
	}

	if (infostring.length() > 0) {
		std::cout << "info string " << infostring << std::endl;
	}

	if (boardflipped) {
		flipboard();
	}

	return bestn;
}

// -------------------------------------------------------------------------------------------------------
// SET BOARD / CASTLING / ENPASSANT
// Functions which takes a token and sets globale game variables based on the token
// -------------------------------------------------------------------------------------------------------

void setboard(std::string token /* this is a fen */) {
	emptyboard();
	int_fast16_t row = 11;
	int_fast16_t col = 4;
	for (int_fast16_t i = 0; i < token.size(); i++) {
		switch (token[i]) {
			case 'P': board[(row << 4) + col] = WHITE_PAWN; col++; break;
			case 'N': board[(row << 4) + col] = WHITE_KNIGHT; col++; break;
			case 'B': board[(row << 4) + col] = WHITE_BISHOP; col++; break;
			case 'R': board[(row << 4) + col] = WHITE_ROOK; col++; break;
			case 'Q': board[(row << 4) + col] = WHITE_QUEEN; col++; break;
			case 'K': board[(row << 4) + col] = WHITE_KING; whitekingposition = (row << 4) + col; col++; break;
			case 'p': board[(row << 4) + col] = BLACK_PAWN; col++; break;
			case 'n': board[(row << 4) + col] = BLACK_KNIGHT; col++; break;
			case 'b': board[(row << 4) + col] = BLACK_BISHOP; col++; break;
			case 'r': board[(row << 4) + col] = BLACK_ROOK; col++; break;
			case 'q': board[(row << 4) + col] = BLACK_QUEEN; col++; break;
			case 'k': board[(row << 4) + col] = BLACK_KING; blackkingposition = (row << 4) + col; col++; break;
			case '/': row--; col = 4; break;
			default: col += token[i] - '0';
		}
	}
}

void setcastlingpossible(std::string token) {
	castlingpossible.whiteking = (token.find("K") != std::string::npos);
	castlingpossible.whitequeen = (token.find("Q") != std::string::npos);
	castlingpossible.blackking = (token.find("k") != std::string::npos);
	castlingpossible.blackqueen = (token.find("q") != std::string::npos);
}

void setenpassantposition(std::string token) {
	enpassantposition = (token == "-" ? 0 : ((token[1] - '1' + 4) << 4) + token[0] - 'a' + 4);
}

void startposition() {
	setboard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	setcastlingpossible("KQkq");
	whitetomove = true;
	enpassantposition = 0;
	moverandom = RANDOM_MAX;
	boardhistory.clear();
	boardhistory.push_back(gethash(false));
}

// -------------------------------------------------------------------------------------------------------
// EVERYTHING ELSE
// -------------------------------------------------------------------------------------------------------

void uciok() {
	printf("id name CederChess 2-SNAPSHOT\n");
	printf("id author Claus Cederberg\n");
	printf("uciok\n");
}

// Returns the first field position of the piece 'piece'.
int_fast16_t findpieceposition(int_fast16_t piece) {
	for (int_fast16_t row = 4; row <= 11; row++) {
		for (int_fast16_t col = 4; col <= 11; col++) {
			if (board[(row << 4) + col] == piece) {
				return (row << 4) + col;
			}
		}
	}
	return -1;
}

// Perform moves.
void performmoves(std::vector<std::string> &tokens, uint_fast16_t skiptokens) {
    for (const std::string &move : tokens) {
    	if (skiptokens > 0) {
    		skiptokens--;
    		continue;
    	}

		enpassantposition = 0;

		int_fast16_t from = ((move[1] - '1' + 4) << 4) + move[0] - 'a' + 4;
		int_fast16_t to = ((move[3] - '1' + 4) << 4) + move[2] - 'a' + 4;
		int_fast16_t piecetaken = board[to];

		board[to] = board[from];
		board[from] = EMPTY;

		if (board[to] == WHITE_PAWN && to > 176 || board[to] == BLACK_PAWN && to < 80) {
			// Pawn Promotion
			bool white = (board[to] == WHITE_PAWN);
			switch (move[4]) {
				case 'Q': case 'q': board[to] = (white ? WHITE_QUEEN : BLACK_QUEEN); break;
				case 'R': case 'r': board[to] = (white ? WHITE_ROOK : BLACK_ROOK); break;
				case 'B': case 'b': board[to] = (white ? WHITE_BISHOP : BLACK_BISHOP); break;
				case 'N': case 'n': board[to] = (white ? WHITE_KNIGHT : BLACK_KNIGHT); break;
			}
		} else if (board[to] == WHITE_PAWN && piecetaken == EMPTY) {
			if (moves[0] != moves[2]) {
				// white pawn goes to empty field by changing file, this is en passant
				board[to - 16] = EMPTY;
			} else if (from <= 91 && to >= 116) {
				// white pawn goes two fields forward, possible for en passant for black
				enpassantposition = from + 16;
			}
		} else if (board[to] == BLACK_PAWN && piecetaken == EMPTY) {
			if (move[0] != move[2]) {
				// black pawn goes to empty field by changing file, this is en passant
				board[to + 16] = EMPTY;
			} else if (from >= 164 && to <= 139) {
				// black pawn goes two fields forward, possible for en passant for white
				enpassantposition = from - 16;
			}
		} else if (board[to] == WHITE_KING) {
			castlingpossible.whiteking = false;
			castlingpossible.whitequeen = false;
			if (move == "e1g1") {
				// white short castling
				board[75] = EMPTY;
				board[73] = WHITE_ROOK;
			} else if (move == "e1c1") {
				// white long castling
				board[68] = EMPTY;
				board[71] = WHITE_ROOK;
			}
		} else if (board[to] == BLACK_KING) {
			castlingpossible.blackking = false;
			castlingpossible.blackqueen = false;
			if (move == "e8g8") {
				// black short castling
				board[187] = EMPTY;
				board[185] = BLACK_ROOK;
			} else if (move == "e8c8") {
				// black long castling
				board[180] = EMPTY;
				board[183] = BLACK_ROOK;
			}
		}

		if (move[0] == 'a' && move[1] == '1') {
			castlingpossible.whitequeen = false;
		} else if (move[0] == 'h' && move[1] == '1') {
			castlingpossible.whiteking = false;
		} else if (move[0] == 'a' && move[1] == '8') {
			castlingpossible.blackqueen = false;
		} else if (move[0] == 'h' && move[1] == '8') {
			castlingpossible.blackking = false;
		}

		boardhistory.push_back(gethash(false));

		whitetomove = !whitetomove;

		if (moverandom > RANDOM_MIN) {
			moverandom -= (boardhistory.size() % 2 == 0 ? RANDOM_DEC : 0);
		}
	}
}

std::vector<std::string> tokenize(std::string line) {
	std::vector<std::string> tokens;
	std::stringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void generatestatemap() {
	statemap.clear();
	statemap.insert(std::pair<stateenum, statestruct>(STATE_START_GAME, statestruct(STATE_START_GAME, "Start Game", DEPTH_DEFAULT, evaluatedefault)));
	statemap.insert(std::pair<stateenum, statestruct>(STATE_MIDDLE_GAME, statestruct(STATE_MIDDLE_GAME, "Middle Game", DEPTH_DEFAULT, evaluatedefault)));
	statemap.insert(std::pair<stateenum, statestruct>(STATE_LATE_MIDDLE_GAME, statestruct(STATE_LATE_MIDDLE_GAME, "Late Middle Game", DEPTH_DEFAULT, evaluatedefault)));
	statemap.insert(std::pair<stateenum, statestruct>(STATE_END_GAME, statestruct(STATE_END_GAME, "End Game", DEPTH_DEFAULT + 2, evaluatedefault)));
	statemap.insert(std::pair<stateenum, statestruct>(STATE_PAWN_END_GAME, statestruct(STATE_PAWN_END_GAME, "Pawn End Game", DEPTH_DEFAULT + 2, evaluatedefault)));
	statemap.insert(std::pair<stateenum, statestruct>(STATE_PAWN_ONLY_END_GAME, statestruct(STATE_PAWN_ONLY_END_GAME, "Pawn Only End Game", DEPTH_DEFAULT + 4, evaluatedefault)));
	statemap.insert(std::pair<stateenum, statestruct>(STATE_ENGINE_PAWN_VS_KING, statestruct(STATE_ENGINE_PAWN_VS_KING, "Engine Pawn vs King", DEPTH_DEFAULT + 4, evaluateenginepawnvsking)));
	statemap.insert(std::pair<stateenum, statestruct>(STATE_ENGINE_KING_VS_PAWN, statestruct(STATE_ENGINE_KING_VS_PAWN, "Engine King vs Pawn", DEPTH_DEFAULT + 4, evaluateenginekingvspawn)));
	statemap.insert(std::pair<stateenum, statestruct>(STATE_ENGINE_RQ_VS_KING, statestruct(STATE_ENGINE_RQ_VS_KING, "Engine Rook/Queen vs King", DEPTH_DEFAULT + 2, evaluateenginerqvsking)));
	statemap.insert(std::pair<stateenum, statestruct>(STATE_ENGINE_KING_VS_NOPAWNS, statestruct(STATE_ENGINE_KING_VS_NOPAWNS, "Engine King vs No Pawns", DEPTH_DEFAULT + 2, evaluateengingkingvsnopawns)));
}

// -------------------------------------------------------------------------------------------------------
// MAIN
// -------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
	if (DEPTH_DEFAULT != 2 && DEPTH_DEFAULT != 4 && DEPTH_DEFAULT != 6 && DEPTH_DEFAULT != 8) {
		printf("DEPTH_DEFAULT should be either 2, 4, 6 or 8\n");
		return 1;
	}

	generatehashkeys();
	generatestatemap();

	// Initialize random number generator to add a small random number to initial score of move.
	// This is done so the engine not always take the same moves when moves are equal in score.
	randomgenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	startposition();
	uciok();

	std::string line;

	// Loop asking for UCI commands in stdin and output result to stdout. Loop is stopped by the command quit.
	// See http://wbec-ridderkerk.nl/html/UCIProtocol.html
	while (true) {
		std::getline(std::cin, line);
		std::vector<std::string> tokens = tokenize(line);

		if (tokens.empty()) {
			continue;
		}

		if (tokens[0] == "uci") {
			uciok();
			continue;
		}

		if (tokens[0] == "isready") {
			std::cout << "readyok" << std::endl;
			continue;
		}

		if (tokens[0] == "position") {
			startposition();

		    int_fast16_t tokenno = -1;
		    int_fast16_t fentokenno = -1;
		    for (const std::string &token : tokens) {
		    	tokenno++;
		    	// Ignore token "startpos" because startposition() has already been called
		    	if (token == "fen") {
					fentokenno = tokenno;
					moverandom = 0;
				} else if (fentokenno >= 0 && tokenno - fentokenno == 1) {  // first token after token "fen"
					setboard(token);
					boardhistory.clear();
					boardhistory.push_back(gethash(false));
				} else if (fentokenno >= 0 && tokenno - fentokenno == 2) {  // second token after token "fen"
					whitetomove = (token == "w");
				} else if (fentokenno >= 0 && tokenno - fentokenno == 3) {  // third token after token "fen"
					setcastlingpossible(token);
				} else if (fentokenno >= 0 && tokenno - fentokenno == 4) {  // fourth token after token "fen"
					setenpassantposition(token);
					// Ignore token 5 (halfmove clock) and token 6 (fullmove number)
				} else if (token == "moves") {
					performmoves(tokens, tokenno + 1);
					break;
				}
		    }

			whitekingposition = findpieceposition(WHITE_KING);
			blackkingposition = findpieceposition(BLACK_KING);

			continue;
		}

		if (tokens[0] == "go") {
			statestruct state = getstate();
			int_fast16_t depthmax = state.defaultdepth;

		    int_fast16_t tokenno = -1;
		    int_fast16_t wtimetokenno = -2;
		    int_fast16_t btimetokenno = -2;
		    for (const std::string &token : tokens) {
		    	tokenno++;
		    	if (token == "wtime") {
		    		wtimetokenno = tokenno;
		    	} else if (token == "btime") {
		    		btimetokenno = tokenno;
		    	} else if (tokenno == wtimetokenno + 1 && whitetomove || tokenno == btimetokenno + 1 && !whitetomove) {
		    		if (stoi(token) < TIME_LIMIT_DEC_DEPTH) {
		    			depthmax = depthmax - 2;
		    			break;
		    		}
		    		if (stoi(token) > TIME_LIMIT_INC_DEPTH) {
		    			depthmax = depthmax + 2;
		    			break;
		    		}
		    	}
		    }

		    std::cout << "info string moverandom: " << std::to_string(moverandom) << ", state: " << state.name << ", defaultdepth: " << std::to_string(state.defaultdepth) << ", whitetomove: " << std::to_string(whitetomove) << std::endl;

			int_fast16_t bestn;
			int_fast16_t depth = 2;
			while (true) {
				bestn = findbestmove(depth, depth == 2 ? SEARCH_MODE_INITIAL : (depth < depthmax ? SEARCH_MODE_UPDATE : SEARCH_MODE_FINAL), state);
				if (moves[0][bestn].score > SCORE_INFINITE - 1000 || depth == depthmax) {
					break;
				}
				depth += 2;
			}

			std::cout << "bestmove " << movestring(moves[0][bestn], !whitetomove, false) << std::endl;
			continue;
		}

		if (tokens[0] == "quit") {
			return 0;
		}
	}
}
