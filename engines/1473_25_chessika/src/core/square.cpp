#include <iostream>
#include "square.h"

Square SquareTool::FromCoords(const char *displayCoords) {

    int file = 0;
    for (int i=0; i<BOARD_WIDTH;i++) {
        if (FILE_INDEXES[i] == displayCoords[0])
            break;
        file += 1;
    }

    int row = 0;
    for (int i=0; i<BOARD_WIDTH;i++) {
        if (ROW_INDEXES[i] == displayCoords[1]) 
            break;
        row += 1;
    }

    return (row * 8 + file);
}



std::string SquareTool::ToString(Square sq) {
    std::ostringstream s;
    s << FILE_INDEXES[FILE_LUT[sq]];
    s << ROW_INDEXES[ROW_LUT[sq]];
    return s.str();
}

