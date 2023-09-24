﻿using System;
using System.Collections.Generic;

namespace GUI
{
    public enum PieceColour { White, Black };
    public enum PieceType { Pawn, Knight, Bishop, Rook, Queen, King };
    // The colour before the game result indicates which colour has lost.
    public enum GameStatus { Inactive, Active, Stalemate, WhiteCheckmate,
        BlackCheckmate, WhiteAdjudicate, BlackAdjudicate, WhiteTime, BlackTime,
        DrawRepetition, DrawFifty, DrawInsuffientMaterial };
    // If a move was a castle, capture, etc.
    public enum MoveResult { None, PawnMove, Capture, KingsideCastle, QueensideCastle }

    /**
     * @brief Representation of the board as a whole.
     */
    public class Board
    {
        public virtual Square[] Squares { get; protected set; }
        public virtual bool BlackCheck { get; set; }
        public virtual bool WhiteCheck { get; set; }
        public virtual bool BlackCastledR { get; set; }
        public virtual bool WhiteCastledR { get; set; }
        public virtual bool BlackCastledL { get; set; }
        public virtual bool WhiteCastledL { get; set; }
        public virtual PieceColour PlayerToMove { get; set; }
        public virtual byte EnPassantSquare { get; protected set; }
        public virtual PieceColour EnPassantColour { get; protected set; }

        public readonly byte[] castleDestinations = { 2, 6, 56, 62 };
        public readonly byte[] pawnPromotionDestinations = { 0, 1, 2, 3, 4, 5, 6, 7,
                                                             56, 57, 58, 59, 60, 61, 62, 63 };
        public readonly byte[] enPassantStartSquares = { 8, 9, 10, 11, 12, 13, 14, 15,
                                                         48, 49, 50, 51, 52, 53, 54, 55 };
        public readonly byte[] enPassantEndSquares = { 24, 25, 26, 27, 28, 29, 30, 31,
                                                       32, 33, 34, 35, 36, 37, 38, 39 };

        /**
         * @brief Default constructor.
         * 
         * Creates a board laid out in the standard chess starting position.
         */
        public Board (bool empty = false)
        {
            BlackCheck = false;
            WhiteCheck = false;
            // For castling, default to true and change based on position
            BlackCastledR = true;
            WhiteCastledR = true;
            BlackCastledL = true;
            WhiteCastledL = true;
            PlayerToMove = PieceColour.White;
            EnPassantSquare = 0;
            EnPassantColour = PieceColour.White;

            if (empty) {
                Squares = new Square[64];
                // Initialise empty squares
                for (int i = 0; i < 64; i++) {
                    Squares [i] = new Square ();
                }
            } else {
                BlackCastledR = false;
                WhiteCastledR = false;
                BlackCastledL = false;
                WhiteCastledL = false;

                Squares = new Square[64];
                // Initialise empty squares
                for (int i = 0; i < 64; i++) {
                    Squares [i] = new Square ();
                }

                // Add pieces for black.
                Squares [0].Piece = new Piece (PieceColour.Black, PieceType.Rook);
                Squares [1].Piece = new Piece (PieceColour.Black, PieceType.Knight);
                Squares [2].Piece = new Piece (PieceColour.Black, PieceType.Bishop);
                Squares [3].Piece = new Piece (PieceColour.Black, PieceType.Queen);
                Squares [4].Piece = new Piece (PieceColour.Black, PieceType.King);
                Squares [5].Piece = new Piece (PieceColour.Black, PieceType.Bishop);
                Squares [6].Piece = new Piece (PieceColour.Black, PieceType.Knight);
                Squares [7].Piece = new Piece (PieceColour.Black, PieceType.Rook);
                for (int i = 8; i <= 15; i++) {
                    Squares [i].Piece = new Piece (PieceColour.Black, PieceType.Pawn);
                }

                // Add pieces for white.
                for (int i = 48; i <= 55; i++) {
                    Squares [i].Piece = new Piece (PieceColour.White, PieceType.Pawn);
                }
                Squares [56].Piece = new Piece (PieceColour.White, PieceType.Rook);
                Squares [57].Piece = new Piece (PieceColour.White, PieceType.Knight);
                Squares [58].Piece = new Piece (PieceColour.White, PieceType.Bishop);
                Squares [59].Piece = new Piece (PieceColour.White, PieceType.Queen);
                Squares [60].Piece = new Piece (PieceColour.White, PieceType.King);
                Squares [61].Piece = new Piece (PieceColour.White, PieceType.Bishop);
                Squares [62].Piece = new Piece (PieceColour.White, PieceType.Knight);
                Squares [63].Piece = new Piece (PieceColour.White, PieceType.Rook);
            }
        }

