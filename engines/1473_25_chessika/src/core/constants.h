#ifndef CONSTANTS_H
#define CONSTANTS_H

// We're choosing the LERF implementation as described on chessprogramming wiki

/*
          noWe    no    noEa                 noNoWe    noNoEa
            +7    +8    +9                      +15  +17
                                                 |     |
            We          Ea          noWeWe  +6 __|     |__+10  noEaEa
            -1          1                         \   /
                                                   >0<
          soWe    so    soEa                   __ /   \ __
            -9    -8    -7          soWeWe -10   |     |   -6  soEaEa
                                                 |     |
                                                -17  -15
                                            soSoWe    soSoEa
*/

typedef unsigned long int U64;

enum BitSquare {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};



enum Direction {
    // General directions
    DIR_NW=7,  DIR_N=8,  DIR_NE=9,
    DIR_W=-1,            DIR_E=1,
    DIR_SW=-9, DIR_S=-8, DIR_SE=-7,

    // Knights directions
    DIR_NNW=15, DIR_NNE=17,
    DIR_NWW=6, DIR_NEE=10,
    DIR_SWW=-10, DIR_SEE=-6,
    DIR_SSW=-17, DIR_SSE=-15
};



static const short BOARD_WIDTH = 8;
static const char  FILE_INDEXES[] = "abcdefgh";
static const char  ROW_INDEXES[] = "12345678";

static const int CHECKMATE_VALUE = 1000;

enum Color { COLOR_WHITE=1, COLOR_BLACK=0, COLOR_EMPTY=-1 };
enum PieceId : unsigned int { EMPTY_ID=0, PAWN_ID=1, KNIGHT_ID=2, BISHOP_ID=3, ROOK_ID=4, QUEEN_ID=5, KING_ID=6 };
static const int PIECE_VALUES[] = { 0, 100, 320, 330, 500, 900, 20000, 0 };
static const char WHITE_PIECE_NAMES[] = ".PNBRQKx";
static const char BLACK_PIECE_NAMES[] = ".pnbrqkx";

static const float INF = 1000000;
static const float VALUE_MATE = 100000;

enum TurnStatus { LEGAL_MOVE, MOVE_DOESNT_EXIST, ILLEGAL_MOVE, CHECKMATE_P1, CHECKMATE_P2, CHECK_P1, CHECK_P2, STALEMATE, TIMEOUT };
static const char TurnStrings[][20] { "LEGAL_MOVE", "MOVE_DOESNT_EXIST", "ILLEGAL_MOVE", "CHECKMATE_P1", "CHECKMATE_P2", "CHECK_P1", "CHECK_P2", "STALEMATE", "TIMEOUT"};

// The following constants have been generated in bitboard.cc
const U64 NOT_A_FILE = 0xfefefefefefefefe;
const U64 NOT_B_FILE = 0xfdfdfdfdfdfdfdfd;
const U64 NOT_G_FILE = 0xbfbfbfbfbfbfbfbf;
const U64 NOT_H_FILE = 0x7f7f7f7f7f7f7f7f;
const U64 H_FILE = 0x8080808080808080;
const U64 A_FILE = 0x0101010101010101;
const U64 B_FILE = 0x0202020202020202;

const U64 KING_MOVES[64] = {
    0x0000000000000302, 0x0000000000000705, 0x0000000000000e0a, 0x0000000000001c14, 0x0000000000003828, 0x0000000000007050, 0x000000000000e0a0, 0x000000000000c040, 
    0x0000000000030203, 0x0000000000070507, 0x00000000000e0a0e, 0x00000000001c141c, 0x0000000000382838, 0x0000000000705070, 0x0000000000e0a0e0, 0x0000000000c040c0, 
    0x0000000003020300, 0x0000000007050700, 0x000000000e0a0e00, 0x000000001c141c00, 0x0000000038283800, 0x0000000070507000, 0x00000000e0a0e000, 0x00000000c040c000, 
    0x0000000302030000, 0x0000000705070000, 0x0000000e0a0e0000, 0x0000001c141c0000, 0x0000003828380000, 0x0000007050700000, 0x000000e0a0e00000, 0x000000c040c00000, 
    0x0000030203000000, 0x0000070507000000, 0x00000e0a0e000000, 0x00001c141c000000, 0x0000382838000000, 0x0000705070000000, 0x0000e0a0e0000000, 0x0000c040c0000000, 
    0x0003020300000000, 0x0007050700000000, 0x000e0a0e00000000, 0x001c141c00000000, 0x0038283800000000, 0x0070507000000000, 0x00e0a0e000000000, 0x00c040c000000000, 
    0x0302030000000000, 0x0705070000000000, 0x0e0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000, 
    0x0203000000000000, 0x0507000000000000, 0x0a0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000
};

