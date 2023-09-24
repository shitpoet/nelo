#include "position.h"
#include "bits.h"
#include "moveGen.h"
#include "hashtables.h"
#include "search.h"

#include "stdlib.h"
#include <bitset>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace bits;

Position::Position() { }

Position::Position(Position& o) {
    m_colors[COLOR_WHITE] = o.m_colors[COLOR_WHITE];
    m_colors[COLOR_BLACK] = o.m_colors[COLOR_BLACK];
    m_pieces[PAWN_ID] = o.m_pieces[PAWN_ID];
    m_pieces[ROOK_ID] = o.m_pieces[ROOK_ID];
    m_pieces[KNIGHT_ID] = o.m_pieces[KNIGHT_ID];
    m_pieces[BISHOP_ID] = o.m_pieces[BISHOP_ID];
    m_pieces[QUEEN_ID] = o.m_pieces[QUEEN_ID];
    m_pieces[KING_ID] = o.m_pieces[KING_ID];
    m_occupancy = o.m_occupancy;
    m_empty = o.m_empty;
    m_enPassantSquare = o.m_enPassantSquare;
    m_castleFlags = o.m_castleFlags;
    m_plyPlayer = o.m_plyPlayer;
    m_history = o.m_history;
}

/**
 * Prints bits of one byte
 */
std::string Position::Debug(int n) {
    std::stringstream ss;

    for (int i=0; i<8; i++) {
        ss << ((n & (1<<i)) >> i) << " ";
    }
    return ss.str();
}

/**
 * Prints the chessboard representation of one U64
 */
std::string Position::Debug(U64 bb) {
    
    std::stringstream ss;
    char pieceIds[64];

    for (int i=0; i<64; i++)
        pieceIds[i] = '.';

    while(bb) {
        U64 ls1b = LS1B(bb);
        int bitIdx = BitScanForward(ls1b);

        if (ls1b)
            pieceIds[bitIdx] = '1';

        ResetLS1B(bb);
    }

    for (int row=7; row>=0; row--) {
        ss << std::setw(2) << row+1 << "  ";

        for (int file=0; file<8; file++) {
            int sq = row*8+file;
            ss << pieceIds[sq] << " ";
        }

        ss << std::endl;
    }
    ss << "    a b c d e f g h" << std::endl;
    return ss.str();
}


/**
 * Prints the chessboard representation of the Position
 */
std::string Position::ToString() const {
    std::stringstream ss;
    char pieceIds[64];

    for (int i=0; i<64; i++)
        pieceIds[i] = '.';

    for (unsigned int pieceId=PAWN_ID; pieceId<= KING_ID; pieceId++) {
        U64 bb = m_pieces[pieceId];
        while(bb) {
            U64 ls1b = LS1B(bb);
            int bitIdx = BitScanForward(ls1b);

            if (ls1b & m_colors[COLOR_BLACK])
                pieceIds[bitIdx] = BLACK_PIECE_NAMES[pieceId];
            if (ls1b & m_colors[COLOR_WHITE])
                pieceIds[bitIdx] = WHITE_PIECE_NAMES[pieceId];

            ResetLS1B(bb);
        }
    }

    for (int row=7; row>=0; row--) {
        ss << std::setw(2) << row+1 << "  ";

        for (int file=0; file<8; file++) {
            int sq = row*8+file;
            ss << pieceIds[sq] << " ";
        }

        ss << std::endl;
    }
    ss << "    a b c d e f g h" << std::endl;

    return ss.str();
}

/**
 * Sets a piece in 3 steps :
 * - Sets a bit on the piece type bitboard
 * - Clears a bit on the opponent occupation bitboard
 * - Sets a bit on the player occupation bitboard
 */
inline void Position::SetSquareValue(Square bitIdx, PieceId id, Color c) {
    U64 sq = 1UL << bitIdx;
    m_pieces[id] |= sq; 
    Color opp = GetOpponent(c);
    m_colors[opp] &= ~sq;
    m_colors[c]  |= sq;
    m_occupancy |= sq;
    m_empty = ~m_occupancy;
}

inline void Position::EmptySquareValue(Square bitIdx) {
    U64 sq = 1UL << bitIdx;
    m_pieces[PAWN_ID] &= ~sq;
    m_pieces[ROOK_ID] &= ~sq;
    m_pieces[KNIGHT_ID] &= ~sq;
    m_pieces[BISHOP_ID] &= ~sq;
    m_pieces[QUEEN_ID] &= ~sq;
    m_pieces[KING_ID] &= ~sq;
    m_colors[COLOR_WHITE] &= ~sq;
    m_colors[COLOR_BLACK] &= ~sq;
    m_occupancy = m_colors[COLOR_WHITE] | m_colors[COLOR_BLACK];
    m_empty = ~m_occupancy; 
}