        /**
         * @brief Copy constructor.
         * 
         * Copies the list of pieces from one board to another.
         */
        public Board(Board other, bool copyLists = false)
        {
            Squares = new Square[64];
            for (int i = 0; i < 64; i++) {
                Squares [i] = new Square ();
                if (other.Squares [i].Piece != null) {
                    Squares [i].Piece = new Piece (other.Squares [i].Piece, copyLists);
                }
            }
            BlackCheck = other.BlackCheck;
            WhiteCheck = other.WhiteCheck;
            BlackCastledL = other.BlackCastledL;
            WhiteCastledL = other.WhiteCastledL;
            BlackCastledR = other.BlackCastledR;
            WhiteCastledR = other.WhiteCastledR;
            PlayerToMove = other.PlayerToMove;
            EnPassantSquare = other.EnPassantSquare;
            EnPassantColour = other.EnPassantColour;
        }

        public void AddPiece(PieceColour colour, PieceType type, int position)
        {
            if (position >= 64)
                throw new ArgumentOutOfRangeException ("position",
                    "Position index for the chessboard must be less than 64.");

            if (Squares [position].Piece != null)
                throw new ArgumentException ("That square is already occupied.", "position");
                    
            Squares[position].Piece = new Piece(colour, type);
        }

        /**
         * @fn IsMoveValid
         * @brief Checks if a move is legal.
         * 
         * Checks is a move is legal by checking if the moving piece
         * contains the move in its @c LegalMoves list.
         */
        public bool IsMoveValid(byte source, byte destination)
        {
            Piece movingPiece = Squares [source].Piece;

            if (movingPiece == null)
                return false;

            if (movingPiece.Colour != PlayerToMove)
                return false;

            if (movingPiece.LegalMoves.Contains (destination))
                return true;

            return false;
        }