const U64 KNIGHT_MOVES[64] = {
    0x0000000000020400, 0x0000000000050800, 0x00000000000a1100, 0x0000000000142200, 0x0000000000284400, 0x0000000000508800, 0x0000000000a01000, 0x0000000000402000, 
    0x0000000002040004, 0x0000000005080008, 0x000000000a110011, 0x0000000014220022, 0x0000000028440044, 0x0000000050880088, 0x00000000a0100010, 0x0000000040200020, 
    0x0000000204000402, 0x0000000508000805, 0x0000000a1100110a, 0x0000001422002214, 0x0000002844004428, 0x0000005088008850, 0x000000a0100010a0, 0x0000004020002040, 
    0x0000020400040200, 0x0000050800080500, 0x00000a1100110a00, 0x0000142200221400, 0x0000284400442800, 0x0000508800885000, 0x0000a0100010a000, 0x0000402000204000, 
    0x0002040004020000, 0x0005080008050000, 0x000a1100110a0000, 0x0014220022140000, 0x0028440044280000, 0x0050880088500000, 0x00a0100010a00000, 0x0040200020400000, 
    0x0204000402000000, 0x0508000805000000, 0x0a1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000, 
    0x0400040200000000, 0x0800080500000000, 0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000, 
    0x0004020000000000, 0x0008050000000000, 0x00110a0000000000, 0x0022140000000000, 0x0044280000000000, 0x0088500000000000, 0x0010a00000000000, 0x0020400000000000
};

const U64 NORTH_ATTACKS[64] = {
    0x0101010101010100, 0x0202020202020200, 0x0404040404040400, 0x0808080808080800, 0x1010101010101000, 0x2020202020202000, 0x4040404040404000, 0x8080808080808000, 
    0x0101010101010000, 0x0202020202020000, 0x0404040404040000, 0x0808080808080000, 0x1010101010100000, 0x2020202020200000, 0x4040404040400000, 0x8080808080800000, 
    0x0101010101000000, 0x0202020202000000, 0x0404040404000000, 0x0808080808000000, 0x1010101010000000, 0x2020202020000000, 0x4040404040000000, 0x8080808080000000, 
    0x0101010100000000, 0x0202020200000000, 0x0404040400000000, 0x0808080800000000, 0x1010101000000000, 0x2020202000000000, 0x4040404000000000, 0x8080808000000000, 
    0x0101010000000000, 0x0202020000000000, 0x0404040000000000, 0x0808080000000000, 0x1010100000000000, 0x2020200000000000, 0x4040400000000000, 0x8080800000000000, 
    0x0101000000000000, 0x0202000000000000, 0x0404000000000000, 0x0808000000000000, 0x1010000000000000, 0x2020000000000000, 0x4040000000000000, 0x8080000000000000, 
    0x0100000000000000, 0x0200000000000000, 0x0400000000000000, 0x0800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000, 
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000 
};

