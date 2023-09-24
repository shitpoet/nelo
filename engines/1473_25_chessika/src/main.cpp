#include "core/constants.h"
#include "core/game.h"
#include "core/moveGen.h"
#include "core/hashtables.h"
#include "protocols/uci.h"
#include "tests.h"

#include <iostream>
#include <unistd.h>


int main() {
    MoveGen::InitFillUpAttacks();
    Zobrist::Init();
    //Tests::EvalMaterial();
    //Tests::TestZobristHashing();
    //Tests::Test3FoldScore();
    //return 0;

    Game game;
    //game.LoadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //std::cout << game.GetBoard().ToString();
    //Position p;
    //p = game.GetBoard();
    //std::cout << std::hex << "Key : " << Zobrist::GetHashCode(p) << std::endl;
    //return 0;

    return Uci::Listen(game);
}

