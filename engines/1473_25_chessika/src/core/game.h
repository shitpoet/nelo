#ifndef _GAME_H
#define _GAME_H

#include "move.h"
#include "constants.h"
#include "position.h"

#include <chrono>
#include <vector>
#include <string>
#include <map>


class Game {

    friend class Tests;

    public:
        Game();

        bool AppendMove(const char*);
        void ClearHistory();
        std::string GetFEN();
        bool InitGame(const char * schema);
        bool LoadFEN(const char*);
        void NewGame();
        void PlayTurn();
        void PrintCastlingRookMove(Move &);
        std::string ToString();

    private:
        Position m_board;
        std::vector<Move> m_history;
        std::vector<Move> m_pvLine;
        int m_turn = 0;
};

#endif
