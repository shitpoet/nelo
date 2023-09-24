MinimalChessEngine 0.6 - August 29st, 2021
Author: Thomas Jahn (lithander@gmx.de)

MinimalChess is a UCI chess engine written in C#.

My focus is on a minimal implementation of only the most important features and optimizations. MinimalChess is written in only a few hundred lines of idiomatic C# code where other engines (of comparable strength) often have thousands.

Features:

* Implements the UCI protocol including all common time management options.
* Based on a simple 8x8 Board representation: Just an array to represent the 64 squares and keep track of the pieces.
* A 2-bucket transposition table based on zobrist hashes. Size can be set via UCI option.
* Staged move generation: TT moves first, followed by MVV-LVA sorted captures, followed by killers, finally all the remaining history-sorted non-captures.
* Iterative Deepening search with Null-Move pruning and Quiescence Search.
* Late move reductions and deep futility pruning.
* Evaluation based on tapered, tuned Piece-Square Tables and a 13th table for a dynamic term.

You can find the source on Github:
	https://github.com/lithander/MinimalChessEngine

And I uploaded explanatory making-of videos on Youtube:
 	https://www.youtube.com/playlist?list=PL6vJSkTaZuBtTokp8-gnTsP39GCaRS3du 