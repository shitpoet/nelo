package engine

import (
	"fmt"
	"strconv"
	"strings"
	"unicode"
)

// position.go implements a structure representing a position.

const (
	// Constants representing each piece type. The value of the constants
	// are selected so they can be used in Position.Pieces to index the
	// bitboards representing the given piece.
	Pawn   uint8 = 0
	Knight uint8 = 1
	Bishop uint8 = 2
	Rook   uint8 = 3
	Queen  uint8 = 4
	King   uint8 = 5
	NoType uint8 = 6

	// Constants representing each piece color. The value of the constants
	// are selected so they can be used in Position.Pieces and Position.SideBB to
	// index the bitboards representing the given piece of the given color, or
	// the given color.
	Black   uint8 = 0
	White   uint8 = 1
	NoColor uint8 = 2

	// Constants representing the four castling rights. Each constant is set to a number
	// with a single high bit, corresponding to each castling right.
	WhiteKingsideRight  uint8 = 0x8
	WhiteQueensideRight uint8 = 0x4
	BlackKingsideRight  uint8 = 0x2
	BlackQueensideRight uint8 = 0x1

	// Constants mapping each board coordinate to its square
	A1, B1, C1, D1, E1, F1, G1, H1 = 0, 1, 2, 3, 4, 5, 6, 7
	A2, B2, C2, D2, E2, F2, G2, H2 = 8, 9, 10, 11, 12, 13, 14, 15
	A3, B3, C3, D3, E3, F3, G3, H3 = 16, 17, 18, 19, 20, 21, 22, 23
	A4, B4, C4, D4, E4, F4, G4, H4 = 24, 25, 26, 27, 28, 29, 30, 31
	A5, B5, C5, D5, E5, F5, G5, H5 = 32, 33, 34, 35, 36, 37, 38, 39
	A6, B6, C6, D6, E6, F6, G6, H6 = 40, 41, 42, 43, 44, 45, 46, 47
	A7, B7, C7, D7, E7, F7, G7, H7 = 48, 49, 50, 51, 52, 53, 54, 55
	A8, B8, C8, D8, E8, F8, G8, H8 = 56, 57, 58, 59, 60, 61, 62, 63

	// A constant representing no square
	NoSq = 64

	// Common fen strings used in debugging and initalizing the engine.
	FENStartPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0"
	FENKiwiPete      = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
)

// A 64 element array where each entry, when bitwise ANDed with the
// castling rights, destorys the correct bit in the castling rights
// if a move to or from that square would take away castling rights.
var Spoilers = [64]uint8{
	0xb, 0xf, 0xf, 0xf, 0x3, 0xf, 0xf, 0x7,
	0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
	0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
	0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
	0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
	0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
	0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf,
	0xe, 0xf, 0xf, 0xf, 0xc, 0xf, 0xf, 0xd,
}

// An array mapping a castling kings destination square,
// to the origin and destination square of the appropriate
// rook to move.
var CastlingRookSq = map[uint8]RookCastlePositions{
	G1: {H1, F1},
	C1: {A1, D1},
	G8: {H8, F8},
	C8: {A8, D8},
}

// A constant mapping piece characters to Piece objects.
var CharToPiece = map[byte]Piece{
	'P': {Pawn, White},
	'N': {Knight, White},
	'B': {Bishop, White},
	'R': {Rook, White},
	'Q': {Queen, White},
	'K': {King, White},
	'p': {Pawn, Black},
	'n': {Knight, Black},
	'b': {Bishop, Black},
	'r': {Rook, Black},
	'q': {Queen, Black},
	'k': {King, Black},
}

// A constant mapping piece types to their respective characters.
var PieceTypeToChar = map[uint8]rune{
	Pawn:   'i',
	Knight: 'n',
	Bishop: 'b',
	Rook:   'r',
	Queen:  'q',
	King:   'k',
	NoType: '.',
}

// A struct representing the from and to squares of a given castling rook.
type RookCastlePositions struct {
	FromSq uint8
	ToSq   uint8
}

// A struct representing a piece
type Piece struct {
	Type  uint8
	Color uint8
}

// A struct representing position state that is irreversible (cannot be undone in
// UnmakeMove). A position state object is used each time a move is made, and then popped
// off of a stack once a move needs to be unmade.
type State struct {
	Hash           uint64
	CastlingRights uint8
	EPSq           uint8
	Rule50         uint8
	Captured       Piece
	Moved          Piece
}

