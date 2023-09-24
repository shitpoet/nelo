#ifndef SQUARE_H
#define SQUARE_H

#include <sstream>
#include <vector>
#include "assert.h"
#include "constants.h"

typedef int Square;
static const short NO_EP = -1;

const std::vector<int> FILE_LUT = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7
};

const std::vector<int> ROW_LUT = {
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7
};

const Square SQ_LUT[8][8] = {  
    { 0,  1,  2,  3,  4,  5,  6,  7 },
    { 8,  9,  10, 11, 12, 13, 14, 15 },
    { 16, 17, 18, 19, 20, 21, 22, 23 },
    { 24, 25, 26, 27, 28, 29, 30, 31 },
    { 32, 33, 34, 35, 36, 37, 38, 39 },
    { 40, 41, 42, 43, 44, 45, 46, 47 },
    { 48, 49, 50, 51, 52, 53, 54, 55 },
    { 56, 57, 58, 59, 60, 61, 62, 63 }
};

class SquareTool {

    public:
        static Square FromCoords(const char *displayCoords);
        static std::string ToString(Square);
};

#endif

