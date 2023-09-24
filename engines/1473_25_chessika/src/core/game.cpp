#include "../eval/eval.h"
#include "../search/search.h"
#include "clock.h"
#include "game.h"
#include "moveGen.h"
#include "settings.h"
#include "hashtables.h"

#include "stdlib.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <cstring>
#include <mutex>
#include <regex>
#include <stdexcept>
#include <unistd.h>
#include <vector>

Game::Game() {}

bool Game::LoadFEN(const char* fen) {

    // Regex groups
    // 1:board  2:player  3:castling  5:en passant 7:ply 8:turn       
    const std::regex re("^([RNBQKPrnbqkp012345678/]+) ([wb]) (([KQkq]*)|-) (([a-h][36])|-) ([01]) ([0-9]+)$");
    std::smatch match;
    std::string str(fen);

    std::string boardSchema;

    if (std::regex_search(str, match, re) && match.size() > 1) {

        m_board.m_plyPlayer = COLOR_WHITE;
        if (match.str(2) == "b")
            m_board.m_plyPlayer = COLOR_BLACK;

        // Castling

        short castleFlags = 0;

        if (match.str(3).find("K") != std::string::npos)
            castleFlags |= 1<<3; 
        
        if (match.str(3).find("Q") != std::string::npos)
            castleFlags |= 1<<2; 

        if (match.str(3).find("k") != std::string::npos)
            castleFlags |= 1<<1; 
        
        if (match.str(3).find("q") != std::string::npos)
            castleFlags |= 1; 

        m_board.m_castleFlags = castleFlags;

        if (match.str(5).find("-") == std::string::npos) {
            m_board.m_enPassantSquare = SquareTool::FromCoords(match.str(5).c_str());
        }
        else {
            m_board.m_enPassantSquare = -1;
        }

        m_turn = atoi(match.str(8).c_str());

        // Initializing board 
        boardSchema = match.str(1);
        m_board.InitPlayerPieces(boardSchema.c_str());
        m_board.m_ply = atoi(match.str(7).c_str());

        return true;
    }
    return false;
}


/***
 * This function is used by Web interface to trace castling moves
 ***/

void Game::PrintCastlingRookMove(Move &m) {
    if (m.m_castle) {
        if(m.m_castle & 1<<3)
            std::cout << "info string castling_rook h1f1" << std::endl;
        else if(m.m_castle & 1<<2)
            std::cout << "info string castling_rook a1d1" << std::endl;
        else if(m.m_castle & 1<<1)
            std::cout << "info string castling_rook h8f8" << std::endl;
        else
            std::cout << "info string castling_rook a8d8" << std::endl;
    }
}


void Game::PlayTurn() {
    TurnStatus moveStatus = LEGAL_MOVE;
    std::vector<Move> resultPV;

    Move bestMove;
    int nodes = 0;

    Clock::InitSearchTime(m_board.m_plyPlayer, m_board.m_ply);

    Search::IterativeSearch(m_board, nodes, resultPV);

    // Refreshing pvLine
    m_pvLine = resultPV;

    if (m_pvLine.size() > 0) {
        bestMove = m_pvLine[0];
        if (bestMove.m_checkmate) {
            if (m_board.m_plyPlayer == COLOR_WHITE)
                moveStatus = CHECKMATE_P1;
            else
                moveStatus = CHECKMATE_P2;
        }
        std::cout << "bestmove " << bestMove.ToCoords() << std::endl;
        if (Settings::WEB_MODE) {
            std::cout << "webmove " << bestMove.ToString() << std::endl;
            PrintCastlingRookMove(bestMove);
        }
    }
    else {
        if (!m_board.IsPlayerUnderCheck(COLOR_WHITE) && !m_board.IsPlayerUnderCheck(COLOR_BLACK)) {
            moveStatus = STALEMATE;
        }
        else {
            if (Clock::TIME_OUT == true) {
                moveStatus = TIMEOUT;
            }
            else {
                if (m_board.IsPlayerUnderCheck(COLOR_BLACK)) {
                    moveStatus = CHECKMATE_P1;
                }
                else if (m_board.IsPlayerUnderCheck(COLOR_WHITE)){
                    moveStatus = CHECKMATE_P2;
                }
            }
        }

        // Hack for web mode : if max depth is 0, we're only checking the last move legality
        if (Settings::WEB_MODE && Settings::MAX_DEPTH == 0) {
            std::cout << "webmove " << m_history.back().ToString() << std::endl;
            PrintCastlingRookMove(m_history.back());
        }
        else {
            std::cout << "info string no move calculated" << std::endl;
            switch(moveStatus) {
                case STALEMATE:
                    std::cout << "info string stalemate" << std::endl;
                    break;
                case CHECKMATE_P1:
                    std::cout << "info string checkmate_p1" << std::endl;
                    break;
                case CHECKMATE_P2:
                    std::cout << "info string checkmate_p2" << std::endl;
                    break;
                default:
                    std::cout << "info string lost_on_time" << std::endl;
            }
            exit(0);
        }
    }
}


std::string Game::GetFEN() {
    std::stringstream ss;
    ss << m_board.ToFEN();
    ss << m_board.m_halfMoves << m_turn;
    return ss.str();
}


bool Game::AppendMove(const char* move) {
    const std::regex re("^([a-h][1-8])([a-h][1-8])[bnrqBNRQ]?$");
    std::smatch match;
    std::string str(move);
    if (std::regex_search(str, match, re) && match.size() > 1) {
        Move inputMove;
        
        if (m_board.DoesMoveExist(inputMove, match.str(1).c_str(), match.str(2).c_str())) {
            m_board.Make(inputMove);
            if (m_board.IsIllegal()) {
                return false;
            }
            m_history.push_back(inputMove);
        }
        else {
            return false;
        }

        return true;
    }
    return false;
}


std::string Game::ToString() {
    std::stringstream ss;
    ss << m_board.ToString();
    ss << m_board.ToFEN() << std::endl;
    return ss.str();
}

void Game::ClearHistory() {
    m_board.m_history.clear();
}

void Game::NewGame() {
    m_board.Clear();
    m_pvLine.clear();
    LoadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}