const U64 SOUTH_ATTACKS[64] = {
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 
    0x0000000000000001, 0x0000000000000002, 0x0000000000000004, 0x0000000000000008, 0x0000000000000010, 0x0000000000000020, 0x0000000000000040, 0x0000000000000080, 
    0x0000000000000101, 0x0000000000000202, 0x0000000000000404, 0x0000000000000808, 0x0000000000001010, 0x0000000000002020, 0x0000000000004040, 0x0000000000008080, 
    0x0000000000010101, 0x0000000000020202, 0x0000000000040404, 0x0000000000080808, 0x0000000000101010, 0x0000000000202020, 0x0000000000404040, 0x0000000000808080, 
    0x0000000001010101, 0x0000000002020202, 0x0000000004040404, 0x0000000008080808, 0x0000000010101010, 0x0000000020202020, 0x0000000040404040, 0x0000000080808080, 
    0x0000000101010101, 0x0000000202020202, 0x0000000404040404, 0x0000000808080808, 0x0000001010101010, 0x0000002020202020, 0x0000004040404040, 0x0000008080808080, 
    0x0000010101010101, 0x0000020202020202, 0x0000040404040404, 0x0000080808080808, 0x0000101010101010, 0x0000202020202020, 0x0000404040404040, 0x0000808080808080, 
    0x0001010101010101, 0x0002020202020202, 0x0004040404040404, 0x0008080808080808, 0x0010101010101010, 0x0020202020202020, 0x0040404040404040, 0x0080808080808080
};

const U64 EAST_ATTACKS[64] = {
    0x00000000000000fe, 0x00000000000000fc, 0x00000000000000f8, 0x00000000000000f0, 0x00000000000000e0, 0x00000000000000c0, 0x0000000000000080, 0x0000000000000000, 
    0x000000000000fe00, 0x000000000000fc00, 0x000000000000f800, 0x000000000000f000, 0x000000000000e000, 0x000000000000c000, 0x0000000000008000, 0x0000000000000000, 
    0x0000000000fe0000, 0x0000000000fc0000, 0x0000000000f80000, 0x0000000000f00000, 0x0000000000e00000, 0x0000000000c00000, 0x0000000000800000, 0x0000000000000000, 
    0x00000000fe000000, 0x00000000fc000000, 0x00000000f8000000, 0x00000000f0000000, 0x00000000e0000000, 0x00000000c0000000, 0x0000000080000000, 0x0000000000000000, 
    0x000000fe00000000, 0x000000fc00000000, 0x000000f800000000, 0x000000f000000000, 0x000000e000000000, 0x000000c000000000, 0x0000008000000000, 0x0000000000000000, 
    0x0000fe0000000000, 0x0000fc0000000000, 0x0000f80000000000, 0x0000f00000000000, 0x0000e00000000000, 0x0000c00000000000, 0x0000800000000000, 0x0000000000000000, 
    0x00fe000000000000, 0x00fc000000000000, 0x00f8000000000000, 0x00f0000000000000, 0x00e0000000000000, 0x00c0000000000000, 0x0080000000000000, 0x0000000000000000, 
    0xfe00000000000000, 0xfc00000000000000, 0xf800000000000000, 0xf000000000000000, 0xe000000000000000, 0xc000000000000000, 0x8000000000000000, 0x0000000000000000
};

const U64 WEST_ATTACKS[64] = {
    0x0000000000000000, 0x0000000000000001, 0x0000000000000003, 0x0000000000000007, 0x000000000000000f, 0x000000000000001f, 0x000000000000003f, 0x000000000000007f, 
    0x0000000000000000, 0x0000000000000100, 0x0000000000000300, 0x0000000000000700, 0x0000000000000f00, 0x0000000000001f00, 0x0000000000003f00, 0x0000000000007f00, 
    0x0000000000000000, 0x0000000000010000, 0x0000000000030000, 0x0000000000070000, 0x00000000000f0000, 0x00000000001f0000, 0x00000000003f0000, 0x00000000007f0000, 
    0x0000000000000000, 0x0000000001000000, 0x0000000003000000, 0x0000000007000000, 0x000000000f000000, 0x000000001f000000, 0x000000003f000000, 0x000000007f000000, 
    0x0000000000000000, 0x0000000100000000, 0x0000000300000000, 0x0000000700000000, 0x0000000f00000000, 0x0000001f00000000, 0x0000003f00000000, 0x0000007f00000000, 
    0x0000000000000000, 0x0000010000000000, 0x0000030000000000, 0x0000070000000000, 0x00000f0000000000, 0x00001f0000000000, 0x00003f0000000000, 0x00007f0000000000, 
    0x0000000000000000, 0x0001000000000000, 0x0003000000000000, 0x0007000000000000, 0x000f000000000000, 0x001f000000000000, 0x003f000000000000, 0x007f000000000000, 
    0x0000000000000000, 0x0100000000000000, 0x0300000000000000, 0x0700000000000000, 0x0f00000000000000, 0x1f00000000000000, 0x3f00000000000000, 0x7f00000000000000
};