// A struct reprenting Blunder's core internal position representation.
type Position struct {
	Pieces         [2][6]Bitboard
	Sides          [2]Bitboard
	Squares        [64]Piece
	Hash           uint64
	CastlingRights uint8
	SideToMove     uint8
	EPSq           uint8
	Ply            uint16
	Rule50         uint8
	prevStates     [100]State
	StatePly       uint8

	MGScores [2]int16
	EGScores [2]int16
	Phase    int16
}

// Setup the position using a fen string.
func (pos *Position) LoadFEN(FEN string) {
	// Reset the internal fields of the position
	pos.Pieces = [2][6]Bitboard{}
	pos.Sides = [2]Bitboard{}
	pos.Squares = [64]Piece{}
	pos.MGScores = [2]int16{}
	pos.EGScores = [2]int16{}
	pos.CastlingRights = 0
	pos.Phase = TotalPhase

	for square := range pos.Squares {
		pos.Squares[square] = Piece{Type: NoType, Color: NoColor}
	}

	// Load in each field of the FEN string.
	fields := strings.Fields(FEN)
	pieces := fields[0]
	color := fields[1]
	castling := fields[2]
	ep := fields[3]
	halfMove := fields[4]
	fullMove := fields[5]

	// Loop over each square of the board, rank by rank, from left to right,
	// loading in pieces at squares described by the FEN string.
	for index, sq := 0, uint8(56); index < len(pieces); index++ {
		char := pieces[index]
		switch char {
		case 'p', 'n', 'b', 'r', 'q', 'k', 'P', 'N', 'B', 'R', 'Q', 'K':
			piece := CharToPiece[char]
			pos.putPiece(piece.Type, piece.Color, sq)
			sq++
		case '/':
			sq -= 16
		case '1', '2', '3', '4', '5', '6', '7', '8':
			sq += pieces[index] - '0'
		}
	}

	// Set the side to move for the position.
	pos.SideToMove = Black
	if color == "w" {
		pos.SideToMove = White
	}

	// Set the en passant square for the position.
	pos.EPSq = NoSq
	if ep != "-" {
		pos.EPSq = coordinateToPos(ep)
		if (PawnAttacks[pos.SideToMove^1][pos.EPSq] & (pos.Pieces[pos.SideToMove][Pawn])) == 0 {
			pos.EPSq = NoSq
		}
	}

	// Set the half move counter and game ply for the position.
	halfMoveCounter, _ := strconv.Atoi(halfMove)
	pos.Rule50 = uint8(halfMoveCounter)

	gamePly, _ := strconv.Atoi(fullMove)
	gamePly *= 2
	if pos.SideToMove == Black {
		gamePly--
	}
	pos.Ply = uint16(gamePly)

	// Set the castling rights, for the position.
	for _, char := range castling {
		switch char {
		case 'K':
			pos.CastlingRights |= WhiteKingsideRight
		case 'Q':
			pos.CastlingRights |= WhiteQueensideRight
		case 'k':
			pos.CastlingRights |= BlackKingsideRight
		case 'q':
			pos.CastlingRights |= BlackQueensideRight
		}
	}

	// Generate the zobrist hash for the position...
	pos.Hash = Zobrist.GenHash(pos)
}