inline void Position::EmptySquareValue(Square bitIdx, Color c, PieceId id) {
    U64 sq = 1UL << bitIdx;
    m_pieces[id] &= ~sq;
    m_colors[c] &= ~sq;
    m_occupancy &= ~sq;
    m_empty = ~m_occupancy;
}


/***
 * We consider that there's more empty squares than occupied squares.
 */
Color Position::SquareColor(const Square& bitIdx) const {
    U64 sq = 1UL << bitIdx;
    if (m_empty & sq)
        return COLOR_EMPTY;
    if (m_colors[COLOR_WHITE] & sq)
        return COLOR_WHITE;
    return COLOR_BLACK;
}

/***
 * We consider that there's more empty squares than occupied squares.
 */
PieceId Position::SquareId(const Square& bitIdx) const {
    U64 sq = 1UL << bitIdx;
    if (m_empty & sq)
        return EMPTY_ID;
    if (m_pieces[PAWN_ID] & sq)
        return PAWN_ID;
    if (m_pieces[ROOK_ID] & sq)
        return ROOK_ID;
    if (m_pieces[KNIGHT_ID] & sq)
        return KNIGHT_ID;
    if (m_pieces[BISHOP_ID] & sq)
        return BISHOP_ID;
    if (m_pieces[QUEEN_ID] & sq)
        return QUEEN_ID;
    return KING_ID;
}

/***
 * We consider that there's more empty squares than occupied squares.
 */
inline PieceId Position::SquareIdNoPawn(const Square& bitIdx) const {
    U64 sq = 1UL << bitIdx;
    if (m_empty & sq)
        return EMPTY_ID;
    if (m_pieces[ROOK_ID] & sq)
        return ROOK_ID;
    if (m_pieces[KNIGHT_ID] & sq)
        return KNIGHT_ID;
    if (m_pieces[BISHOP_ID] & sq)
        return BISHOP_ID;
    if (m_pieces[QUEEN_ID] & sq)
        return QUEEN_ID;
    return KING_ID;
}

void Position::GetMoves(Color srcColor, PieceId srcId, std::vector<Move>& moves, U64 (*f)(U64, Square)) {
    U64 srcBits = m_pieces[srcId] & m_colors[srcColor];

    while(srcBits) {
        int srcBitIdx = BitScanForward(LS1B(srcBits));
        U64 attackBits = f(m_occupancy, srcBitIdx) & ~m_pieces[KING_ID] & ~m_colors[srcColor];
        
        while(attackBits) {
            int dstBitIdx = BitScanForward(LS1B(attackBits));
            moves.emplace_back(srcColor, SquareColor(dstBitIdx), srcId, SquareId(dstBitIdx), srcBitIdx, dstBitIdx);
            ResetLS1B(attackBits);
        }
        ResetLS1B(srcBits);
    }
}

void Position::GetCastlingMoves(Color plyPlayer, std::vector<Move>& moves) {

    if (m_castleFlags == 0)
        return;

    U64 srcBits = m_pieces[KING_ID] & m_colors[plyPlayer];
    int sq = BitScanForward(srcBits);

    // White player

    if (plyPlayer == COLOR_WHITE) {
        if (CanCastleKingSide(COLOR_WHITE)) {
            moves.emplace_back(plyPlayer, COLOR_EMPTY,
                                KING_ID, EMPTY_ID, sq, g1);
            moves.back().m_castle |= (1<<3);
        }
        if (CanCastleQueenSide(COLOR_WHITE)) {
            moves.emplace_back(plyPlayer, COLOR_EMPTY,
                                KING_ID, EMPTY_ID, sq, c1);
            moves.back().m_castle |= (1<<2);
        }
        return;
    }

    // Black player

    if (CanCastleKingSide(COLOR_BLACK)) {
        moves.emplace_back(plyPlayer, COLOR_EMPTY,
                            KING_ID, EMPTY_ID, sq, g8);
        moves.back().m_castle |= (1<<1);
    }
    if (CanCastleQueenSide(COLOR_BLACK)) {
        moves.emplace_back(plyPlayer, COLOR_EMPTY,
                            KING_ID, EMPTY_ID, sq, c8);
        moves.back().m_castle |= 1;
    }
}

