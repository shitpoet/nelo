#ifndef PIECE_H
#define PIECE_H

#include <string>

class Piece {
    public:
        static std::string GetPieceName(PieceId, Color);
        static PieceId FromChar(char);
};

#endif
