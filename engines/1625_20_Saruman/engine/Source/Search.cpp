#include "Search.hpp"
#include "Board.hpp"
#include "MoveList.hpp"
#include "CaptureList.hpp"
#include "UCI.hpp"
#include "TranspositionTables.hpp"

#include <iostream>

void outbitboard(u64 n);
u64 Search::nodes = 0;
u64 Search::totalNodes = 0;
u64 Search::KillerList[] = {};

void Search::clearKiller()
{
    for (int i = 0; i < 1024; i++) {
        KillerList[i] = 0;
    }
}

pair<string, int> Search::RootAlphaBeta(Board gameBoard, int playerColor, int remainingDepth, std::vector<string> searchMoves)
{
    MoveList possibleMoves;
    if (searchMoves.size() == 0) {
        possibleMoves = MoveList(gameBoard, ((playerColor == 1) ? WHITE_CODE : BLACK_CODE), TranspositionTables::getBest(gameBoard.getZorHash()).best, KillerList[remainingDepth]);
    } else {
        possibleMoves = MoveList(gameBoard, ((playerColor == 1) ? WHITE_CODE : BLACK_CODE), searchMoves);
    }
    nodes++;

    if (possibleMoves.getMovN(0) == 0) {
        return std::make_pair("", 0);
    }

    u64 curBestMove = possibleMoves.getMovN(0);
    int currMoveNumber = 0;
    int maxScore = NEGATIVE_INFINITY;
    int score;

    u64 castle = gameBoard.getCastleOrEnpasent();
    u64 lastHash = gameBoard.getZorHash();
    int enpasCol = gameBoard.getEnpasentCol();
    int halfMoveNumber = gameBoard.halfMoveClock;
    int lastTo = gameBoard.getLastMove();

    while (true) {
        u64 nextMove = possibleMoves.getNextMove();
        if (nextMove != 0) {
            gameBoard.makeMov(nextMove);
            if (gameBoard.inCheck(((playerColor == 1) ? WHITE_CODE : BLACK_CODE)) == false) {
                UCI::sendInfo("currmove " + possibleMoves.getMoveCode(nextMove) + " currmovenumber " + to_string(++currMoveNumber));
                score = -AlphaBeta(gameBoard, NEGATIVE_INFINITY, -maxScore, remainingDepth - 1, playerColor*-1, true);
                //score = -AlphaBeta(gameBoard, NEGATIVE_INFINITY, INFINITY, remainingDepth - 1, playerColor*-1);
                //cout << "Move is " << possibleMoves.getMoveCode(nextMove.second) << " and score is " << score << std::endl;
                if (score > maxScore){
                    maxScore = score;
                    curBestMove = nextMove;
                }
            }
            gameBoard.unMakeMov(nextMove, castle, enpasCol, lastHash, halfMoveNumber, lastTo);
        } else {
            break;
        }
    }

    if (currMoveNumber == 0) {
        UCI::killSearch = true;
    }

    if ((UCI::quit) or (UCI::killSearch)){
        return make_pair("", 0);
    }
    TranspositionTables::setEntry(gameBoard.getZorHash(), curBestMove, remainingDepth, maxScore, PV_NODE);
    return make_pair(possibleMoves.getMoveCode(curBestMove), maxScore);
}