void Position::GetPawnMoves(Color plyPlayer, std::vector<Move> &moves) {
    
    U64 playerPawns = m_pieces[PAWN_ID] & m_colors[plyPlayer];
    if (!playerPawns)
        return;

    int direction;
    const int shift = 8;
    const U64 promotionRow[2]= { RANK_MASKS[a1], RANK_MASKS[a8] };
    const U64 noPromoRow[2]= { ~RANK_MASKS[a1], ~RANK_MASKS[a8] };

    Color opp;
    U64 simplePushBits;
    U64 doublePushBits;
    U64 eastCaptureBits;
    U64 westCaptureBits;
    Square dstSquare;

    if (plyPlayer == COLOR_WHITE) {
        opp = COLOR_BLACK;
        direction = 8;
        simplePushBits = (playerPawns << shift) & m_empty;
        doublePushBits = ((simplePushBits & RANK_MASKS[a3]) << 8) & m_empty;
        eastCaptureBits = ((playerPawns & NOT_H_FILE) << 9) & m_colors[COLOR_BLACK];
        westCaptureBits = ((playerPawns & NOT_A_FILE) << 7) & m_colors[COLOR_BLACK];
    }
    else {
        opp = COLOR_WHITE;
        direction = -8;
        simplePushBits = (playerPawns >> shift) & m_empty;
        doublePushBits = ((simplePushBits & RANK_MASKS[a6]) >> 8) & m_empty;
        eastCaptureBits = ((playerPawns & NOT_H_FILE) >> 7) & m_colors[COLOR_WHITE];
        westCaptureBits = ((playerPawns & NOT_A_FILE) >> 9) & m_colors[COLOR_WHITE];
    }

    // Simple Push
    U64 pushBits = simplePushBits & (noPromoRow[plyPlayer]);

    while (pushBits) {
        dstSquare = BitScanForward(LS1B(pushBits));
        moves.emplace_back(plyPlayer, COLOR_EMPTY, PAWN_ID, EMPTY_ID, dstSquare-direction, dstSquare);
        ResetLS1B(pushBits);
    }

    // Simple Push + Promo
    pushBits = simplePushBits & promotionRow[plyPlayer];

    while (pushBits) {
        dstSquare = BitScanForward(LS1B(pushBits));
        MoveGen::AddPromotionMoves(plyPlayer, COLOR_EMPTY, EMPTY_ID, dstSquare-direction, dstSquare, moves);
        ResetLS1B(pushBits);
    }

    // Double Push
    while (doublePushBits) {
        dstSquare = BitScanForward(LS1B(doublePushBits));
        moves.emplace_back(plyPlayer, COLOR_EMPTY, PAWN_ID, EMPTY_ID, dstSquare-(2*direction), dstSquare);
        moves.back().m_doublePush = true;
        ResetLS1B(doublePushBits);
    }


    // Captures East
    
    int captureOffset = plyPlayer == COLOR_WHITE ? 9 : -7;
    U64 captureBits = eastCaptureBits & (noPromoRow[plyPlayer]);
    
    while (captureBits) {
        dstSquare = BitScanForward(LS1B(captureBits));
        moves.emplace_back(plyPlayer, opp, PAWN_ID, SquareId(dstSquare), dstSquare-captureOffset, dstSquare);
        ResetLS1B(captureBits);
    }

    // Captures East + Promo

    captureBits = eastCaptureBits & promotionRow[plyPlayer];
    
    while (captureBits) {
        dstSquare = BitScanForward(LS1B(captureBits));
        MoveGen::AddPromotionMoves(plyPlayer, opp, SquareIdNoPawn(dstSquare), dstSquare-captureOffset, dstSquare, moves);
        ResetLS1B(captureBits);
    }

    // Captures West
    
    captureOffset = plyPlayer == COLOR_WHITE ? 7 : -9;
    captureBits = westCaptureBits & (noPromoRow[plyPlayer]);
    
    while (captureBits) {
        dstSquare = BitScanForward(LS1B(captureBits));
        moves.emplace_back(plyPlayer, opp, PAWN_ID, SquareId(dstSquare), dstSquare-captureOffset, dstSquare);
        ResetLS1B(captureBits);
    }

    // Captures West + Promo

    captureBits = westCaptureBits & promotionRow[plyPlayer];
    
    while (captureBits) {
        dstSquare = BitScanForward(LS1B(captureBits));
        MoveGen::AddPromotionMoves(plyPlayer, opp, SquareId(dstSquare), dstSquare-captureOffset, dstSquare, moves);
        ResetLS1B(captureBits);
    }
    
    // e.p. moves
    // "à la stockfish", if the loaded fen is invalid (e.p. square on wrong side)
    // it'll produce an erroneous result
    
    if (m_enPassantSquare != NO_EP) {
        // previous double push was white ply
        U64 srcBits;
        if ((1UL << m_enPassantSquare) & RANK_MASKS[a3]) {
            srcBits = playerPawns & RANK_MASKS[a4] & KING_MOVES[m_enPassantSquare];
        }
        // previous double push was black ply
        else {
            srcBits = playerPawns & RANK_MASKS[a5] & KING_MOVES[m_enPassantSquare];
        }
        while (srcBits) {
            Square srcSq = BitScanForward(LS1B(srcBits));
            moves.emplace_back(plyPlayer, COLOR_EMPTY, PAWN_ID, EMPTY_ID, srcSq, m_enPassantSquare);
            moves.back().m_enPassant = true;
            ResetLS1B(srcBits);
        }
    }
}