        /**
         * @fn MakeMove
         * @brief Makes a move.
         * 
         * Makes a move, switches the @c PlayerToMove variable,
         * and updates piece legal moves.
         */
        public MoveResult MakeMove(byte source, byte destination, PieceType? promoteTo = null)
        {
            MoveResult ret = MoveResult.None;

            if (!IsMoveValid (source, destination)) {
                throw new InvalidOperationException ("Invalid move entered.");
            }
            Piece movingPiece = Squares [source].Piece;

            // Forbid castling if the king moves.
            if (movingPiece.Type == PieceType.King) {
                if (movingPiece.Colour == PieceColour.White) {
                    WhiteCastledR = true;
                    WhiteCastledL = true;
                } else {
                    BlackCastledR = true;
                    BlackCastledL = true;
                }
            }

            if (movingPiece.Type == PieceType.Rook) {
                if (source == 0) {
                    BlackCastledL = true;
                } else if (source == 7) {
                    BlackCastledR = true;
                } else if (source == 56) {
                    WhiteCastledL = true;
                } else if (source == 63) {
                    WhiteCastledR = true;
                }
            }

            // Special rules for castling
            if (movingPiece.Type == PieceType.King &&
                (source == 4 || source == 60) &&
                Array.IndexOf (castleDestinations, destination) != -1) {
                ret = destination - source > 0 ? MoveResult.KingsideCastle : MoveResult.QueensideCastle;
                Square castleRookSquare = destination - source > 0 ?
                    Squares [destination + 1] : Squares [destination - 2];
                Squares [destination].Piece = movingPiece;
                Squares [source].Piece = null;
                Squares [destination - source > 0 ?
                    destination - 1 :
                    destination + 1].Piece = castleRookSquare.Piece;
                castleRookSquare.Piece = null;
            } else {
                // Handle pawn moves
                if (Squares [source].Piece.Type == PieceType.Pawn) {
                    ret = MoveResult.PawnMove;
                    // Handle en passant creation
                    if (Array.IndexOf (enPassantStartSquares, source) > -1 && Array.IndexOf (enPassantEndSquares, destination) > -1) {
                        EnPassantColour = Squares [source].Piece.Colour;
                        EnPassantSquare = EnPassantColour == PieceColour.White ? (byte)(destination + 8) : (byte)(destination - 8);
                    }
                }
                if (Squares [destination].Piece != null) {
                    ret = MoveResult.Capture;
                }
                switch (promoteTo) {
                    case PieceType.Bishop:
                        Squares [destination].Piece = new Piece (movingPiece.Colour, PieceType.Bishop);
                        Squares [source].Piece = null;
                        break;
                    case PieceType.Knight:
                        Squares [destination].Piece = new Piece (movingPiece.Colour, PieceType.Knight);
                        Squares [source].Piece = null;
                        break;
                    case PieceType.Rook:
                        Squares [destination].Piece = new Piece (movingPiece.Colour, PieceType.Rook);
                        Squares [source].Piece = null;
                        break;
                    case PieceType.Queen:
                        Squares [destination].Piece = new Piece (movingPiece.Colour, PieceType.Queen);
                        Squares [source].Piece = null;
                        break;
                    default:
                        // Handle en passant capture
                        if (movingPiece.Type == PieceType.Pawn && destination == EnPassantSquare && EnPassantSquare != 0) {
                            byte captureSquare = EnPassantColour == PieceColour.White ? (byte)(destination - 8) : (byte)(destination + 8);
                            Squares [destination].Piece = movingPiece;
                            Squares [captureSquare].Piece = null;
                            Squares [source].Piece = null;
                            ret = MoveResult.Capture;
                            EnPassantSquare = 0;
                        } else {
                            Squares [destination].Piece = movingPiece;
                            Squares [source].Piece = null;
                            if (movingPiece.Type != PieceType.Pawn) {
                                EnPassantSquare = 0;
                            }
                        }
                        break;
                }
                        
                Gtk.Application.Invoke (delegate {
                    MainClass.win.UpdateMaterialDifference (new Board(this));
                });
            }

            if (PlayerToMove == PieceColour.White) {
                PlayerToMove = PieceColour.Black;
            } else {
                PlayerToMove = PieceColour.White;
            }
            PiecePseudoLegalMoves.GeneratePseudoLegalMoves (this);
            PieceLegalMoves.GenerateLegalMoves (this);
            return ret;
        }

        /**
         * @fn UndoMove
         * @brief Undoes a move.
         * 
         * Undoes a move by doing the complete reverse of MakeMove and then
         * regenerating legal moves.
         */

        public void UndoMove(byte originalSource, byte originalDestination, bool[] lastCastles,
                             PieceType? originalPromoteTo = null)
        {
            Piece movingPiece = Squares[originalDestination].Piece;

            // Special rules for castling
            if (movingPiece.Type == PieceType.King &&
                Array.IndexOf(castleDestinations, originalDestination) != -1)
            {
                Square castleRookSquare = originalDestination - originalSource > 0 ?
                    Squares[originalDestination - 1] : Squares[originalDestination + 1];
                Squares[originalSource].Piece = movingPiece;
                Squares[originalDestination].Piece = null;
                Squares[originalDestination - originalSource > 0 ?
                    originalDestination + 1 :
                    originalDestination - 2].Piece = castleRookSquare.Piece;
                castleRookSquare.Piece = null;
                BlackCastledR = lastCastles[0];
                WhiteCastledR = lastCastles[1];
                BlackCastledL = lastCastles[2];
                WhiteCastledL = lastCastles[3];
            }
            else
            {
                Squares[originalSource].Piece = movingPiece;
                Squares[originalDestination].Piece = null;
            }

            if (PlayerToMove == PieceColour.White)
            {
                PlayerToMove = PieceColour.Black;
            }
            else
            {
                PlayerToMove = PieceColour.White;
            }
            PiecePseudoLegalMoves.GeneratePseudoLegalMoves(this);
            PieceLegalMoves.GenerateLegalMoves(this);
        }

        
        public GameStatus CheckForMate()
        {
            int legalMoveCountWhite = 0;
            int legalMoveCountBlack = 0;

            for (int i = 0; i < 64; i++) {
                Piece currentPiece = Squares [i].Piece;
                if (currentPiece == null)
                    continue;

                if (currentPiece.Colour == PieceColour.White) {
                    legalMoveCountWhite += currentPiece.LegalMoves.Count;
                } else {
                    legalMoveCountBlack += currentPiece.LegalMoves.Count;
                }
            }

            if (legalMoveCountWhite == 0 && WhiteCheck) {
                return GameStatus.WhiteCheckmate;
            } else if (legalMoveCountBlack == 0 && BlackCheck) {
                return GameStatus.BlackCheckmate;
            } else if (((legalMoveCountWhite == 0 && PlayerToMove == PieceColour.White) ||
                (legalMoveCountBlack == 0 && PlayerToMove == PieceColour.Black)) &&
                !WhiteCheck && !BlackCheck) {
                return GameStatus.Stalemate;
            }

            return GameStatus.Active;
        }

