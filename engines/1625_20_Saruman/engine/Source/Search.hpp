#ifndef SEARCH_H
#define SEARCH_H

using namespace std;

#include "Board.hpp"
#include "Evaluation.hpp"
#define NULL_MOVE_REDUCTION 2

#include <string>

/**
 * @class Search
 * @brief Searches the game tree for the best move
 *
 */

class Search
{
    private:
        static u64 KillerList[1024]; /**< Last killer by depth */
    public:
        static u64 nodes; /**< The number of nodes that have been searched at a given depth*/
        static u64 totalNodes; /** < The total number of nodes searched */
        /**
		 * @fn RootAlphaBeta
		 * @brief Performs an Alpha-Beta search starting at the root node
		 * @param gameBoard The game to search
		 * @param playerColor An int representing the color of the player to make a move for. 1 for white, -1 for black
		 * @param remainingDepth The depth to search to
		 * @param searchMoves a list of moves to search (if moves to search are being restricted)
		 * @return string, int String representing the best move and integer representing the score in centipawns
		 */
        static pair<string, int> RootAlphaBeta(Board gameBoard, int playerColor, int remainingDepth, std::vector<std::string> searchMoves);
        /**
		 * @fn AlphaBeta
		 * @brief Searches the game tree to a given depth
		 * @param gameBoard The game to search
		 * @param playerColor An int representing the color of the player to make a move for. 1 for white, -1 for black
		 * @param remainingDepth The depth to search to
		 * @return int The score of the position
		 */
        static int AlphaBeta(Board& gameBoard, int alpha, int beta, int remainingDepth, int playerColor, bool allowNullMove);
        /**
         * @fn qSearch
         * @brief Performs a quiescence search on a given position
		 * @param gameBoard The game to search
		 * @param playerColor An int representing the color of the player to make a move for. 1 for white, -1 for black
		 * @return int The score of the position
         */
        static int qSearch(Board& gameBoard, int alpha, int beta, int playerColor);
        static void clearKiller();
};

#endif // SEARCH_H
