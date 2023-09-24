#include "assert.h"
#include "constants.h"
#include "piece.h"

std::string Piece::GetPieceName(PieceId pieceId, Color playerSide) {
    std::string pieceName;
    if (playerSide == COLOR_WHITE)
        pieceName = WHITE_PIECE_NAMES[pieceId];
    else
        pieceName = BLACK_PIECE_NAMES[pieceId];

    return pieceName;
}

PieceId Piece::FromChar(char c) {

    for (int i=0; i<8;i++) {
        if (WHITE_PIECE_NAMES[i] == c || BLACK_PIECE_NAMES[i] == c)
            return static_cast<PieceId>(i);
    }
    // Unrecognized id
    assert(false);
}
