#include "tests.h"
#include "core/hashtables.h"
#include "core/settings.h"
#include "eval/eval.h"

#include "assert.h"
#include <iostream>


void Tests::RunPerft(Game& game, int maxDepth) {
    if (maxDepth > 0)
        maxDepth--;
    else
        maxDepth = 0;

    int nodes = Tests::SplitPerft(game.m_board, maxDepth, false);
    std::cout << game.GetFEN() << " ;D" << maxDepth+1 << " " << nodes << std::endl;
}


int Tests::SplitPerft(Position& b, int maxDepth, bool displayMoves) {
    int nodes = 0;
    int rootDepth = 0;

    std::vector<Move> nextMoves;
    b.GetPseudoLegalMoves(b.m_plyPlayer, nextMoves);

    BoardFlags bFlags = BoardFlags(b);
    //Position refBoard(b);

    for (auto &m: nextMoves) {

        int _splitNodes = 0;

        b.Make(m);
        
        if (! b.IsPlayerUnderCheck(m.m_srcSide)) {
            Perft(b, rootDepth, maxDepth, _splitNodes);

            if (_splitNodes != 0) {
                nodes += _splitNodes;

                if (displayMoves) {
                    std::cout << SquareTool::ToString(m.GetSrcSquare());
                    std::cout << SquareTool::ToString(m.GetDstSquare());
                    if (m.GetPawnPromotionId()) {
                        std::cout << Piece::GetPieceName(m.GetPawnPromotionId(), m.m_srcSide);
                    }
                    std::cout << ": " << _splitNodes << std::endl;
                }
            }
        }
        b.Unmake(m, bFlags);
        //if (!Position::Identical(refBoard,b)) {
        //    std::cout << "Issue reverting " << m.ToCoords() << std::endl;
        //    assert(false);
        //}
    }

    return nodes;
}


void Tests::Perft(Position& b, int depth, int maxDepth, int& nodes) {

    if (depth == maxDepth) {
        nodes++;
    }
    else {
        
        std::vector<Move> nextMoves;
        b.GetPseudoLegalMoves(b.m_plyPlayer, nextMoves);

        BoardFlags bFlags = BoardFlags(b);
        //Position refBoard(b);

        for(auto& m : nextMoves) {

            b.Make(m);
            
            if (! b.IsPlayerUnderCheck(m.m_srcSide)) {
                Perft(b, depth+1, maxDepth, nodes);
            }
            b.Unmake(m, bFlags);
            //if (!Position::Identical(refBoard,b)) {
            //    std::cout << "Issue reverting " << m.ToCoords() << std::endl;
            //    assert(false);
            //}
        }
    }
}

void Tests::Test3FoldScore() {
    std::cout <<"Tests::Test3FoldScore" << std::endl;
    Game game;
    game.LoadFEN("rk6/8/8/8/8/8/7r/1K6 b - - 0 1");
    std::cout << game.m_board.ToString() << std::endl;
    assert(game.AppendMove("h2h3") == true);
    assert(game.AppendMove("b1c1") == true);
    assert(game.AppendMove("h3h2") == true);
    assert(game.AppendMove("c1b1") == true);
    assert(game.AppendMove("h2h3") == true);
    assert(game.AppendMove("b1c1") == true);
    assert(game.AppendMove("h3h2") == true);
    assert(game.AppendMove("c1b1") == true);
    assert(game.AppendMove("h2h3") == true);
    assert(game.m_board.IsThreeFold() == true);
}

/***
 * Things to test : captures, enPassant, castling rights
 ***/
void Tests::TestZobristHashing() {
    std::cout <<"Tests::TestZobristHashing" << std::endl;
    std::cout << std::hex;

    Game game1;
    game1.LoadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    game1.AppendMove("e2e4");
    game1.AppendMove("g8f6");
    game1.AppendMove("b1c3");
    game1.AppendMove("f6e4");
    U64 key1 = Zobrist::GetHashCode(game1.m_board);
    std::cout << game1.m_board.ToString();
    std::cout << "key : " << key1 << std::endl;
    Position p1(game1.m_board);

    std::cout <<std::endl;

    Game game2;
    game2.LoadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    game2.AppendMove("b1c3");
    game2.AppendMove("g8f6");
    game2.AppendMove("e2e4");
    game2.AppendMove("f6e4");
    std::cout << game2.m_board.ToString();
    U64 key1bis = Zobrist::GetHashCode(game2.m_board);
    std::cout << "key : " << key1bis << std::endl;
    Position p2(game2.m_board);

    assert(Position::Identical(p1,p2));
    assert(key1 == key1bis);
}


void Tests::EvalMaterial() {
    std::cout <<"Tests::EvalMaterial" << std::endl;
    Game game;
    game.LoadFEN("p7/8/8/8/8/8/8/P7 w - - 0 1");
    assert(Eval::GetBoardMaterialValue(game.m_board, COLOR_WHITE) == PIECE_VALUES[PAWN_ID]);
    assert(Eval::GetBoardMaterialValue(game.m_board, COLOR_BLACK) == PIECE_VALUES[PAWN_ID]);

    game.LoadFEN("r7/8/8/8/8/8/8/R7 w - - 0 1");
    assert(Eval::GetBoardMaterialValue(game.m_board, COLOR_WHITE) == PIECE_VALUES[ROOK_ID]);
    assert(Eval::GetBoardMaterialValue(game.m_board, COLOR_BLACK) == PIECE_VALUES[ROOK_ID]);

    // We ignore king value
    game.LoadFEN("k7/8/8/8/8/8/8/K7 w - - 0 1");
    assert(Eval::GetBoardMaterialValue(game.m_board, COLOR_WHITE) == 0);
    assert(Eval::GetBoardMaterialValue(game.m_board, COLOR_BLACK) == 0);

    //game.LoadFEN("rk6/8/8/8/8/8/7r/1K6 b - - 0 1");
    //std::cout << game.m_board.ToString() << std::endl;
    //std::cout << Eval::GetBoardMaterialValue(game.m_board, COLOR_WHITE) << std::endl;
    //std::cout << Eval::GetBoardMaterialValue(game.m_board, COLOR_BLACK) << std::endl;
}