int Search::AlphaBeta(Board& gameBoard, int alpha, int beta, int remainingDepth, int playerColor, bool allowNullMove)
{
    nodes++;
    if ((UCI::quit) or (UCI::killSearch)){
        return 0;
    }

    if (TranspositionTables::isOpen(gameBoard.getZorHash())) {
        return 0;
    }

    if (remainingDepth <= 0){
        if (gameBoard.inCheck(((playerColor*-1 == 1) ? WHITE_CODE : BLACK_CODE))) {
            return -ILLEGAL_MOVE;
        }
        return qSearch(gameBoard, alpha, beta, playerColor);
        //return gameBoard.getEvaluation()*playerColor;
    }
    entry best = TranspositionTables::getBest(gameBoard.getZorHash());
    if (best.depth >= remainingDepth) {
        if (best.type == CUT_NODE) {
            if (best.score >= beta) {
                return best.score; //beta
            }
        } else if (best.type == ALL_NODE) {
            if (best.score <= alpha) {
                return alpha; //best.score;
            }
        }
    }

    MoveList possibleMoves = MoveList(gameBoard, ((playerColor == 1) ? WHITE_CODE : BLACK_CODE), best.best, KillerList[remainingDepth]);
    if (possibleMoves.kingTake) {
        return -ILLEGAL_MOVE;
    }

    int score;
    u64 castle = gameBoard.getCastleOrEnpasent();
    u64 lastHash = gameBoard.getZorHash();
    int enpasCol = gameBoard.getEnpasentCol();
    int halfMoveNumber = gameBoard.halfMoveClock;
    int lastTo = gameBoard.getLastMove();
    bool canMove = false;

    if (allowNullMove) {
        if (gameBoard.inCheck(((playerColor == 1) ? WHITE_CODE : BLACK_CODE)) == false) {
            gameBoard.setCastleOrEnpas(gameBoard.nextCastleOrEnpasent());
            gameBoard.setZorHash(gameBoard.getZorHash() ^ TranspositionTables::getBlackHash());
            int eval = -AlphaBeta(gameBoard, -beta, -beta + 1, remainingDepth - 1 - NULL_MOVE_REDUCTION, playerColor*-1, false);
            gameBoard.setCastleOrEnpas(castle);
            gameBoard.setZorHash(lastHash);
            if (eval >= beta) {
                return eval;
            }
        }
    }

    TranspositionTables::setOpen(gameBoard.getZorHash());
    u64 bestMove = 0;
    int lowScore = -1000000;

    while (true) {
        u64 nextMove = possibleMoves.getNextMove();
        if (nextMove != 0) {
            gameBoard.makeMov(nextMove);
            score = -AlphaBeta(gameBoard, -beta, -alpha, remainingDepth - 1, playerColor*-1, true);
            gameBoard.unMakeMov(nextMove, castle, enpasCol, lastHash, halfMoveNumber, lastTo);
            if (score != ILLEGAL_MOVE) {
                canMove = true;
                if (score >= beta){
                    TranspositionTables::setEntry(gameBoard.getZorHash(), nextMove, remainingDepth, beta, CUT_NODE);
                    KillerList[remainingDepth] = nextMove;
                    //addKiller(nextMove, remainingDepth);
                    return beta;
                }
                if (score > alpha){
                    alpha = score;
                }
                if (score > lowScore) {
                    lowScore = score;
                    bestMove = nextMove;
                }
            }
        } else {
            break;
        }
    }
    if (canMove == false) {
        TranspositionTables::unSetOpen(lastHash);
        if (gameBoard.inCheck(((playerColor == 1) ? WHITE_CODE : BLACK_CODE))) {
            return (-MATE_SCORE - remainingDepth);
        }
        return 0;
    }

    if (gameBoard.halfMoveClock >= 100) {
        TranspositionTables::unSetOpen(lastHash);
        return 0;
    }

    TranspositionTables::setEntry(gameBoard.getZorHash(), bestMove, remainingDepth, alpha, ALL_NODE);
    return alpha;
}

int Search::qSearch(Board& gameBoard, int alpha, int beta, int playerColor)
{
    //int stand_pat = (gameBoard.getEvaluation() + Evaluation::GetMobilityScore(gameBoard))*playerColor;
    int stand_pat = gameBoard.getEvaluation()*playerColor;


    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    CaptureList possibleCaptures = CaptureList(gameBoard, ((playerColor == 1) ? WHITE_CODE : BLACK_CODE));
    if (possibleCaptures.kingTake) {
        return -ILLEGAL_MOVE;
    }

    int score;
    u64 castle = gameBoard.getCastleOrEnpasent();
    u64 lastHash = gameBoard.getZorHash();
    int enpasCol = gameBoard.getEnpasentCol();
    int lastTo = gameBoard.getLastMove();
    int halfMoveNumber = gameBoard.halfMoveClock;

    while (true) {
        u64 nextCapture = possibleCaptures.getNextMove();
        if (nextCapture != 0) {
            gameBoard.makeMov(nextCapture);
            score = -qSearch(gameBoard, -beta, -alpha, playerColor*-1);
            gameBoard.unMakeMov(nextCapture, castle, enpasCol, lastHash, halfMoveNumber, lastTo);
            if (score != ILLEGAL_MOVE) {
                if (score >= beta){
                    return beta;
                }
                if (score > alpha){
                    alpha = score;
                }
            }
        } else {
            break;
        }
    }

    return alpha;
}