const U64 NORTH_EAST_ATTACKS[64] = {
    0x8040201008040200, 0x0080402010080400, 0x0000804020100800, 0x0000008040201000, 0x0000000080402000, 0x0000000000804000, 0x0000000000008000, 0x0000000000000000, 
    0x4020100804020000, 0x8040201008040000, 0x0080402010080000, 0x0000804020100000, 0x0000008040200000, 0x0000000080400000, 0x0000000000800000, 0x0000000000000000, 
    0x2010080402000000, 0x4020100804000000, 0x8040201008000000, 0x0080402010000000, 0x0000804020000000, 0x0000008040000000, 0x0000000080000000, 0x0000000000000000, 
    0x1008040200000000, 0x2010080400000000, 0x4020100800000000, 0x8040201000000000, 0x0080402000000000, 0x0000804000000000, 0x0000008000000000, 0x0000000000000000, 
    0x0804020000000000, 0x1008040000000000, 0x2010080000000000, 0x4020100000000000, 0x8040200000000000, 0x0080400000000000, 0x0000800000000000, 0x0000000000000000, 
    0x0402000000000000, 0x0804000000000000, 0x1008000000000000, 0x2010000000000000, 0x4020000000000000, 0x8040000000000000, 0x0080000000000000, 0x0000000000000000, 
    0x0200000000000000, 0x0400000000000000, 0x0800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
};

const U64 NORTH_WEST_ATTACKS[64] = {
    0x0000000000000000, 0x0000000000000100, 0x0000000000010200, 0x0000000001020400, 0x0000000102040800, 0x0000010204081000, 0x0001020408102000, 0x0102040810204000, 
    0x0000000000000000, 0x0000000000010000, 0x0000000001020000, 0x0000000102040000, 0x0000010204080000, 0x0001020408100000, 0x0102040810200000, 0x0204081020400000, 
    0x0000000000000000, 0x0000000001000000, 0x0000000102000000, 0x0000010204000000, 0x0001020408000000, 0x0102040810000000, 0x0204081020000000, 0x0408102040000000, 
    0x0000000000000000, 0x0000000100000000, 0x0000010200000000, 0x0001020400000000, 0x0102040800000000, 0x0204081000000000, 0x0408102000000000, 0x0810204000000000, 
    0x0000000000000000, 0x0000010000000000, 0x0001020000000000, 0x0102040000000000, 0x0204080000000000, 0x0408100000000000, 0x0810200000000000, 0x1020400000000000, 
    0x0000000000000000, 0x0001000000000000, 0x0102000000000000, 0x0204000000000000, 0x0408000000000000, 0x0810000000000000, 0x1020000000000000, 0x2040000000000000, 
    0x0000000000000000, 0x0100000000000000, 0x0200000000000000, 0x0400000000000000, 0x0800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
};

