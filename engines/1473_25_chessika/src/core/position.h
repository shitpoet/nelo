#ifndef _POSITION_H
#define _POSITION_H

#include "constants.h"
#include "square.h"
#include "boardFlags.h"
#include "move.h"

class BoardFlags;

class Position {

    friend class Eval;
    friend class BoardFlags;
    friend class MoveGen;
    friend class Game;
    friend class Search;
    friend class Tests;
    friend class Uci;
    friend class Web;
    friend class Zobrist;

    protected:
        short m_castleFlags = 0; // KQkq
        Square m_enPassantSquare = NO_EP;
        Color m_plyPlayer = COLOR_BLACK;
        unsigned int m_ply = 0;


    public:
        Position();
        Position(Position&); // for testing purposes

        static bool Identical(Position a, Position b);
        static Color GetOpponent(Color);
        static void Test();

        void DebugState() const;
        bool DoesMoveExist(Move&, const char* src, const char* dst);
        std::string GetPieceName(PieceId, Color);
        std::string GetPieceName(Square) const;
        void GetPseudoLegalMoves(Color, std::vector<Move>&);
        void GetPseudoLegalMoves(Square sq, std::vector<Move>&);
        int GetSmallestAttacker(Square sq);
        void InitPlayerPieces(const char * schema);
        bool IsIllegal();
        bool IsPlayerUnderCheck(Color playerSide);
        void Make(const Move&);
        void MakeNullMove();
        void Revert(const Move&);
        void RevertBoardFlags(const BoardFlags&);
        Color SquareColor(const Square&) const;
        PieceId SquareId(const Square&) const;
        PieceId SquareIdNoPawn(const Square&) const;
        std::string ToString() const;
        std::string ToFEN() const;
        void Unmake(const Move& a, BoardFlags& flags);
        void UnmakeNullMove(BoardFlags&);
        void Clear();

    private:
        // For now, we keep the initial PieceId enum values
        // index 0 is for empty, then Pawn=1, etc.
        U64 m_pieces[7] { 0, 0, 0, 0, 0, 0, 0 };
        U64 m_colors[2] { 0, 0 };
        U64 m_occupancy = 0;
        U64 m_empty = ~m_occupancy;
        int m_halfMoves = 0;

        std::vector<U64> m_history;

        static std::string Debug(U64 x); // Pretty print
        static std::string Debug(int x); // Binary print

        bool CanCastleKingSide(Color);
        bool CanCastleQueenSide(Color);
        void BuildMoves(Color, PieceId, std::vector<Move>&, U64 srcBit, U64 attackBits);
        void GetMoves(Color, PieceId, std::vector<Move>&, U64 (*f)(U64, Square));
        void GetPawnMoves(Color, std::vector<Move>&);
        void GetCastlingMoves(Color, std::vector<Move>&);
        bool IsKingSquareAttacked(Color, Square);
        bool IsThreeFold();
        void MovePiece(Color, Square, Square);
        void MovePiece(Color, Color, PieceId, PieceId, Square, Square);
        void SetSquareValue(Square, PieceId, Color);
        void EmptySquareValue(Square, Color, PieceId);
        void EmptySquareValue(Square);
        void PushHistoryKey();
        void PopHistoryKey();
};

#endif
