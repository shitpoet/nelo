#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "board.h"
#include "move.h"
#include "checkmove.h"
#include "attack.h"

// exclude kings
inline int isWhitePiece(unsigned char piece)
{
    if (piece <= 4)
        return 1;

    return 0;
}

inline int isBlackPiece(unsigned char piece)
{
    if (piece >= 6 && piece <= 10)
        return 1;

    return 0;
}

inline int isWhitePiece_withKing(unsigned char piece)
{
    if (piece <= 5)
        return 1;

    return 0;
}

inline int isBlackPiece_withKing(unsigned char piece)
{
    if (piece >= 6 && piece <= 11)
        return 1;

    return 0;
}

// 1 for legal move, 0 for illegal move
// king side castling for white king
inline int checkMove_wkingside(BOARD *pos)
{
    // check if king and rook have been moved
    if (pos->castle_flag & KSW)
    {
        // check if positions between king and rook are empty
        if (pos->board[7][5] == __ && pos->board[7][6] == __)
        {
            // check if king is in check and the two other spots are threatened
            if (!ifCheck(pos, -1) && !isThreatened(pos->board, 7, 5, -1) && !isThreatened(pos->board, 7, 6, -1))
            {
                return 1;
            }
        }
    }

    return 0;
}

// queen side castling for white king
inline int checkMove_wqueenside(BOARD *pos)
{
    // check if king and rook have been moved
    if (pos->castle_flag & QSW)
    {
        // check if positions between king and rook are empty
        if (pos->board[7][1] == __ && pos->board[7][2] == __ && pos->board[7][3] == __)
        {
            // check if king is in check and the two other spots are threatened
            if (!ifCheck(pos, -1) && !isThreatened(pos->board, 7, 2, -1) && !isThreatened(pos->board, 7, 3, -1))
            {
                return 1;
            }
        }
    }

    return 0;
}

// king side castling for black king
inline int checkMove_bkingside(BOARD *pos)
{
    // check if king and rook have been moved
    if (pos->castle_flag & KSB)
    {
        // check if positions between king and rook are empty
        if (pos->board[0][5] == __ && pos->board[0][6] == __)
        {
            // check if king is in check and the two other spots are threatened
            if (!ifCheck(pos, 1) && !isThreatened(pos->board, 0, 5, 1) && !isThreatened(pos->board, 0, 6, 1))
            {
                return 1;
            }
        }
    }

    return 0;
}

// queen side castling for black king
inline int checkMove_bqueenside(BOARD *pos)
{
    // check if king and rook have been moved
    if (pos->castle_flag & QSB)
    {
        // check if positions between king and rook are empty
        if (pos->board[0][1] == __ && pos->board[0][2] == __ && pos->board[0][3] == __)
        {
            // check if king is in check and the two other spots are threatened
            if (!ifCheck(pos, 1) && !isThreatened(pos->board, 0, 2, 1) && !isThreatened(pos->board, 0, 3, 1))
            {
                return 1;
            }
        }
    }

    return 0;
}

// for sliders, check if a square in the route is occupied
static inline int isOccupied_horizontal(unsigned char board[8][8], int index_x, int index_y, int new_y)
{
    // left
    if (index_y > new_y)
    {
        for (int i = index_y - 1; i > new_y; i--)
        {
            if (board[index_x][i] != __)
            {
                return 1;
            }
        }
    }
    // right
    else
    {
        for (int i = index_y + 1; i < new_y; i++)
        {
            if (board[index_x][i] != __)
            {
                return 1;
            }
        }
    }

    return 0;
}

static inline int isOccupied_vertical(unsigned char board[8][8], int index_x, int index_y, int new_x)
{
    // up
    if (index_x > new_x)
    {
        for (int i = index_x - 1; i > new_x; i--)
        {
            if (board[i][index_y] != __)
            {
                return 1;
            }
        }
    }
    // down
    else
    {
        for (int i = index_x + 1; i < new_x; i++)
        {
            if (board[i][index_y] != __)
            {
                return 1;
            }
        }
    }

    return 0;
}