        public Piece PieceAt(int square)
        {
            return Squares [square].Piece;
        }

        public string ToFEN()
        {
            string fen = String.Empty;
            // Board configuration
            int blankCounter = 0;
            for (int i = 0; i < 64; i++) {
                Square sq = Squares[i];
                if (sq.Piece != null && blankCounter > 0) {
                    fen += blankCounter.ToString ();
                    blankCounter = 0;
                }
                if (sq.Piece == null) {
                    blankCounter++;
                } else {
                    switch (sq.Piece.Type) {
                        case PieceType.Pawn:
                            if (sq.Piece.Colour == PieceColour.Black)
                                fen += 'p';
                            else
                                fen += 'P';
                            break;
                        case PieceType.Rook:
                            if (sq.Piece.Colour == PieceColour.Black)
                                fen += 'r';
                            else
                                fen += 'R';
                            break;
                        case PieceType.Knight:
                            if (sq.Piece.Colour == PieceColour.Black)
                                fen += 'n';
                            else
                                fen += 'N';
                            break;
                        case PieceType.Bishop:
                            if (sq.Piece.Colour == PieceColour.Black)
                                fen += 'b';
                            else
                                fen += 'B';
                            break;
                        case PieceType.Queen:
                            if (sq.Piece.Colour == PieceColour.Black)
                                fen += 'q';
                            else
                                fen += 'Q';
                            break;
                        case PieceType.King:
                            if (sq.Piece.Colour == PieceColour.Black)
                                fen += 'k';
                            else
                                fen += 'K';
                            break;
                        default:
                            break;
                    }
                }

                // If at end of row (but not the last one), append '/'.
                // Also append the blank square count if not 0
                if (i % 8 == 7) {
                    if (blankCounter > 0) {
                        fen += blankCounter.ToString ();
                        blankCounter = 0;
                    }
                    if (i != 63)
                        fen += '/';
                }
            }
            fen += ' ';

            // Active colour
            if (PlayerToMove == PieceColour.White)
                fen += 'w';
            else
                fen += 'b';
            fen += ' ';

            // Castling availability
            if (!WhiteCastledR)
                fen += "K";
            if (!WhiteCastledL)
                fen += "Q";
            if (!BlackCastledR)
                fen += "k";
            if (!BlackCastledL)
                fen += "q";
            if (WhiteCastledR && BlackCastledR && WhiteCastledL && BlackCastledL)
                fen += '-';
            fen += ' ';

            // En passant target
            // NOT IMPLEMENTED
            fen += "- ";

            // Halfmove clock
            // NOT IMPLEMENTED
            fen += "0 ";

            // Fullmove count
            // NOT IMPLEMENTED
            fen += "1";

            return fen;
        }

        public override string ToString ()
        {
            string output = "";

            for (int i = 0; i < 64; i++) {
                output += (Squares [i].ToString () + " ");
                // If i is in column 7, start a new line
                if (i % 8 == 7)
                    output += Environment.NewLine;
            }

            return output;
        }