const U64 SOUTH_WEST_ATTACKS[64] = {
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 
    0x0000000000000000, 0x0000000000000001, 0x0000000000000002, 0x0000000000000004, 0x0000000000000008, 0x0000000000000010, 0x0000000000000020, 0x0000000000000040, 
    0x0000000000000000, 0x0000000000000100, 0x0000000000000201, 0x0000000000000402, 0x0000000000000804, 0x0000000000001008, 0x0000000000002010, 0x0000000000004020, 
    0x0000000000000000, 0x0000000000010000, 0x0000000000020100, 0x0000000000040201, 0x0000000000080402, 0x0000000000100804, 0x0000000000201008, 0x0000000000402010, 
    0x0000000000000000, 0x0000000001000000, 0x0000000002010000, 0x0000000004020100, 0x0000000008040201, 0x0000000010080402, 0x0000000020100804, 0x0000000040201008, 
    0x0000000000000000, 0x0000000100000000, 0x0000000201000000, 0x0000000402010000, 0x0000000804020100, 0x0000001008040201, 0x0000002010080402, 0x0000004020100804, 
    0x0000000000000000, 0x0000010000000000, 0x0000020100000000, 0x0000040201000000, 0x0000080402010000, 0x0000100804020100, 0x0000201008040201, 0x0000402010080402, 
    0x0000000000000000, 0x0001000000000000, 0x0002010000000000, 0x0004020100000000, 0x0008040201000000, 0x0010080402010000, 0x0020100804020100, 0x0040201008040201
};

const U64 SOUTH_EAST_ATTACKS[64] = {
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000002, 0x0000000000000004, 0x0000000000000008, 0x0000000000000010, 0x0000000000000020, 0x0000000000000040, 0x0000000000000080, 0x0000000000000000, 
    0x0000000000000204, 0x0000000000000408, 0x0000000000000810, 0x0000000000001020, 0x0000000000002040, 0x0000000000004080, 0x0000000000008000, 0x0000000000000000, 
    0x0000000000020408, 0x0000000000040810, 0x0000000000081020, 0x0000000000102040, 0x0000000000204080, 0x0000000000408000, 0x0000000000800000, 0x0000000000000000, 
    0x0000000002040810, 0x0000000004081020, 0x0000000008102040, 0x0000000010204080, 0x0000000020408000, 0x0000000040800000, 0x0000000080000000, 0x0000000000000000, 
    0x0000000204081020, 0x0000000408102040, 0x0000000810204080, 0x0000001020408000, 0x0000002040800000, 0x0000004080000000, 0x0000008000000000, 0x0000000000000000, 
    0x0000020408102040, 0x0000040810204080, 0x0000081020408000, 0x0000102040800000, 0x0000204080000000, 0x0000408000000000, 0x0000800000000000, 0x0000000000000000, 
    0x0002040810204080, 0x0004081020408000, 0x0008102040800000, 0x0010204080000000, 0x0020408000000000, 0x0040800000000000, 0x0080000000000000, 0x0000000000000000
};

const U64 RANK_MASKS_EX[64] = {
    0x00000000000000fe, 0x00000000000000fd, 0x00000000000000fb, 0x00000000000000f7, 0x00000000000000ef, 0x00000000000000df, 0x00000000000000bf, 0x000000000000007f, 
    0x000000000000fe00, 0x000000000000fd00, 0x000000000000fb00, 0x000000000000f700, 0x000000000000ef00, 0x000000000000df00, 0x000000000000bf00, 0x0000000000007f00, 
    0x0000000000fe0000, 0x0000000000fd0000, 0x0000000000fb0000, 0x0000000000f70000, 0x0000000000ef0000, 0x0000000000df0000, 0x0000000000bf0000, 0x00000000007f0000, 
    0x00000000fe000000, 0x00000000fd000000, 0x00000000fb000000, 0x00000000f7000000, 0x00000000ef000000, 0x00000000df000000, 0x00000000bf000000, 0x000000007f000000, 
    0x000000fe00000000, 0x000000fd00000000, 0x000000fb00000000, 0x000000f700000000, 0x000000ef00000000, 0x000000df00000000, 0x000000bf00000000, 0x0000007f00000000, 
    0x0000fe0000000000, 0x0000fd0000000000, 0x0000fb0000000000, 0x0000f70000000000, 0x0000ef0000000000, 0x0000df0000000000, 0x0000bf0000000000, 0x00007f0000000000, 
    0x00fe000000000000, 0x00fd000000000000, 0x00fb000000000000, 0x00f7000000000000, 0x00ef000000000000, 0x00df000000000000, 0x00bf000000000000, 0x007f000000000000, 
    0xfe00000000000000, 0xfd00000000000000, 0xfb00000000000000, 0xf700000000000000, 0xef00000000000000, 0xdf00000000000000, 0xbf00000000000000, 0x7f00000000000000
};

