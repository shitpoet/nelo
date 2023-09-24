#include "uci.h"
#include "../search/search.h"
#include "../core/clock.h"
#include "../core/settings.h"
#include "../tests.h"

#include <iostream>
#include <regex>
#include <string>
#include <thread>

int Uci::Listen(Game &game) {
    std::string input;

    while(true) {
        std::getline(std::cin, input);

        // Handling \r\n on windows 
        int i = input.find('\r', 0);
        if (i > -1)
            input.replace(i, 1, "");

        if (input == "" || input[0] == '#') {
            continue;
        }

        if (input == "uci") {
            std::cout << "id name Chessika 2.27" << std::endl
                      << "id author Laurent 'MrPingouin' Chea" << std::endl
                      //<< "option name MaxDuration type spin default 1 min 1 max 500" << std::endl
                      //<< "option name MaxDepth type spin default 10 min 1 max 20" << std::endl
                      //<< "option name ShowMoveOrdering type check default false" << std::endl
                      //<< "option name ShowCalculation type check default true" << std::endl
                      //<< "option name WebMode type check default false" << std::endl
                      << "uciok" << std::endl;
        }

        // Options handling

        else if (input.rfind("setoption", 0) == 0) {
            const std::regex optionRxp("setoption name ([a-zA-Z-_0-9]+) value ([A-Za-z0-9-_]+)");
            std::smatch match;

            if (std::regex_search(input, match, optionRxp) && match.size() > 1) {
                if (match.str(1) == "MaxDuration") {
                    Clock::MAX_DURATION_SECS = atoi(match.str(2).c_str());
                } 
                else if (match.str(1) == "MaxDepth") {
                    Settings::MAX_DEPTH = atoi(match.str(2).c_str());
                } 
                else if (match.str(1) == "ShowMoveOrdering") {
                    Settings::SHOW_MOVE_ORDERING = (match.str(2) == "true");
                } 
                else if (match.str(1) == "ShowCalculation") {
                    Settings::SHOW_MOVE_CALCULATION = (match.str(2) == "true");
                } 
                else if (match.str(1) == "WebMode") {
                    Settings::WEB_MODE = (match.str(2) == "true");
                } 
                else {
                    std::cout << "info string INVALID OPTION " << match.str(1) << std::endl;
                }
            }
        }
        else if (input == "stop") {
            Search::Stop();
        }
        else if (input == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (input.rfind("perft", 0) == 0) {
            const std::regex valueRxp("perft ([0-9]+)");
            std::smatch match;
            if (std::regex_search(input, match, valueRxp) && match.size() > 1) {
                int depth = atoi(match.str(1).c_str());
                Tests::RunPerft(game, depth);
                return 0;
            }
        }
        else if (input == "ucinewgame") {
            game.NewGame();
        }

        /**
         * Loading positions
         */
        
        else if (input.rfind("position startpos", 0) == 0) {
            if (InsertHistoryMoves(game, input) == -1)
                return -1;
        }
        else if (input.rfind("position fen", 0) == 0) {
            const std::regex fenRxp("position fen (.*)");
            std::smatch match;

            if (std::regex_search(input, match, fenRxp) && match.size() > 1) {
                game.LoadFEN(match.str(1).c_str());
            }
            else {
                return 0;
            }
        }

        else if (input.rfind("go", 0) == 0) {
            const std::regex goRxp("(wtime|btime|winc|binc|movestogo|depth) ([0-9]+)");
            std::smatch match;
            while (std::regex_search(input, match, goRxp)) {
                int value = atoi(match.str(2).c_str());
                if (match.str(1) == "wtime") {
                    Clock::WHITE_TIME = value;
                }
                if (match.str(1) == "btime") {
                    Clock::BLACK_TIME = value;
                }
                if (match.str(1) == "movestogo") {
                    Clock::MOVES_TO_GO = value;
                }
                if (match.str(1) == "winc") {
                    Clock::WHITE_INC_TIME = value;
                }
                if (match.str(1) == "binc") {
                    Clock::BLACK_INC_TIME = value;
                }
                if (match.str(1) == "depth") {
                    Settings::MAX_DEPTH = value;
                }
                input = match.suffix().str(); 
            }
            if (Settings::WEB_MODE) {
                game.PlayTurn();
            }
            else {
                std::thread t(&Game::PlayTurn, std::ref(game));
                t.detach();
            }
        }
        else if (input == "d") {
            std::cout << game.ToString();
        }
        else if (input == "quit") {
            return 0;
        }
        else  {
            std::cout << "Unknown command: '" << input << "'" << std::endl;
        }
    }
    return 0;
}

int Uci::InsertHistoryMoves(Game& game, std::string input) {
    game.LoadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    game.ClearHistory();

    const std::regex moveRxp("[a-h1-8]{4}[brnqBRNQ]?");
    std::smatch match;

    while (std::regex_search(input, match, moveRxp)) { 
        if (!game.AppendMove(match.str(0).c_str())) {
            std::cout << "info string INVALID move " << match.str(0) << std::endl;
            return -1;
        }
        input = match.suffix().str(); 
    } 

    return 0;
}