// Generate the FEN string represention of the current board.
func (pos Position) GenFEN() string {
	positionStr := strings.Builder{}

	for rankStartPos := 56; rankStartPos >= 0; rankStartPos -= 8 {
		emptySquares := 0
		for sq := rankStartPos; sq < rankStartPos+8; sq++ {
			piece := pos.Squares[sq]
			if piece.Type == NoType {
				emptySquares++
			} else {
				// If we have some consecutive empty squares, add then to the FEN
				// string board, and reset the empty squares counter.
				if emptySquares > 0 {
					positionStr.WriteString(strconv.Itoa(emptySquares))
					emptySquares = 0
				}

				piece := pos.Squares[sq]
				pieceChar := PieceTypeToChar[piece.Type]
				if piece.Color == White {
					pieceChar = unicode.ToUpper(pieceChar)
				}

				// In FEN strings pawns are represented with p's not i's
				if pieceChar == 'i' {
					pieceChar = 'p'
				} else if pieceChar == 'I' {
					pieceChar = 'P'
				}

				positionStr.WriteRune(pieceChar)
			}
		}

		if emptySquares > 0 {
			positionStr.WriteString(strconv.Itoa(emptySquares))
			emptySquares = 0
		}

		positionStr.WriteString("/")

	}

	sideToMove := ""
	castlingRights := ""
	epSquare := ""

	if pos.SideToMove == White {
		sideToMove = "w"
	} else {
		sideToMove = "b"
	}

	if pos.CastlingRights&WhiteKingsideRight != 0 {
		castlingRights += "K"
	}
	if pos.CastlingRights&WhiteQueensideRight != 0 {
		castlingRights += "Q"
	}
	if pos.CastlingRights&BlackKingsideRight != 0 {
		castlingRights += "k"
	}
	if pos.CastlingRights&BlackQueensideRight != 0 {
		castlingRights += "q"
	}

	if castlingRights == "" {
		castlingRights = "-"
	}

	if pos.EPSq == NoSq {
		epSquare = "-"
	} else {
		epSquare = posToCoordinate(pos.EPSq)
	}

	fullMoveCount := pos.Ply / 2
	if pos.Ply%2 != 0 {
		fullMoveCount = pos.Ply/2 + 1
	}

	return fmt.Sprintf(
		"%s %s %s %s %d %d",
		strings.TrimSuffix(positionStr.String(), "/"),
		sideToMove, castlingRights, epSquare,
		pos.Rule50, fullMoveCount,
	)
}

// Return a string representation of the board.
func (pos Position) String() (boardStr string) {
	boardStr += "\n"
	for rankStartPos := 56; rankStartPos >= 0; rankStartPos -= 8 {
		boardStr += fmt.Sprintf("%v | ", (rankStartPos/8)+1)
		for index := rankStartPos; index < rankStartPos+8; index++ {
			piece := pos.Squares[index]
			pieceChar := PieceTypeToChar[piece.Type]
			if piece.Color == White {
				pieceChar = unicode.ToUpper(pieceChar)
			}

			boardStr += fmt.Sprintf("%c ", pieceChar)
		}
		boardStr += "\n"
	}

	boardStr += "   ----------------"
	boardStr += "\n    a b c d e f g h"

	boardStr += "\n\n"
	if pos.SideToMove == White {
		boardStr += "turn: white\n"
	} else {
		boardStr += "turn: black\n"
	}

	boardStr += "castling rights: "
	if pos.CastlingRights&WhiteKingsideRight != 0 {
		boardStr += "K"
	}
	if pos.CastlingRights&WhiteQueensideRight != 0 {
		boardStr += "Q"
	}
	if pos.CastlingRights&BlackKingsideRight != 0 {
		boardStr += "k"
	}
	if pos.CastlingRights&BlackQueensideRight != 0 {
		boardStr += "q"
	}

	boardStr += "\nen passant: "
	if pos.EPSq == NoSq {
		boardStr += "none"
	} else {
		boardStr += posToCoordinate(pos.EPSq)
	}

	boardStr += fmt.Sprintf("\nfen: %s", pos.GenFEN())
	boardStr += fmt.Sprintf("\nzobrist hash: 0x%x", pos.Hash)
	boardStr += fmt.Sprintf("\nrule 50: %d\n", pos.Rule50)
	boardStr += fmt.Sprintf("game ply: %d\n", pos.Ply)
	return boardStr
}