const U64 DIAG_MASKS_EX[64] = {
    0x8040201008040200, 0x0080402010080400, 0x0000804020100800, 0x0000008040201000, 0x0000000080402000, 0x0000000000804000, 0x0000000000008000, 0x0000000000000000, 
    0x4020100804020000, 0x8040201008040001, 0x0080402010080002, 0x0000804020100004, 0x0000008040200008, 0x0000000080400010, 0x0000000000800020, 0x0000000000000040, 
    0x2010080402000000, 0x4020100804000100, 0x8040201008000201, 0x0080402010000402, 0x0000804020000804, 0x0000008040001008, 0x0000000080002010, 0x0000000000004020, 
    0x1008040200000000, 0x2010080400010000, 0x4020100800020100, 0x8040201000040201, 0x0080402000080402, 0x0000804000100804, 0x0000008000201008, 0x0000000000402010, 
    0x0804020000000000, 0x1008040001000000, 0x2010080002010000, 0x4020100004020100, 0x8040200008040201, 0x0080400010080402, 0x0000800020100804, 0x0000000040201008, 
    0x0402000000000000, 0x0804000100000000, 0x1008000201000000, 0x2010000402010000, 0x4020000804020100, 0x8040001008040201, 0x0080002010080402, 0x0000004020100804, 
    0x0200000000000000, 0x0400010000000000, 0x0800020100000000, 0x1000040201000000, 0x2000080402010000, 0x4000100804020100, 0x8000201008040201, 0x0000402010080402, 
    0x0000000000000000, 0x0001000000000000, 0x0002010000000000, 0x0004020100000000, 0x0008040201000000, 0x0010080402010000, 0x0020100804020100, 0x0040201008040201
};

const U64 ANTI_DIAG_MASKS_EX[64] = {
    0x0000000000000000, 0x0000000000000100, 0x0000000000010200, 0x0000000001020400, 0x0000000102040800, 0x0000010204081000, 0x0001020408102000, 0x0102040810204000, 
    0x0000000000000002, 0x0000000000010004, 0x0000000001020008, 0x0000000102040010, 0x0000010204080020, 0x0001020408100040, 0x0102040810200080, 0x0204081020400000, 
    0x0000000000000204, 0x0000000001000408, 0x0000000102000810, 0x0000010204001020, 0x0001020408002040, 0x0102040810004080, 0x0204081020008000, 0x0408102040000000, 
    0x0000000000020408, 0x0000000100040810, 0x0000010200081020, 0x0001020400102040, 0x0102040800204080, 0x0204081000408000, 0x0408102000800000, 0x0810204000000000, 
    0x0000000002040810, 0x0000010004081020, 0x0001020008102040, 0x0102040010204080, 0x0204080020408000, 0x0408100040800000, 0x0810200080000000, 0x1020400000000000, 
    0x0000000204081020, 0x0001000408102040, 0x0102000810204080, 0x0204001020408000, 0x0408002040800000, 0x0810004080000000, 0x1020008000000000, 0x2040000000000000, 
    0x0000020408102040, 0x0100040810204080, 0x0200081020408000, 0x0400102040800000, 0x0800204080000000, 0x1000408000000000, 0x2000800000000000, 0x4000000000000000, 
    0x0002040810204080, 0x0004081020408000, 0x0008102040800000, 0x0010204080000000, 0x0020408000000000, 0x0040800000000000, 0x0080000000000000, 0x0000000000000000
};

const U64 FILE_MASKS[64] = {
    0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080, 
    0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080, 
    0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080, 
    0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080, 
    0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080, 
    0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080, 
    0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080, 
    0x0101010101010101, 0x0202020202020202, 0x0404040404040404, 0x0808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080
};