        public override bool Equals (object obj)
        {
            if (obj == null)
                return false;

            Board otherBoard = (Board)obj;
            if (otherBoard == null)
                return false;

            if (ToString () == otherBoard.ToString ())
                return true;
            else
                return false;
        }
    }

    /**
     * @brief Representation of a single piece.
     * 
     * @fn Note that it contains no default constructor as there is no "base" piece.
     */
    public class Piece
    {
        public PieceColour Colour { get; private set; }
        public PieceType Type { get; private set; }
        public List<byte> PseudoLegalMoves { get; set; }
        public List<byte> LegalMoves { get; set; }
        public byte TimesAttacked { get; set; }
        public byte TimesDefended { get; set; }
        public bool HasMoved { get; set; } /**< We need this to know if we can castle, for example. */

        /**
         * @brief Constructor for a Piece.
         * 
         * @param colour    the colour of the piece.
         * @param type      the type of the piece.
         */
        public Piece(PieceColour colour, PieceType type)
        {
            Colour = colour;
            Type = type;
        }

        /**
         * @brief Copy constructor for a piece.
         * 
         * @param other     the piece from which to copy.
         */
        public Piece(Piece other, bool copyLists = false)
        {
            Colour = other.Colour;
            Type = other.Type;
            if (copyLists == true)
            {
                if (other.PseudoLegalMoves != null)
                {
                    PseudoLegalMoves = new List<byte>(other.PseudoLegalMoves);
                }
                if (other.LegalMoves != null)
                {
                    LegalMoves = new List<byte>(other.LegalMoves);
                }
            }
            TimesAttacked = other.TimesAttacked;
            TimesDefended = other.TimesDefended;
            HasMoved = other.HasMoved;
        }

        /**
         * @brief Tests whether a piece is equal to another.
         * 
         * @fn Override of @c Object.Equals. First tests whether @c obj is null, then attempts to cast to
         * Piece. If it is still not null, it compares the @c PieceType and @c PieceColour properties.
         * 
         * @param obj   the object to compare to.
         */
        public override bool Equals (object obj)
        {
            if (obj == null)
                return false;

            Piece otherPiece = (Piece)obj;
            if (otherPiece == null)
                return false;

            if (Colour == otherPiece.Colour && Type == otherPiece.Type) {
                return true;
            } else {
                return false;
            }
        }

        /**
         * @brief Returns a string representation of the Piece object.
         * 
         * @return A string representation according to FEN notation (i.e., a black
         * queen is "Q" and a white bishop is "b").
         */
        public override string ToString ()
        {
            string output = "";

            switch (Type) {
                case PieceType.Pawn:
                    output = "p";
                    break;
                case PieceType.Knight:
                    output = "n";
                    break;
                case PieceType.Bishop:
                    output = "b";
                    break;
                case PieceType.Rook:
                    output = "r";
                    break;
                case PieceType.Queen:
                    output = "q";
                    break;
                case PieceType.King:
                    output = "k";
                    break;
            }

            if (Colour == PieceColour.White)
                output = output.ToUpper ();

            return output;
        }

        /**
         * @brief Generates a hash code which C# uses to compare objects in IEnumerables.
         * 
         * @return A hash of the integer representation of the piece colour added to the
         * integer representation of the piece type.
         */
        public override int GetHashCode ()
        {
            return (int)Colour + (int)Type;
        }
    }

    /**
     * @brief A representation of a single square.
     * 
     * @class A representation of a single square which can hold a Piece object or be empty.
     */
    public class Square
    {
        public Piece Piece { get; set; }

        /**
         * @brief Default constructor creates an empty square.
         */
        public Square()
        {
            Piece = null;
        }

        /**
         * @brief Constructor which initialises to hold a piece.
         * 
         * @param piece     the piece to hold in the square.
         */
        public Square(Piece piece)
        {
            Piece = piece;
        }

        /**
         * @brief Copy constructor.
         * 
         * @param other     a square to copy the piece from.
         */
        public Square(Square other)
        {
            Piece = other.Piece;
        }

        public override string ToString ()
        {
            if (Piece == null)
                return "_";
            else
                return Piece.ToString ();
        }

        public bool isEmpty()
        {
            return Piece == null;
        }
    }
}