// Given a move, create a copy of the current position with the move
// applied, and return whether or not the new position is valid.
func (pos *Position) DoMove(move Move) (isValid bool) {
	// Get the data we need from the given move
	from := move.FromSq()
	to := move.ToSq()
	moveType := move.MoveType()
	flag := move.Flag()

	// Create a new State object to save the state of the irreversible aspects
	// of the position before making the current move.
	state := State{
		Hash:           pos.Hash,
		CastlingRights: pos.CastlingRights,
		EPSq:           pos.EPSq,
		Rule50:         pos.Rule50,
		Captured:       pos.Squares[to],
		Moved:          pos.Squares[from],
	}

	// Increment the game ply and the fifty-move rule counter
	pos.Ply++
	pos.Rule50++

	// Clear the en passant square and en passant zobrist number
	pos.Hash ^= Zobrist.EPNumber(pos.EPSq)
	pos.EPSq = NoSq

	// Clear the moving piece from its origin square
	pos.zobristClearPiece(from)

	switch moveType {
	case Quiet:
		// if the move is quiet, simple put the piece at the destination square.
		pos.zobristPutPiece(state.Moved.Type, pos.SideToMove, to)
	case Attack:
		if flag == AttackEP {
			// If it's an attack en passant, get the actually capture square
			// of the pawn being captured, remove it, and put the moving pawn
			// on the destination square...
			capSq := uint8(int8(to) - getPawnPushDelta(pos.SideToMove))
			state.Captured = pos.Squares[capSq]

			pos.zobristClearPiece(capSq)
			pos.zobristPutPiece(Pawn, pos.SideToMove, to)
		} else {
			// Otherwise if the move is a normal attack, remove the captured piece
			// from the position, and put the moving piece at its destination square...
			pos.zobristClearPiece(to)
			pos.zobristPutPiece(state.Moved.Type, pos.SideToMove, to)
		}

		// Reset the fifty move rule counter
		pos.Rule50 = 0
	case Castle:
		// If the move is a castle, move the king to the appropriate square...
		pos.zobristPutPiece(state.Moved.Type, pos.SideToMove, to)

		// And move the correct rook.
		rookFrom, rookTo := CastlingRookSq[to].FromSq, CastlingRookSq[to].ToSq
		pos.zobristClearPiece(rookFrom)
		pos.zobristPutPiece(Rook, pos.SideToMove, rookTo)
	case Promotion:
		// If a pawn is promoting, check if it's capturing a piece,
		// remove the captured piece if needed, and then put the
		// correct promotion piece type on the to square indicated
		// by the move flag value.
		if state.Captured.Type != NoType {
			pos.zobristClearPiece(to)
		}
		pos.zobristPutPiece(uint8(flag+1), pos.SideToMove, to)
	}

	// If a pawn is moving, do some extra work.
	if state.Moved.Type == Pawn {

		// Reset the fifty-move rule counter.
		pos.Rule50 = 0

		if abs(int8(from)-int8(to)) == 16 {
			// If the move is a double pawn push, and there is no enemy pawn that's in
			// a position to capture en passant on the next turn, don't set the position's
			// en passant square.
			pos.EPSq = uint8(int8(to) - getPawnPushDelta(pos.SideToMove))
			if PawnAttacks[pos.SideToMove][pos.EPSq]&(pos.Pieces[pos.SideToMove^1][Pawn]) == 0 {
				pos.EPSq = NoSq
			}
		}

	}

	// Remove the current castling rights.
	pos.Hash ^= Zobrist.CastlingNumber(pos.CastlingRights)

	// Update the castling rights and the zobrist hash with the new castling rights.
	pos.CastlingRights = pos.CastlingRights & Spoilers[from] & Spoilers[to]
	pos.Hash ^= Zobrist.CastlingNumber(pos.CastlingRights)

	// Update the zobrist hash if the en passant square was set
	pos.Hash ^= Zobrist.EPNumber(pos.EPSq)

	// Save the State object and increment the stack counter
	// to point to the next empty slot in the position state history.
	pos.prevStates[pos.StatePly] = state
	pos.StatePly++

	// Flip the side to move and update the zobrist hash.
	pos.SideToMove ^= 1
	pos.Hash ^= Zobrist.SideToMoveNumber(pos.SideToMove)

	// Return the new position and let the caller know if it's valid.
	return !sqIsAttacked(pos, pos.SideToMove^1, pos.Pieces[pos.SideToMove^1][King].Msb())
}