void Position::GetPseudoLegalMoves(Color c, std::vector<Move>& moves) {
    moves.reserve(64);
    GetPawnMoves(c, moves);
    GetMoves(c, KNIGHT_ID, moves, MoveGen::KnightAttacks);
    GetMoves(c, BISHOP_ID, moves, MoveGen::BishopAttacks);
    GetMoves(c, ROOK_ID, moves, MoveGen::RookAttacks);
    GetMoves(c, QUEEN_ID, moves, MoveGen::QueenAttacks);
    GetMoves(c, KING_ID, moves, MoveGen::KingAttacks);
    GetCastlingMoves(c, moves);
}

void Position::GetPseudoLegalMoves(Square sq, std::vector<Move>& moves) {
    std::vector<Move> tmpMoves;
    tmpMoves.reserve(8);

    Color c = SquareColor(sq);
    PieceId id = SquareId(sq);

    switch(id) {
        case PAWN_ID:
            GetPawnMoves(c, tmpMoves);
            break;
        case KNIGHT_ID:
            GetMoves(c, KNIGHT_ID, tmpMoves, MoveGen::KnightAttacks);
            break;
        case BISHOP_ID:
            GetMoves(c, BISHOP_ID, tmpMoves, MoveGen::BishopAttacks);
            break;
        case ROOK_ID:
            GetMoves(c, ROOK_ID, tmpMoves, MoveGen::RookAttacks);
            break;
        case QUEEN_ID:
            GetMoves(c, QUEEN_ID, tmpMoves, MoveGen::QueenAttacks);
            break;
        case KING_ID:
            GetMoves(c, KING_ID, tmpMoves, MoveGen::KingAttacks);
            GetCastlingMoves(c, tmpMoves);
            break;
        default:
            assert(id != EMPTY_ID);
    }
    
    for(auto &m: tmpMoves) {
        if (m.m_srcSquare == sq) {
            moves.emplace_back(m.m_srcSide, m.m_dstSide, m.m_srcId, m.m_dstId, sq, m.m_dstSquare);
            moves.back().m_enPassant = m.m_enPassant;
        }
    }
}
        
bool Position::IsKingSquareAttacked(Color c, Square sq) {

    Color opp = GetOpponent(c);

    if (KNIGHT_MOVES[sq] & (m_pieces[KNIGHT_ID] & m_colors[opp]))
        return true;
    if (KING_MOVES[sq] & (m_pieces[KING_ID] & m_colors[opp]))
        return true;
    if (MoveGen::RookAttacks(m_occupancy,sq) & ((m_pieces[ROOK_ID] | m_pieces[QUEEN_ID]) & m_colors[opp]))
        return true;
    if (MoveGen::BishopAttacks(m_occupancy,sq) & ((m_pieces[BISHOP_ID] | m_pieces[QUEEN_ID]) & m_colors[opp]))
        return true;

    // Checking pawn moves
    // 5 : mask of 101

    if (c == COLOR_WHITE) {
        U64 pawns = m_pieces[PAWN_ID] & m_colors[opp] & RANK_MASKS[sq+8];
        return (5UL << (sq+7)) & pawns;
    }
    else {
        U64 pawns = m_pieces[PAWN_ID] & m_colors[opp] & RANK_MASKS[sq-8];
        return (5UL << (sq-9)) & pawns;
    }
}

// Can be -1 : no attacker found
int Position::GetSmallestAttacker(Square sq) {
    //std::cout << "GetSmallestAttacker " << sq << std::endl;
    //std::cout << "PlyPlayer : " << m_plyPlayer << std::endl;
    //std::cout << ToString() << std::endl;
    
    U64 attacker;

    // Include King ?
    attacker = KING_MOVES[sq] & m_pieces[KING_ID] & m_colors[m_plyPlayer];
    if (attacker)
        return BitScanForward(LS1B(attacker));

    // Checking pawn moves

    if (m_plyPlayer == COLOR_BLACK) {
        U64 pawns = m_pieces[PAWN_ID] & m_colors[m_plyPlayer] & RANK_MASKS[sq+8];
        // East
        attacker = ((1UL << (sq+7)) & pawns & NOT_A_FILE);
        // West
        attacker |= ((1UL << (sq+9)) & pawns & NOT_H_FILE);
    }
    else {
        U64 pawns = m_pieces[PAWN_ID] & m_colors[m_plyPlayer] & RANK_MASKS[sq-8];
        // East
        attacker = ((1UL << (sq-7)) & pawns & NOT_A_FILE);
        // West
        attacker |= ((1UL << (sq-9)) & pawns & NOT_H_FILE);
    }

    if (attacker) {
        //std::cout << Debug(attacker) << std::endl;
        return BitScanForward(LS1B(attacker));
    }

    attacker = KNIGHT_MOVES[sq] & m_pieces[KNIGHT_ID] & m_colors[m_plyPlayer];
    if (attacker)
        return BitScanForward(LS1B(attacker));

    attacker = MoveGen::BishopAttacks(m_occupancy,sq) & m_pieces[BISHOP_ID] & m_colors[m_plyPlayer];
    if (attacker)
        return BitScanForward(LS1B(attacker));

    attacker = MoveGen::RookAttacks(m_occupancy,sq) & m_pieces[ROOK_ID] & m_colors[m_plyPlayer];
    if (attacker)
        return BitScanForward(LS1B(attacker));

    attacker = MoveGen::QueenAttacks(m_occupancy,sq) & m_pieces[QUEEN_ID] & m_colors[m_plyPlayer];
    if (attacker)
        return BitScanForward(LS1B(attacker));


    return -1;
}