const U64 RANK_MASKS[64] = {
    0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 0x00000000000000ff, 
    0x000000000000ff00, 0x000000000000ff00, 0x000000000000ff00, 0x000000000000ff00, 0x000000000000ff00, 0x000000000000ff00, 0x000000000000ff00, 0x000000000000ff00, 
    0x0000000000ff0000, 0x0000000000ff0000, 0x0000000000ff0000, 0x0000000000ff0000, 0x0000000000ff0000, 0x0000000000ff0000, 0x0000000000ff0000, 0x0000000000ff0000, 
    0x00000000ff000000, 0x00000000ff000000, 0x00000000ff000000, 0x00000000ff000000, 0x00000000ff000000, 0x00000000ff000000, 0x00000000ff000000, 0x00000000ff000000, 
    0x000000ff00000000, 0x000000ff00000000, 0x000000ff00000000, 0x000000ff00000000, 0x000000ff00000000, 0x000000ff00000000, 0x000000ff00000000, 0x000000ff00000000, 
    0x0000ff0000000000, 0x0000ff0000000000, 0x0000ff0000000000, 0x0000ff0000000000, 0x0000ff0000000000, 0x0000ff0000000000, 0x0000ff0000000000, 0x0000ff0000000000, 
    0x00ff000000000000, 0x00ff000000000000, 0x00ff000000000000, 0x00ff000000000000, 0x00ff000000000000, 0x00ff000000000000, 0x00ff000000000000, 0x00ff000000000000, 
    0xff00000000000000, 0xff00000000000000, 0xff00000000000000, 0xff00000000000000, 0xff00000000000000, 0xff00000000000000, 0xff00000000000000, 0xff00000000000000
};


const U64 DIAG_MASKS[64] = {
    0x8040201008040201, 0x0080402010080402, 0x0000804020100804, 0x0000008040201008, 0x0000000080402010, 0x0000000000804020, 0x0000000000008040, 0x0000000000000080, 
    0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 0x0000804020100804, 0x0000008040201008, 0x0000000080402010, 0x0000000000804020, 0x0000000000008040, 
    0x2010080402010000, 0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 0x0000804020100804, 0x0000008040201008, 0x0000000080402010, 0x0000000000804020, 
    0x1008040201000000, 0x2010080402010000, 0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 0x0000804020100804, 0x0000008040201008, 0x0000000080402010, 
    0x0804020100000000, 0x1008040201000000, 0x2010080402010000, 0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 0x0000804020100804, 0x0000008040201008, 
    0x0402010000000000, 0x0804020100000000, 0x1008040201000000, 0x2010080402010000, 0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 0x0000804020100804, 
    0x0201000000000000, 0x0402010000000000, 0x0804020100000000, 0x1008040201000000, 0x2010080402010000, 0x4020100804020100, 0x8040201008040201, 0x0080402010080402, 
    0x0100000000000000, 0x0201000000000000, 0x0402010000000000, 0x0804020100000000, 0x1008040201000000, 0x2010080402010000, 0x4020100804020100, 0x8040201008040201,
};

const U64 ANTI_DIAG_MASKS[64] = {
    0x0000000000000001, 0x0000000000000102, 0x0000000000010204, 0x0000000001020408, 0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 
    0x0000000000000102, 0x0000000000010204, 0x0000000001020408, 0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 
    0x0000000000010204, 0x0000000001020408, 0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 
    0x0000000001020408, 0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 
    0x0000000102040810, 0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 0x1020408000000000, 
    0x0000010204081020, 0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 0x1020408000000000, 0x2040800000000000, 
    0x0001020408102040, 0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 0x1020408000000000, 0x2040800000000000, 0x4080000000000000, 
    0x0102040810204080, 0x0204081020408000, 0x0408102040800000, 0x0810204080000000, 0x1020408000000000, 0x2040800000000000, 0x4080000000000000, 0x8000000000000000
};

static U64 FILL_UP_ATTACKS[8][256];
static U64 Z_KEYS_PIECES[12][64];
static U64 Z_KEYS_BLACK_TO_MOVE;
static U64 Z_KEYS_CASTLING_RIGHTS[4];
static U64 Z_KEYS_EN_PASSANT[8];

const long int MICRO_SECS_TO_SECS = 1000000;

#endif