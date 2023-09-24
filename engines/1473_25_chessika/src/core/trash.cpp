


    // Beginning of AlphaBeta, just after time out test
    
    /* // TT code. Later.

    if (TT.find(key) != TT.end() && TT[key].m_depth == depth) {
        //std::cout << "Found key : " << std::hex << key << "\n";
        //std::cout << b.ToString();
        switch(TT[key].m_nodeType) {
            case LOWER_BOUND:
                alpha = std::max(alpha, TT[key].m_value);
                break;
            case UPPER_BOUND:
                alpha = std::min(beta, TT[key].m_value);
                break;
            case EXACT:
                return TT[key].m_value;

        }
        if (alpha >= beta)
            return TT[key].m_value;
    }
    //float previousAlpha = alpha;
    */




    // Just before returning value, at the end of Alphabeta


    /* // TT code. Later.
    NODE_TYPE nodeType = EXACT;

    if (best <= previousAlpha) 
        nodeType = UPPER_BOUND;

    if (best >= beta)
        nodeType = LOWER_BOUND;

    TT[key] = TTEntry(best, b.m_ply, nodeType);
    //std::cout << "Storing key : " << std::hex << key << ", type:" << nodeType << "\n";
    //std::cout << b.ToString();
    */