bool Position::IsPlayerUnderCheck(Color c) {
    Square kingSq = BitScanForward(LS1B(m_pieces[KING_ID] & m_colors[c]));
    return IsKingSquareAttacked(c, kingSq);
}



void Position::DebugState() const {
    std::cout << "STATE ---------------------------" << std::endl;
    //std::cout << "White" << std::endl;
    //std::cout << Debug(m_colors[COLOR_WHITE]);
    //std::cout << "Black" << std::endl;
    //std::cout << Debug(m_colors[COLOR_BLACK]);
    //std::cout << "Kings" << std::endl;
    //std::cout << Debug(m_pieces[KING_ID]);
    std::cout << "Pawns" << std::endl;
    std::cout << Debug(m_pieces[PAWN_ID]);
    std::cout << "Empty" << std::endl;
    std::cout << Debug(m_empty);
    //std::cout << "Queens" << std::endl;
    //std::cout << Debug(m_pieces[QUEEN_ID]);
    //std::cout << "Rooks" << std::endl;
    //std::cout << Debug(m_pieces[ROOK_ID]);
    //std::cout << "Bishop" << std::endl;
    //std::cout << Debug(m_pieces[BISHOP_ID]);
    //std::cout << "Knights" << std::endl;
    //std::cout << Debug(m_pieces[KNIGHT_ID]);
}

bool Position::Identical(Position a, Position b) {

    if (a.m_colors[COLOR_WHITE] != b.m_colors[COLOR_WHITE]) {
        std::cout << "White table issue" << std::endl;
        return false;
    }
    if (a.m_colors[COLOR_BLACK] != b.m_colors[COLOR_BLACK]) {
        std::cout << "Black table issue, A-B, should be A" << std::endl;
        std::cout << a.ToString();
        std::cout << b.ToString();
        std::cout << Debug(a.m_colors[COLOR_BLACK]);
        std::cout << Debug(b.m_colors[COLOR_BLACK]);
        return false;
    }
    if (a.m_pieces[PAWN_ID] != b.m_pieces[PAWN_ID]) {
        std::cout << "Pawn table issue, A-B, should be A" << std::endl;
        std::cout << a.ToString() << std::endl;
        std::cout << b.ToString() << std::endl;
        //std::cout << Debug(a.m_pieces[PAWN_ID]) << std::endl;
        //std::cout << Debug(b.m_pieces[PAWN_ID]) << std::endl;
        return false;
    }
    if (a.m_pieces[ROOK_ID] != b.m_pieces[ROOK_ID]) {
        std::cout << "Rook table issue" << std::endl;
        return false;
    }
    if (a.m_pieces[BISHOP_ID] != b.m_pieces[BISHOP_ID]) {
        std::cout << "Bishop table issue" << std::endl;
        return false;
    }
    if (a.m_pieces[KNIGHT_ID] != b.m_pieces[KNIGHT_ID]) {
        std::cout << "Knight table issue" << std::endl;
        return false;
    }
    if (a.m_pieces[QUEEN_ID] != b.m_pieces[QUEEN_ID]) {
        std::cout << "Queen table issue" << std::endl;
        return false;
    }
    if (a.m_pieces[KING_ID] != b.m_pieces[KING_ID]) {
        std::cout << "King table issue" << std::endl;
        return false;
    }
    if (a.m_occupancy != b.m_occupancy) {
        std::cout << "Occupancy table issue, A-B, should be A" << std::endl;
        std::cout << a.ToString() << std::endl;
        std::cout << b.ToString() << std::endl;
        std::cout << Debug(a.m_occupancy) << std::endl;
        std::cout << Debug(b.m_occupancy);
        return false;
    }
    if (a.m_empty != b.m_empty) {
        std::cout << "Empty issue" << std::endl;
        return false;
    }

    if (a.m_enPassantSquare != b.m_enPassantSquare) {
        std::cout << "En Passant issue" << std::endl;
        return false;
    }

    if (a.m_castleFlags != b.m_castleFlags) {
        std::cout << "Castle flags issue" << std::endl;
        return false;
    }
    if (a.m_plyPlayer != b.m_plyPlayer) {
        std::cout << "player side issue" << std::endl;
        return false;
    }

    if (a.m_halfMoves != b.m_halfMoves) {
        std::cout << "half moves issue" << std::endl;
        return false;
    }
    return true;
}