func (pos *Position) UndoMove(move Move) {
	// Get the State object for this move
	pos.StatePly--
	state := pos.prevStates[pos.StatePly]

	// Restore some aspects of the position using the State object.
	pos.Hash = state.Hash
	pos.CastlingRights = state.CastlingRights
	pos.EPSq = state.EPSq
	pos.Rule50 = state.Rule50

	// Flip the side to move.
	pos.SideToMove ^= 1

	// Decrement the game ply
	pos.Ply--

	// Get the data we need from the given move
	from := move.FromSq()
	to := move.ToSq()
	moveType := move.MoveType()
	flag := move.Flag()

	// Put the moving piece back on it's orgin square
	pos.putPiece(state.Moved.Type, state.Moved.Color, from)

	switch moveType {
	case Quiet:
		// if the move is quiet, remove the piece from its destination square.
		pos.clearPiece(to)
	case Attack:
		if flag == AttackEP {
			// If it was an attack en passant, put the pawn back that
			// was captured, and clear the moving pawn from the destination
			// square.
			capSq := uint8(int8(to) - getPawnPushDelta(pos.SideToMove))
			pos.clearPiece(to)
			pos.putPiece(Pawn, state.Captured.Color, capSq)
		} else {
			// Otherwise If the move was a normal attack, put the captured piece
			// back on the destination square, and remove the attacking piece.
			pos.clearPiece(to)
			pos.putPiece(state.Captured.Type, state.Captured.Color, to)
		}
	case Castle:
		// If the move was a castle, clear the king from the destination square...
		pos.clearPiece(to)

		// and move the castled rook back to the right square.
		rookFrom, rookTo := CastlingRookSq[to].FromSq, CastlingRookSq[to].ToSq
		pos.clearPiece(rookTo)
		pos.putPiece(Rook, pos.SideToMove, rookFrom)
	case Promotion:
		// If the pawn was promoted, remove the promoted piece, and if
		// the promotion was a capture, put the captured piece back on
		// the destination square.
		pos.clearPiece(to)
		if state.Captured.Type != NoType {
			pos.putPiece(state.Captured.Type, state.Captured.Color, to)
		}
	}
}

func (pos *Position) DoNullMove() {
	state := State{
		Hash:           pos.Hash,
		CastlingRights: pos.CastlingRights,
		EPSq:           pos.EPSq,
		Rule50:         pos.Rule50,
	}

	// Save the State object and increment the stack counter
	// to point to the next empty slot in the position state history.
	pos.prevStates[pos.StatePly] = state
	pos.StatePly++

	// Clear the en passant square and en passant zobrist number
	pos.Hash ^= Zobrist.EPNumber(pos.EPSq)
	pos.EPSq = NoSq

	// Set the fifty move rule counter to 0, since we're
	// making a null-move.
	pos.Rule50 = 0

	// Increment the game ply.
	pos.Ply++

	// Flip the side to move and update the zobrist hash
	pos.SideToMove ^= 1
	pos.Hash ^= Zobrist.SideToMoveNumber(pos.SideToMove)
}

func (pos *Position) UndoNullMove() {
	// Get the State object for the null move
	pos.StatePly--
	state := pos.prevStates[pos.StatePly]

	// Restore the irreversible aspects of the position using the State object.
	pos.Hash = state.Hash
	pos.CastlingRights = state.CastlingRights
	pos.EPSq = state.EPSq
	pos.Rule50 = state.Rule50

	// Decrement the game ply.
	pos.Ply--

	// Flip the side to move.
	pos.SideToMove ^= 1
}

// Determine if the side to move is in check.
func (pos *Position) InCheck() bool {
	return sqIsAttacked(
		pos,
		pos.SideToMove,
		pos.Pieces[pos.SideToMove][King].Msb())
}

// Determine if the current position has no majors or miniors left.
func (pos *Position) NoMajorsOrMiniors() bool {
	knights := pos.Pieces[White][Knight].CountBits() + pos.Pieces[Black][Knight].CountBits()
	bishops := pos.Pieces[White][Bishop].CountBits() + pos.Pieces[Black][Bishop].CountBits()
	rook := pos.Pieces[White][Rook].CountBits() + pos.Pieces[Black][Rook].CountBits()
	queen := pos.Pieces[White][Queen].CountBits() + pos.Pieces[Black][Queen].CountBits()
	return knights+bishops+rook+queen == 0
}

