#include "eval.h"
#include "../core/bits.h"
#include "../core/moveGen.h"
#include "../core/position.h"

#include "math.h"
#include <map>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <iostream>
#include <iomanip>
#include <tuple>
#include <vector>

bool Eval::InsufficientMaterial(Position &p) {
	// if any player has a pawn/queen/rook, there's no draw
	if (p.m_pieces[PAWN_ID] | p.m_pieces[QUEEN_ID] | p.m_pieces[ROOK_ID]) {
		return false;
	}
	// knight + bishop for one side : no draw
	if ((p.m_pieces[KNIGHT_ID] & p.m_colors[COLOR_WHITE]) && (p.m_pieces[BISHOP_ID] & p.m_colors[COLOR_WHITE]))
		return false;
	if ((p.m_pieces[KNIGHT_ID] & p.m_colors[COLOR_BLACK]) && (p.m_pieces[BISHOP_ID] & p.m_colors[COLOR_BLACK]))
		return false;
	return true;
}