// FIXME : create bit masks by castling type
bool Position::CanCastleKingSide(Color playerSide) {
    if (playerSide && ((m_castleFlags & (1<<3)) >> 3) && 
                SquareId(f1) == EMPTY_ID &&
                SquareId(g1) == EMPTY_ID && 
                SquareId(h1) == ROOK_ID && 
                SquareColor(h1) == COLOR_WHITE) {
        return (IsKingSquareAttacked(m_plyPlayer, e1) |
                IsKingSquareAttacked(m_plyPlayer, f1) |
                IsKingSquareAttacked(m_plyPlayer, g1)) == 0;
    }
    if (!playerSide && ((m_castleFlags & (1<<1)) >> 1) &&
                SquareId(f8) == EMPTY_ID && 
                SquareId(g8) == EMPTY_ID &&
                SquareId(h8) == ROOK_ID && 
                SquareColor(h8) == COLOR_BLACK) {
        return (IsKingSquareAttacked(m_plyPlayer, e8) |
                IsKingSquareAttacked(m_plyPlayer, f8) |
                IsKingSquareAttacked(m_plyPlayer, g8)) == 0;
    }
    return false;
}

bool Position::CanCastleQueenSide(Color playerSide) {
    if (playerSide && ((m_castleFlags & (1<<2)) >> 2) &&
                SquareId(d1) == EMPTY_ID &&
                SquareId(c1) == EMPTY_ID && 
                SquareId(b1) == EMPTY_ID && 
                SquareId(a1) == ROOK_ID && 
                SquareColor(a1) == COLOR_WHITE) {
        return (IsKingSquareAttacked(m_plyPlayer, e1) |
                IsKingSquareAttacked(m_plyPlayer, d1) |
                IsKingSquareAttacked(m_plyPlayer, c1)) == 0;
    }
    if (!playerSide && (m_castleFlags & 1) && 
                SquareId(d8) == EMPTY_ID &&
                SquareId(c8) == EMPTY_ID &&
                SquareId(b8) == EMPTY_ID && 
                SquareId(a8) == ROOK_ID && 
                SquareColor(a8) == COLOR_BLACK) {
        return (IsKingSquareAttacked(m_plyPlayer, e8) |
                IsKingSquareAttacked(m_plyPlayer, d8) |
                IsKingSquareAttacked(m_plyPlayer, c8)) == 0;
    }
    return false;
}

bool Position::IsThreeFold() {
    if (m_history.size() > 1) {
        U64 lastKey = m_history.back();
        int repetitions = 1;
    
        for(int i = (m_history.size()-2); i >= 0; i--) {
            //std::cout << "i " << i << "\n";
            if (m_history[i] == lastKey)
                repetitions++;

            if (repetitions == 3)
                return true;
        }
    }
    return false;
}

void Position::PushHistoryKey() {
    U64 key = Zobrist::GetHashCode(*this);
    //std::cout << "m_history.size : " << m_history.size() << "\n";
    m_history.push_back(key);
    //std::cout << "push : " << key << "\n";
}

void Position::PopHistoryKey() {
    m_history.pop_back();
}

void Position::MakeNullMove() {
    m_plyPlayer = (m_plyPlayer == COLOR_WHITE) ? COLOR_BLACK : COLOR_WHITE;
    m_enPassantSquare = NO_EP;
    m_ply++;
    PushHistoryKey();

    m_halfMoves = 0;
}

void Position::Make(const Move &m) {

    MovePiece(m.m_srcSide, m.m_dstSide, 
                m.m_srcId, m.m_dstId, 
                m.m_srcSquare, m.m_dstSquare);

    m_plyPlayer = (m_plyPlayer == COLOR_WHITE) ? COLOR_BLACK : COLOR_WHITE;
    m_ply++;

    if (m.m_capture || m.m_srcId == PAWN_ID) {
        m_halfMoves = 0;
    }
    else {
        m_halfMoves++;
    }

    if (m.m_srcId == PAWN_ID) {
        m_halfMoves = 0;

        if (m.m_doublePush) {
            short dir = m.m_srcSide ? 8 : -8;
            m_enPassantSquare = m.m_srcSquare + dir;
            PushHistoryKey();
            return;
        }

        if (m.m_pawnPromotionId) {
            m_pieces[PAWN_ID] &= ~(1UL << m.m_dstSquare); 
            SetSquareValue(m.m_dstSquare, m.m_pawnPromotionId, m.m_srcSide);
            m_enPassantSquare = NO_EP;
            PushHistoryKey();
            return;
        }
        if (m.m_enPassant) {
            short dir = m.m_srcSide ? 8 : -8;
            EmptySquareValue(m_enPassantSquare - dir, GetOpponent(m.m_srcSide), PAWN_ID);
            m_enPassantSquare = NO_EP;
            PushHistoryKey();
            return;
        }
        m_enPassantSquare = NO_EP;
    }    

    m_enPassantSquare = NO_EP;

    // Rook/King movement tracking for Castling invalidation
    
    const Square rookCastlingMoves [9][2] = {
        { -1, -1},
        { a8, d8},
        { h8, f8},
        { -1, -1},
        { a1, d1},
        { -1, -1},
        { -1, -1},
        { -1, -1},
        { h1, f1}
    };
    
    if (m.m_srcId == KING_ID) {

        // Clearing all White player flags : 0011 (3)
        // Clearing all Black player flags : 1100 (12)

        short clearingFlags = 12 >> m.m_srcSide >> m.m_srcSide;
        m_castleFlags &= clearingFlags;

        if (m.m_castle) {
            MovePiece(m.m_srcSide, rookCastlingMoves[m.m_castle][0], rookCastlingMoves[m.m_castle][1]);
        }
    }

    // Clearing castling flags in case of rook movement
    if (m.m_srcId == ROOK_ID) {
        if ( m.m_srcSide == COLOR_WHITE) {
            if (((m_castleFlags & (1<<3))) && m.m_srcSquare == h1)
                m_castleFlags &= ~(1<<3);
            else if (((m_castleFlags & (1<<2))) && m.m_srcSquare == a1)
                m_castleFlags &= ~(1<<2);
        }
        else {
            if (((m_castleFlags & (1<<1))) && m.m_srcSquare == h8)
                m_castleFlags &= ~(1<<1);
            else if ((m_castleFlags & 1) && m.m_srcSquare == a8)
                m_castleFlags &= ~1;
        }
    }
    PushHistoryKey();
}

