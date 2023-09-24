/**
 * Print King Attacks as hex
 */
void GenerateKingAttacks() {
    
    std::cout << "King attacks :";
    std::cout << std::hex;
    for (int i=0; i<64; i++) {
        if (i%8 == 0)
            std::cout << std::endl;

        U64 moves = 0;
        U64 current = 1UL << i;

        moves |= (current & NOT_A_FILE) >> abs(DIR_SW);
        moves |= (current >> abs(DIR_S));
        moves |= (current & NOT_H_FILE) >> abs(DIR_SE);
        moves |= (current & NOT_A_FILE) >> abs(DIR_W);

        moves |= (current & NOT_A_FILE) << DIR_NW;
        moves |= (current & NOT_H_FILE) << DIR_E;
        moves |= current << DIR_N;
        moves |= (current & NOT_H_FILE) << DIR_NE;

        //std::cout << BitBoard::Debug(moves) << std::endl;
        std::cout << "0x" << std::setfill('0') << std::setw(16) << moves << ", ";
    }
    std::cout << std::dec;
    std::cout << std::endl;
}


/**
 * Print Knight Attacks as hex
 */
void GenerateKnightAttacks() {

    std::cout << "Knight attacks :";
    std::cout << std::hex;
    for (int i=0; i<64; i++) {
        if (i%8 == 0)
            std::cout << std::endl;

        U64 moves = 0;
        U64 current = 1UL << i;

        moves |= (current & NOT_A_FILE) >> abs(DIR_SSW);
        moves |= (current & NOT_H_FILE) >> abs(DIR_SSE);
        moves |= (current & NOT_A_FILE & NOT_B_FILE) >> abs(DIR_SWW);
        moves |= (current & NOT_G_FILE & NOT_H_FILE) >> abs(DIR_SEE);

        moves |= (current & NOT_A_FILE) << DIR_NNW;
        moves |= (current & NOT_H_FILE) << DIR_NNE;
        moves |= (current & NOT_A_FILE & NOT_B_FILE) << DIR_NWW;
        moves |= (current & NOT_G_FILE & NOT_H_FILE) << DIR_NEE;

        //std::cout << std::endl << BitBoard::Debug(moves);
        std::cout << "0x" << std::setfill('0') << std::setw(16) << moves << ", ";
    }
    std::cout << std::dec;
    std::cout << std::endl;
}




void GenerateNorthEastAttacks() {

    std::cout << "North East ray attacks :" << std::endl;
    std::cout << std::hex;

    for(int i=0; i<64; i++) {

        // Creating a file mask
        U64 fileMask = 0;
        for (int z=0; z<64; z++) {
            if (z%8 == 0) 
                fileMask |= (1UL << z);
        }

        // Looking for the initial file iteration
        // I guess it could exist a better implementation
        int initialFile = 0;
        U64 tmp = 1UL<<i;
        while ((fileMask & tmp) == 0) {
            fileMask = fileMask << 1;
            initialFile++;
        }

        // Okay... now we don't need the bit on this initial file
        // And we skip the H file
        // We skip the 8th rank
        
        if (tmp & H_FILE || i>55) {
            tmp = 0;
        }
        else {
            tmp = tmp << 9;

            // Projecting ray

            for (int j=initialFile+2; j<8; j++) {
                tmp |= (tmp<<9);
            }
        }

        //std::cout << BitBoard::Debug(tmp) << std::endl;
        if (i%8 == 0)
            std::cout << std::endl;
        std::cout << "0x" << std::setfill('0') << std::setw(16) << tmp << ", ";

    }
    std::cout << std::endl;
    std::cout << std::dec;
}