// Determine if a move is pseduo-legally valid.
func (pos *Position) MoveIsPseduoLegal(move Move) bool {
	fromSq, toSq := move.FromSq(), move.ToSq()
	moved := pos.Squares[fromSq]
	captured := pos.Squares[toSq]

	toBB := SquareBB[toSq]
	allBB := pos.Sides[White] | pos.Sides[Black]
	sideToMove := pos.SideToMove

	if moved.Color != sideToMove ||
		captured.Type == King ||
		captured.Color == sideToMove {
		return false
	}

	if moved.Type == Pawn {
		if fromSq > 55 || fromSq < 8 {
			return false
		}

		// Credit to the Stockfish team for the idea behind this section of code to
		// verify pseduo-legal pawn moves.
		if ((PawnAttacks[sideToMove][fromSq] & toBB & allBB) == 0) &&
			!((fromSq+uint8(getPawnPushDelta(sideToMove)) == toSq) && (captured.Type == NoType)) &&
			!((fromSq+uint8(getPawnPushDelta(sideToMove)*2) == toSq) &&
				captured.Type == NoType &&
				pos.Squares[toSq-uint8(getPawnPushDelta(sideToMove))].Type == NoType &&
				canDoublePush(fromSq, sideToMove)) {
			return false
		}
	} else {
		if (moved.Type == Knight && ((KnightMoves[fromSq] & toBB) == 0)) ||
			(moved.Type == Bishop && ((GenBishopMoves(fromSq, allBB) & toBB) == 0)) ||
			(moved.Type == Rook && ((GenRookMoves(fromSq, allBB) & toBB) == 0)) ||
			(moved.Type == Queen && (((GenBishopMoves(fromSq, allBB) | GenRookMoves(fromSq, allBB)) & toBB) == 0)) ||
			(moved.Type == King && ((KingMoves[fromSq] & toBB) == 0)) {
			return false
		}
	}

	return true
}

// Put a piece of a given color and type on a square.
func (pos *Position) putPiece(pieceType, pieceColor, to uint8) {
	pos.Pieces[pieceColor][pieceType].SetBit(to)
	pos.Sides[pieceColor].SetBit(to)
	pos.Squares[to].Type = pieceType
	pos.Squares[to].Color = pieceColor

	pos.MGScores[pieceColor] += PieceValueMG[pieceType] + PSQT_MG[pieceType][FlipSq[pieceColor][to]]
	pos.EGScores[pieceColor] += PieceValueEG[pieceType] + PSQT_EG[pieceType][FlipSq[pieceColor][to]]
	pos.Phase -= PhaseValues[pieceType]
}

// Clear a piece of a given color and type from a square.
func (pos *Position) clearPiece(from uint8) {
	piece := &pos.Squares[from]
	pos.Pieces[piece.Color][piece.Type].ClearBit(from)
	pos.Sides[piece.Color].ClearBit(from)

	pos.MGScores[piece.Color] -= PieceValueMG[piece.Type] + PSQT_MG[piece.Type][FlipSq[piece.Color][from]]
	pos.EGScores[piece.Color] -= PieceValueEG[piece.Type] + PSQT_EG[piece.Type][FlipSq[piece.Color][from]]
	pos.Phase += PhaseValues[piece.Type]

	piece.Type = NoType
	piece.Color = NoColor
}

// Put the piece given on the given square
func (pos *Position) zobristPutPiece(pieceType, pieceColor, to uint8) {
	pos.Pieces[pieceColor][pieceType].SetBit(to)
	pos.Sides[pieceColor].SetBit(to)

	pos.Squares[to].Type = pieceType
	pos.Squares[to].Color = pieceColor
	pos.Hash ^= Zobrist.PieceNumber(pieceType, pieceColor, to)

	pos.MGScores[pieceColor] += PieceValueMG[pieceType] + PSQT_MG[pieceType][FlipSq[pieceColor][to]]
	pos.EGScores[pieceColor] += PieceValueEG[pieceType] + PSQT_EG[pieceType][FlipSq[pieceColor][to]]
	pos.Phase -= PhaseValues[pieceType]
}

// Clear the piece given from the given square.
func (pos *Position) zobristClearPiece(from uint8) {
	piece := &pos.Squares[from]
	pos.Pieces[piece.Color][piece.Type].ClearBit(from)
	pos.Sides[piece.Color].ClearBit(from)

	pos.MGScores[piece.Color] -= PieceValueMG[piece.Type] + PSQT_MG[piece.Type][FlipSq[piece.Color][from]]
	pos.EGScores[piece.Color] -= PieceValueEG[piece.Type] + PSQT_EG[piece.Type][FlipSq[piece.Color][from]]
	pos.Phase += PhaseValues[piece.Type]

	pos.Hash ^= Zobrist.PieceNumber(piece.Type, piece.Color, from)
	piece.Type = NoType
	piece.Color = NoColor
}

// Given a color, return the delta for a single pawn push for that
// color.
func getPawnPushDelta(color uint8) int8 {
	if color == White {
		return 8
	}
	return -8
}

// Determine if it's legal for a pawn to double push,
// given it's color and origin square.
func canDoublePush(fromSq uint8, color uint8) bool {
	if color == White {
		return RankOf(fromSq) == Rank2
	}
	return RankOf(fromSq) == Rank7
}