void Position::Unmake(const Move& a, BoardFlags& flags) {
    m_plyPlayer = (m_plyPlayer == COLOR_WHITE) ? COLOR_BLACK : COLOR_WHITE;
    m_ply--;
    Revert(a);
    RevertBoardFlags(flags);
    PopHistoryKey();
}

void Position::UnmakeNullMove(BoardFlags& flags) {
    m_plyPlayer = (m_plyPlayer == COLOR_WHITE) ? COLOR_BLACK : COLOR_WHITE;
    m_ply--;
    RevertBoardFlags(flags);
    PopHistoryKey();
}

void Position::Revert(const Move& a) {

    const Square rookRevertMoves [9][2] = {
        { -1, -1},
        { d8, a8},
        { f8, h8},
        { -1, -1},
        { d1, a1},
        { -1, -1},
        { -1, -1},
        { -1, -1},
        { f1, h1}
    };

    MovePiece(a.m_srcSide, a.m_dstSquare, a.m_srcSquare);

    if (a.m_enPassant) {
        short dir = a.m_srcSide ? 8 : -8;
        SetSquareValue(a.m_dstSquare - dir, PAWN_ID, GetOpponent(a.m_srcSide));
        return;
    }

    if (a.m_capture) {
        SetSquareValue(a.m_dstSquare, a.m_dstId, a.m_dstSide);
    }

    if (a.m_pawnPromotionId) {
        U64 sq = 1UL << a.m_srcSquare;
        m_pieces[a.m_pawnPromotionId] &= ~sq; 
        m_pieces[PAWN_ID] |= sq; 
    }

    // K : 1000 (8)
    // Q :  100 (4)
    // k :   10 (2)
    // q :    1 (1)
    if (a.m_castle) {
        MovePiece(a.m_srcSide, rookRevertMoves[a.m_castle][0], rookRevertMoves[a.m_castle][1]);
        return;
    }
}

void Position::Test() {
    Position b;
    b.InitPlayerPieces("8/8/5p4/4K3/8/8/8/7k");
    std::cout << b.ToString();
    Square kingSq = BitScanForward(LS1B(b.m_pieces[KING_ID] & b.m_colors[COLOR_WHITE]));
    bool check = b.IsKingSquareAttacked(COLOR_WHITE, kingSq);
    std::cout << "check ? " << check << std::endl;
}

void Position::MovePiece(Color playerSide, Square srcSquare, Square dstSquare) {
    PieceId dstId = SquareId(srcSquare);
    EmptySquareValue(srcSquare);
    EmptySquareValue(dstSquare);
    SetSquareValue(dstSquare, dstId, playerSide);
}

void Position::MovePiece(Color srcSide, Color dstSide, PieceId srcId, PieceId dstId, Square srcSquare, Square dstSquare) {
    EmptySquareValue(srcSquare, srcSide, srcId);
    EmptySquareValue(dstSquare, dstSide, dstId);
    SetSquareValue(dstSquare, srcId, srcSide);
}

std::string Position::GetPieceName(Square c) const {
    std::string pieceName;
    PieceId pieceId = SquareId(c);
    if (SquareColor(c) == COLOR_WHITE)
        pieceName = WHITE_PIECE_NAMES[pieceId];
    else
        pieceName = BLACK_PIECE_NAMES[pieceId];

    return pieceName;
}