static inline int isOccupied_diagonal(unsigned char board[8][8], int index_x, int index_y, int new_x, int new_y)
{
    if (index_x > new_x)
    {
        // up left
        if (index_y > new_y)
        {
            for (int i = index_x - new_x - 1; i > 0; i--)
            {
                if (board[index_x - i][index_y - i] != __)
                {
                    return 1;
                }
            }
        }
        // up right
        else
        {
            for (int i = index_x - new_x - 1; i > 0; i--)
            {
                if (board[index_x - i][index_y + i] != __)
                {
                    return 1;
                }
            }
        }
    }
    else
    {
        // down left
        if (index_y > new_y)
        {
            for (int i = new_x - index_x - 1; i > 0; i--)
            {
                if (board[index_x + i][index_y - i] != __)
                {
                    return 1;
                }
            }
        }
        // down right
        else
        {
            for (int i = new_x - index_x - 1; i > 0; i--)
            {
                if (board[index_x + i][index_y + i] != __)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

// check if this move is pseudo-legal
int isPseudoLegal(BOARD *pos, MOVE *move, int color)
{
    int cur_64 = move->from;
    int new_64 = move->to;
    int cur_x = cur_64 / 8;
    int cur_y = cur_64 % 8;
    int new_x = new_64 / 8;
    int new_y = new_64 % 8;
    unsigned char piece = pos->board[cur_x][cur_y];
    unsigned char op_piece = pos->board[new_x][new_y];

    // quick check
    // illegal if move a piece that is not ours or attack our pieces or attack enemy king
    if (color == 1) // black
    {
        if (!isBlackPiece_withKing(piece) || isBlackPiece_withKing(op_piece) || op_piece == wK)
            return 0;
    }
    else // white
    {
        if (!isWhitePiece_withKing(piece) || isWhitePiece_withKing(op_piece) || op_piece == bK)
            return 0;
    }

    switch (piece)
    {
        case wP:
            if (cur_y == new_y)
            {
                if (cur_x == 6 && new_x == 4)
                {
                    if (pos->board[5][cur_y] == __ && pos->board[4][cur_y] == __)
                        return 1;
                }
                else if (cur_x - new_x == 1)
                {
                    if (op_piece == __)
                        return 1;
                }
            }
            else if (new_x == cur_x - 1 && abs(new_y - cur_y) == 1)
            {
                // en passant
                if (op_piece == __)
                {
                    if (pos->ep_file == new_y + 1 && new_x == 2)
                        return 1;
                }
                else // capture
                {
                    return 1;
                }
            }
            break;
        case bP:
            if (cur_y == new_y)
            {
                if (cur_x == 1 && new_x == 3)
                {
                    if (pos->board[2][cur_y] == __ && pos->board[3][cur_y] == __)
                        return 1;
                }
                else if (new_x - cur_x == 1)
                {
                    if (op_piece == __)
                        return 1;
                }
            }
            else if (new_x == cur_x + 1 && abs(new_y - cur_y) == 1)
            {
                // en passant
                if (op_piece == __)
                {
                    if (pos->ep_file == new_y + 1 && new_x == 5)
                        return 1;
                }
                else // capture
                {
                    return 1;
                }
            }
            break;
        case wN:
        case bN:
            if ((abs(new_x - cur_x) == 2 && abs(new_y - cur_y) == 1) || (abs(new_x - cur_x) == 1 && abs(new_y - cur_y) == 2))
            {
                return 1;
            }
            break;
        case wR:
        case bR:
            // don't worry about moving to the same square since it's covered above
            if (cur_x == new_x)
            {
                if (!isOccupied_horizontal(pos->board, cur_x, cur_y, new_y))
                    return 1;
            }
            else if (cur_y == new_y)
            {
                if (!isOccupied_vertical(pos->board, cur_x, cur_y, new_x))
                    return 1;
            }
            break;
        case wB:
        case bB:
            if (abs(cur_x - new_x) == abs(cur_y - new_y))
            {
                if (!isOccupied_diagonal(pos->board, cur_x, cur_y, new_x, new_y))
                    return 1;
            }
            break;
        case wQ:
        case bQ:
            if (abs(cur_x - new_x) == abs(cur_y - new_y))
            {
                if (!isOccupied_diagonal(pos->board, cur_x, cur_y, new_x, new_y))
                    return 1;
            }
            else if (cur_x == new_x)
            {
                if (!isOccupied_horizontal(pos->board, cur_x, cur_y, new_y))
                    return 1;
            }
            else if (cur_y == new_y)
            {
                if (!isOccupied_vertical(pos->board, cur_x, cur_y, new_x))
                    return 1;
            }
            break;
        case wK:
            if (abs(cur_x - new_x) <= 1 && abs(cur_y - new_y) <= 1) // normal move
            {
                return 1;
            }
            // castling
            else if (cur_64 == e1 && new_64 == g1)
            {
                if (checkMove_wkingside(pos))
                    return 1;
            }
            else if (cur_64 == e1 && new_64 == c1)
            {
                if (checkMove_wqueenside(pos))
                    return 1;
            }
            break;
        case bK:
            if (abs(cur_x - new_x) <= 1 && abs(cur_y - new_y) <= 1) // normal move
            {
                return 1;
            }
            // castling
            else if (cur_64 == e8 && new_64 == g8)
            {
                if (checkMove_bkingside(pos))
                    return 1;
            }
            else if (cur_64 == e8 && new_64 == c8)
            {
                if (checkMove_bqueenside(pos))
                    return 1;
            }
            break;
    }

    return 0;
}