void GenerateNorthWestAttacks() {

    U64 moves[64];

    for(int i=63; i>=0; i--) {

        // Creating a file mask
        U64 fileMask = 0;
        for (int z=0; z<64; z++) {
            if (z%8 == 0) 
                fileMask |= (1UL << z);
        }

        // Looking for the initial file iteration
        // I guess it could exist a better implementation
        int initialFile = 0;
        U64 tmp = 1UL<<i;
        while ((fileMask & tmp) == 0) {
            fileMask = fileMask << 1;
            initialFile++;
        }

        // Okay... now we don't need the bit on this initial file
        // And we skip the H file
        // We skip the 8th rank
        
        if (tmp & A_FILE || i>55) {
            tmp = 0;
        }
        else {
            tmp = tmp << 7;

            // Projecting ray

            for (int j=initialFile-1; j>0; j--) {
                tmp |= (tmp<<7);
            }
        }

        moves[i] = tmp;

        //std::cout << BitBoard::Debug(tmp) << std::endl;

    }

    std::cout << "North West ray attacks :\n";
    std::cout << std::hex;

    for(int i=0; i<64; i++) {
        if (i%8 == 0)
            std::cout << std::endl;
        std::cout << "0x" << std::setfill('0') << std::setw(16) << moves[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << std::dec;
}


void GenerateSouthWestAttacks() {

    std::cout << "South West ray attacks :\n";
    std::cout << std::hex;

    U64 moves[64];

    for(int i=63; i>=0; i--) {

        U64 fileMask = A_FILE;

        // Looking for the initial file iteration
        // I guess it could exist a better implementation
        int initialFile = 0;
        U64 tmp = 1UL<<i;
        while ((fileMask & tmp) == 0) {
            fileMask = fileMask << 1;
            initialFile++;
        }

        // Okay... now we don't need the bit on this initial file
        // And we skip the H file
        // We skip the 1st rank
        
        if (tmp & A_FILE || i<8) {
            tmp = 0;
        }
        else {
            tmp = tmp >> 9;

            // Projecting ray

            for (int j=initialFile-2; j>=0; j--) {
                tmp |= (tmp>>9);
            }
        }

        moves[i] = tmp;

        //std::cout << BitBoard::Debug(tmp) << std::endl;
    }

    std::cout << std::hex;

    for(int i=0; i<64; i++) {
        if (i%8 == 0)
            std::cout << std::endl;
        std::cout << "0x" << std::setfill('0') << std::setw(16) << moves[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << std::dec;
}


void GenerateSouthEastAttacks() {

    std::cout << "South East ray attacks :\n";
    std::cout << std::hex;

    for(int i=0; i<64; i++) {

        U64 fileMask = A_FILE;

        // Looking for the initial file iteration
        // I guess it could exist a better implementation
        int initialFile = 0;
        U64 tmp = 1UL<<i;
        while ((fileMask & tmp) == 0) {
            fileMask = fileMask << 1;
            initialFile++;
        }

        // Okay... now we don't need the bit on this initial file
        // And we skip the H file
        // We skip the 1st rank
        
        if (tmp & H_FILE || i<8) {
            tmp = 0;
        }
        else {
            tmp = tmp >> 7;

            // Projecting ray

            for (int j=initialFile+2; j<8; j++) {
                tmp |= (tmp>>7);
            }
        }

        //std::cout << BitBoard::Debug(tmp) << "\n";
        //continue;
        if (i%8 == 0)
            std::cout << std::endl;
        std::cout << "0x" << std::setfill('0') << std::setw(16) << tmp << ", ";

    }
    std::cout << std::endl;
    std::cout << std::dec;
}



void GenerateEastAttacks() {
    std::cout << "East ray attacks :";
    std::cout << std::hex;
    U64 eastAttacks[64];
    U64 eastRay =  0x00000000000000fe;
    U64 rankMask = 0x00000000000000ff;
    for(int i=0; i<64; i++, eastRay <<=1 ) {
        if (i%8 == 0)
            std::cout << std::endl;
        eastAttacks[i] = eastRay & (rankMask << ((i/8)*8));
        std::cout << BitBoard::Debug(eastAttacks[i]) << std::endl;
        //std::cout << "0x" << std::setfill('0') << std::setw(16) << eastAttacks[i] << ", ";
    }
    std::cout << std::dec;
    std::cout << std::endl;
}


void GenerateWestAttacks() {
    U64 westRay =  0x7f00000000000000;
    std::cout << "West ray attacks :\n";
    std::cout << std::hex;
    U64 westAttacks[64];
    U64 rankMask = 0x00000000000000ff;
    for(int i=63; i>=0; i--, westRay >>=1 ) {
        westAttacks[i] = westRay & (rankMask << ((i/8)*8));
        /* std::cout << "ray : \n"; */
        /* std::cout << BitBoard::Debug(westRay) << "\n\n"; */
        /* std::cout << "result : \n"; */
        /* std::cout << BitBoard::Debug(westRay & (rankMask << ((i/8)*8))) << "\n\n"; */
    }

    for(int i=0; i<64; i++) {
        if (i%8 == 0)
            std::cout << std::endl;
        //std::cout << BitBoard::Debug(westAttacks[i]) << "\n";
        std::cout << "0x" << std::setfill('0') << std::setw(16) << westAttacks[i] << ", ";
    }
    std::cout << std::dec;
    std::cout << std::endl;
}


void GenerateSouthAttacks() {
    U64 southAttacks[64];
    std::cout << "South ray attacks : \n";
    std::cout << std::hex;
    U64 southRay = 0x0001010101010101 << 7;
    for(int i=63; i>=0; i--, southRay >>=1 ) {
        southAttacks[i] = southRay;
    }

    for(int i=0; i<64; i++) {
        if (i%8 == 0)
            std::cout << std::endl;
        //southAttacks[i] = southRay;
        //std::cout << BitBoard::Debug(southAttacks[i]) << std::endl;
        std::cout << "0x" << std::setfill('0') << std::setw(16) << southAttacks[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << std::dec;
}

void GenerateConstants() {

    U64 notAFile = 0;
    for (int i=0; i<64; i++) {
        if (i%8 > 0) 
            notAFile |= (1UL << i);
    }
    /* std::cout << "notAFile : " << std::hex << notAFile << std::endl; */

    U64 notBFile = 0;
    for (int i=0; i<64; i++) {
        if ((i+7)%8 > 0) 
            notBFile |= (1UL << i);
    }
    /* std::cout << "notBFile : " << std::hex << notBFile << std::endl; */

    U64 notGFile = 0;
    for (int i=0; i<64; i++) {
        if ((i+2)%8 > 0) 
            notGFile |= (1UL << i);
    }
    /* std::cout << "notGFile : " << std::hex << notGFile << std::endl; */

    U64 notHFile = 0;
    for (int i=0; i<64; i++) {
        if ((i+1)%8 > 0) 
            notHFile |= (1UL << i);
    }
    /* std::cout << "notHFile : " << std::hex << notHFile << std::endl; */
    /* std::cout << std::dec; */

    /* 
        GenerateKingAttacks(); // Done 
        GenerateKnightAttacks(); // Done 
        GenerateNorthAttacks(); // Done
        GenerateSouthAttacks(); // Done
        GenerateEastAttacks(); // Done
        GenerateWestAttacks(); // Done
        GenerateNorthEastAttacks(); // Done
        GenerateSouthEastAttacks(); // Done
        GenerateNorthWestAttacks(); // Done
        GenerateSouthWestAttacks(); // Done

    	GenerateFirstRankAttacks();
    */
}

void testRayAttacks(int sq) {
    U64 test = NORTH_ATTACKS[sq] | SOUTH_ATTACKS[sq] | EAST_ATTACKS[sq] | WEST_ATTACKS[sq] | 
               NORTH_EAST_ATTACKS[sq] | NORTH_WEST_ATTACKS[sq] |
               SOUTH_EAST_ATTACKS[sq] | SOUTH_WEST_ATTACKS[sq];
    
    std::cout << BitBoard::Debug(test) << std::endl;
}


void GenerateRankMaskEx() {
    std::cout << "Rank Masks Ex" << "\n";
    std::cout << std::hex;
    for(int sq=0; sq<64; sq++) {
        if ((sq%8) == 0)
            std::cout << "\n";
        U64 res = EAST_ATTACKS[sq] | WEST_ATTACKS[sq];
        std::cout << "0x" << std::setfill('0') << std::setw(16) << res << ", ";

    }
    std::cout << std::dec << "\n\n";
}

void GenerateDiagMaskEx() {
    std::cout << "Diag Masks Ex" << "\n";
    std::cout << std::hex;
    for(int sq=0; sq<64; sq++) {
        if ((sq%8) == 0)
            std::cout << "\n";
        U64 res = NORTH_EAST_ATTACKS[sq] | SOUTH_WEST_ATTACKS[sq];
        std::cout << "0x" << std::setfill('0') << std::setw(16) << res << ", ";

    }
    std::cout << std::dec << "\n\n";
}

void GenerateAntiDiagMaskEx() {
    std::cout << "Anti Diag Masks Ex" << "\n";
    std::cout << std::hex;
    for(int sq=0; sq<64; sq++) {
        if ((sq%8) == 0)
            std::cout << "\n";
        U64 res = SOUTH_EAST_ATTACKS[sq] | NORTH_WEST_ATTACKS[sq];
        std::cout << "0x" << std::setfill('0') << std::setw(16) << res << ", ";

    }
    std::cout << std::dec << "\n\n";
}

void GenerateFileMasks() {
    std::cout << "File Masks" << "\n";
    std::cout << std::hex;
    for(int sq=0; sq<64; sq++) {
        if ((sq%8) == 0)
            std::cout << "\n";
        int file = sq&7;
        U64 res = A_FILE << file;
        std::cout << "0x" << std::setfill('0') << std::setw(16) << res << ", ";
    }
    std::cout << std::dec << "\n\n";
}


void GenerateRankMask() {
    std::cout << "Rank Masks" << "\n";
    std::cout << std::hex;
    for(int sq=0; sq<64; sq++) {
        if ((sq%8) == 0)
            std::cout << "\n";
        U64 res = EAST_ATTACKS[sq] | WEST_ATTACKS[sq] | (1UL << sq);
        std::cout << "0x" << std::setfill('0') << std::setw(16) << res << ", ";

    }
    std::cout << std::dec << "\n\n";
}

void GenerateDiagMask() {
    std::cout << "Diag Masks" << "\n";
    std::cout << std::hex;
    for(int sq=0; sq<64; sq++) {
        if ((sq%8) == 0)
            std::cout << "\n";
        U64 res = NORTH_EAST_ATTACKS[sq] | SOUTH_WEST_ATTACKS[sq] | (1UL << sq);
        std::cout << "0x" << std::setfill('0') << std::setw(16) << res << ", ";

    }
    std::cout << std::dec << "\n\n";
}

void GenerateAntiDiagMask() {
    std::cout << "Anti Diag Masks" << "\n";
    std::cout << std::hex;
    for(int sq=0; sq<64; sq++) {
        if ((sq%8) == 0)
            std::cout << "\n";
        U64 res = SOUTH_EAST_ATTACKS[sq] | NORTH_WEST_ATTACKS[sq] | (1UL << sq);
        std::cout << "0x" << std::setfill('0') << std::setw(16) << res << ", ";

    }
    std::cout << std::dec << "\n\n";
}