std::string Position::ToFEN() const {
    std::stringstream ss;

    for (int row=7; row>=0; row--) {
        int blank = 0;
        for (int file=0; file<8; file++) {
            std::string pieceName = GetPieceName(SQ_LUT[row][file]);
            if (pieceName == ".")
                blank++;
            else {
                if (blank > 0) {
                    ss << blank;
                    blank = 0;
                }
                ss << pieceName;
            }
        }
        if (blank != 0) 
            ss << blank;
        
        if (row>0)
            ss << "/";
    }

    if (m_plyPlayer == COLOR_WHITE)
        ss << " w ";
    else
        ss << " b ";
    
    bool castleSpace = false;
    if (m_castleFlags & (1<<3)) {
        ss << "K";
        castleSpace = true;
    }
    if (m_castleFlags & (1<<2)) {
        ss << "Q";
        castleSpace = true;
    }
    if (m_castleFlags & (1<<1)) {
        ss << "k";
        castleSpace = true;
    }
    if (m_castleFlags & 1) {
        ss << "q";
        castleSpace = true;
    }
    if (castleSpace)
        ss << " ";
    else
        ss << "- ";

    if (m_enPassantSquare == NO_EP) {
        ss << "- ";
    }
    else {
        ss << SquareTool::ToString(m_enPassantSquare) << " ";
    }

    return ss.str();
}


Color Position::GetOpponent(Color c)  {
    return static_cast<Color>(c^1);
}


void Position::RevertBoardFlags(const BoardFlags& bf) {
    m_castleFlags = bf.m_castleFlags;
    m_enPassantSquare = bf.m_enPassantSquare; 
    m_plyPlayer = bf.m_plyPlayer;
    m_halfMoves = bf.m_halfMoves;
}

/**
 * This is the function called when a human player enters a move
 */
bool Position::DoesMoveExist(Move& move, const char* src, const char* dst) {

    std::vector<Move> nextMoves;
    GetPseudoLegalMoves(m_plyPlayer, nextMoves);

    bool moveFound = false;

    for (auto &m : nextMoves) {
        if (m.m_srcSquare == SquareTool::FromCoords(src) && m.m_dstSquare == SquareTool::FromCoords(dst) && m.m_illegal == false) {

            // FIXME : We only consider Promote to queen for now
            if (m.m_pawnPromotionId != EMPTY_ID && m.m_pawnPromotionId != QUEEN_ID )
                continue;
            moveFound = true;
            move = m;
        }
    }

    return moveFound;
}

void Position::Clear() {
    for(unsigned int i=0; i<7; i++) 
        m_pieces[i] = 0;
    for(unsigned int i=0; i<2; i++) 
        m_colors[i] = 0;
    m_occupancy = 0;
    m_empty = ~m_occupancy;
}

void Position::InitPlayerPieces(const char * schema) {

    Clear();

    int row = 7;
    int file = 0;

    // Reading piece positions
    int i = -1;
    while(schema[++i] != '\0') {
        char c = schema[i];
        switch(c) {
            case 'r':
                SetSquareValue(SQ_LUT[row][file++], ROOK_ID, COLOR_BLACK);
                break;
            case 'n':
                SetSquareValue(SQ_LUT[row][file++], KNIGHT_ID, COLOR_BLACK);
                break;
            case 'b':
                SetSquareValue(SQ_LUT[row][file++], BISHOP_ID, COLOR_BLACK);
                break;
            case 'q':
                SetSquareValue(SQ_LUT[row][file++], QUEEN_ID, COLOR_BLACK);
                break;
            case 'k':
                SetSquareValue(SQ_LUT[row][file++], KING_ID, COLOR_BLACK);
                break;
            case 'p':
                SetSquareValue(SQ_LUT[row][file++], PAWN_ID, COLOR_BLACK);
                break;
            case 'R':
                SetSquareValue(SQ_LUT[row][file++], ROOK_ID, COLOR_WHITE);
                break;
            case 'N':
                SetSquareValue(SQ_LUT[row][file++], KNIGHT_ID, COLOR_WHITE);
                break;
            case 'B':
                SetSquareValue(SQ_LUT[row][file++], BISHOP_ID, COLOR_WHITE);
                break;
            case 'Q':
                SetSquareValue(SQ_LUT[row][file++], QUEEN_ID, COLOR_WHITE);
                break;
            case 'K':
                SetSquareValue(SQ_LUT[row][file++], KING_ID, COLOR_WHITE);
                break;
            case 'P':
                SetSquareValue(SQ_LUT[row][file++], PAWN_ID, COLOR_WHITE);
                break;
            case '/':
                file = 0;
                row--;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                char s[] = "X";
                s[0] = c;
                file += atoi(s);
                break;
        }

        if (c == ' ')
            break;

    }
    //std::cout << ToString();
    //assert(false);
}

/**
 * Once a move has been made, we need to test the legality of the new board :
 * is the player checked after new move ?
 */
bool Position::IsIllegal() {
    return IsPlayerUnderCheck(GetOpponent(m_plyPlayer));
}


