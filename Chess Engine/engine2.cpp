#include "engine.h"
#include <iostream>
#include <chrono>
#include "engine2.h"
#include <vector>
#include <algorithm>
#include <tuple>

std::chrono::time_point<std::chrono::high_resolution_clock> endTime2;

const int64_t pawn_pcsq[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     15,  20,  30,  40,  40,  30,  20,  15,
     10,  10,  20,  30,  30,  20,  10,  10,
      5,   5,  10,  25,  25,  10,   5,   5,
      0,   0,   0,  20,  20,   0,   0,   0,
      5,  -5, -10, -30, -30, -10,  -5,   5,
      5,  10,  10, -20, -20,  10,  10,   5,
      0,   0,   0,   0,   0,   0,   0,   0
};

const int64_t knight_pcsq[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20,   0,   0,   0,   0, -20, -40,
    -30,   0,  10,  15,  15,  10,   0, -30,
    -30,   5,  15,  20,  20,  15,   5, -30,
    -30,   0,  15,  20,  20,  15,   0, -30,
    -30,   5,  10,  15,  15,  10,   5, -30,
    -40, -20,   0,   5,   5,   0, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50
};

const int64_t bishop_pcsq[64] = {
    -10, -10, -10, -10, -10, -10, -10, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,   5,   5,   5,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   5,   5,   5,   5,   0, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10, -10, -20, -10, -10, -20, -10, -10
};

const int64_t king_pcsq[64] = {
    -40, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -20, -20, -20, -20, -20, -20, -20, -20,
      0,  20,  40, -20,   0, -20,  40,  20
};

const int64_t king_pcsq_black[64] = {
    -40, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -40, -40, -40, -40, -40, -40, -40, -40,
    -20, -20, -20, -20, -20, -20, -20, -20,
     20,  40, -20,   0, -20,  40,  20,   0 
};

const int64_t king_endgame_pcsq[64] = {
    -40, -30, -20, -10, -10, -20, -30, -40,
    -30, -10,   0,  10,  10,   0, -10, -30,
    -20,   0,  30,  50,  50,  30,   0, -20,
    -10,  10,  50,  60,  60,  50,  10, -10,
    -10,  10,  50,  60,  60,  50,  10, -10,
    -20,   0,  30,  50,  50,  30,   0, -20,
    -30, -10,   0,  10,  10,   0, -10, -30,
    -40, -30, -20, -10, -10, -20, -30, -40
};

unsigned int ctzll3(unsigned long long x) {
    unsigned long index; // Variable to store the result
    // _BitScanForward64 returns 0 if x is zero, so handle this case:
    if (_BitScanForward64(&index, x))
        return index;
    else
        return 64; // Define behavior for x == 0
}

int kingDistance2(uint64_t king1, uint64_t king2) {
    int index1 = ctzll3(king1);
    int index2 = ctzll3(king2);

    int x1 = index1 % 8;
    int y1 = index1 / 8;
    int x2 = index2 % 8;
    int y2 = index2 / 8;

    return std::max(std::abs(x1 - x2), std::abs(y1 - y2));
}

// Incentives for pieces moving towards the enemy king
auto addIncentiveForPiece = [&](uint64_t pieces, uint64_t enemyKing, const int incentiveArray[]) {
    double_t incentive = 0;
    while (pieces) {
        uint64_t piece = pieces & (~pieces + 1);
        int distance = kingDistance2(piece, enemyKing);
        incentive += incentiveArray[distance];
        pieces &= ~piece;  // Clear the least significant bit set
    }
    return incentive;
};

double_t evaluate2(Board& board);

double_t evaluate2(Board& board) {
    // Check for draw condition based on insufficient material
    if ((std::_Popcount(board.whitePieces) == 1) && (std::_Popcount(board.blackPieces) == 1)) {
        return 0; // Return 0 to indicate a draw
    }
    double_t result = 0;

    const double_t pawnValue = 100;
    const double_t knightValue = 325;
    const double_t bishopValue = 325;
    const double_t rookValue = 500;
    const double_t queenValue = 975;

    int numWhitePawns = std::_Popcount(board.whitePawns);
    int numWhiteBishops = std::_Popcount(board.whiteBishops);
    int numWhiteKnights = std::_Popcount(board.whiteKnights);
    int numWhiteRooks = std::_Popcount(board.whiteRooks);
    int numWhiteQueens = std::_Popcount(board.whiteQueens);

    int numBlackPawns = std::_Popcount(board.blackPawns);
    int numBlackBishops = std::_Popcount(board.blackBishops);
    int numBlackKnights = std::_Popcount(board.blackKnights);
    int numBlackRooks = std::_Popcount(board.blackRooks);
    int numBlackQueens = std::_Popcount(board.blackQueens);

    // Encourage draws if both sides have no pawns or major pieces left and only up to one minor piece each.
    if (!numWhitePawns && !numBlackPawns &&
        !numWhiteQueens && !numBlackQueens &&
        !numWhiteRooks && !numBlackRooks &&
        (numWhiteBishops + numWhiteKnights <= 1) &&
        (numBlackBishops + numBlackKnights <= 1)) {

        // If both sides have at most one minor piece each, this is a draw
        if (numWhiteBishops + numWhiteKnights + numBlackBishops + numBlackKnights <= 2) {
            return board.whiteToMove ? -5 : 5;
        }
    }

    // Define file masks
    Bitboard fileMasks[8] = {
        0x0101010101010101ULL, 0x0202020202020202ULL, 0x0404040404040404ULL, 0x0808080808080808ULL,
        0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL
    };

    // Define rank masks for pawn progress bonuses
    int pawnProgressBonus[8] = { 0, 10, 20, 30, 50, 70, 90, 0 }; // No bonus on rank 1 and rank 8

    // Define passed pawn bonus
    int passedPawnBonus[8] = { 0, 10, 20, 30, 50, 70, 90, 0 }; // No bonus on rank 1 and rank 8

    // Rank masks
    Bitboard rankMasks[8] = {
        0xFFULL, 0xFF00ULL, 0xFF0000ULL, 0xFF000000ULL, 0xFF00000000ULL, 0xFF0000000000ULL, 0xFF000000000000ULL, 0xFF00000000000000ULL
    };


    // Calculate the game phase (0-1)
    const double_t totalMaterial = 16 * pawnValue + 4 * knightValue + 4 * bishopValue + 4 * rookValue + 2 * queenValue;
    double_t whiteMaterial = numWhitePawns * pawnValue +
        numWhiteKnights * knightValue +
        numWhiteBishops * bishopValue +
        numWhiteRooks * rookValue +
        numWhiteQueens * queenValue;

    double_t blackMaterial = numBlackPawns * pawnValue +
        numBlackKnights * knightValue +
        numBlackBishops * bishopValue +
        numBlackRooks * rookValue +
        numBlackQueens * queenValue;

    double_t currentMaterial = whiteMaterial + blackMaterial;
    double_t gamePhase = ((totalMaterial - currentMaterial)) / totalMaterial;

    // Helper function to get the positional value of a bitboard
    auto getPositionalValueWhite = [](int64_t pieces, const int64_t values[]) {
        double_t positionalValue = 0;
        while (pieces) {
            int index = ctzll3(pieces);
            positionalValue += values[63 - index];
            pieces &= pieces - 1;  // Clear the least significant bit set
        }
        return positionalValue;
        };

    // Helper function to get the positional value of a bitboard
    auto getPositionalValueBlack = [](int64_t pieces, const int64_t values[]) {
        double_t positionalValue = 0;
        while (pieces) {
            int index = ctzll3(pieces);
            positionalValue += values[index];
            pieces &= pieces - 1;  // Clear the least significant bit set
        }
        return positionalValue;
        };

    int numPawns = numWhitePawns + numBlackPawns;
    double multiplierBishop = 5*(16 - numPawns);

    //Bishops worth more if there are less pawns
    result += numWhiteBishops * multiplierBishop;
    result -= numBlackBishops * multiplierBishop;

    // Bishop pair incentive
    if (numWhiteBishops == 2) {
        result += multiplierBishop;
    }

    if (numBlackBishops == 2) {
        result -= multiplierBishop;
    }


    int whitePawnDefenders = 0;
    int blackPawnDefenders = 0;
    // no pawns is bad
    if (gamePhase > 0.6) {
        if ((numWhitePawns < 1) && (numWhiteQueens == 0)) {
            result -= 140 * gamePhase;
        }
        if ((numBlackPawns < 1) && (numBlackQueens == 0)) {
            result += 140 * gamePhase;
        }
    }
    // early game king safety
    else {
        // Definitions for king movements
        const Bitboard notHFile = 0xFEFEFEFEFEFEFEFEULL; // Mask to avoid wraparound from a to h
        const Bitboard notAFile = 0x7F7F7F7F7F7F7F7FULL; // Mask to avoid wraparound from h to a

        // Define pawn defenders bonus
        int kingSafetyBonus[6] = { -150, -50, -20, 0, 5, 10 };
        int pawnStormBonus[8] = { 0, 0, 0, 5, 10, 12, 15, 0 };

        auto calculateKingSafety = [&](Bitboard king, Bitboard pawns, bool isWhite) {
            Bitboard kingNorth = isWhite ? king << 8 : king >> 8;
            Bitboard kingNorthWest = isWhite ? (king << 9) & notAFile : (king >> 9) & notHFile;
            Bitboard kingNorthEast = isWhite ? (king << 7) & notHFile : (king >> 7) & notAFile;
            Bitboard kingWest = (king << 1) & notAFile;
            Bitboard kingEast = (king >> 1) & notHFile;

            // Check if these positions are occupied by pawns
            Bitboard shieldPawns = (kingNorth | kingNorthEast | kingNorthWest | kingEast | kingWest) & pawns;

            // Count the number of pawns directly shielding the king
            int pawnDefenders = std::_Popcount(shieldPawns);
            return pawnDefenders;
        };

        // Calculate white king safety
        whitePawnDefenders = calculateKingSafety(board.whiteKing, board.whitePawns, true);
        result += kingSafetyBonus[whitePawnDefenders];

        // Calculate black king safety
        blackPawnDefenders = calculateKingSafety(board.blackKing, board.blackPawns, false);
        result -= kingSafetyBonus[blackPawnDefenders];

        // Get the file of the enemy king
        int whiteKingFile = ctzll3(board.whiteKing) % 8;
        int blackKingFile = ctzll3(board.blackKing) % 8;

        // Create a mask for the king's file and the adjacent files
        Bitboard whiteKingFileMask = 0;
        Bitboard blackKingFileMask = 0;
        whiteKingFileMask |= fileMasks[whiteKingFile];
        blackKingFileMask |= fileMasks[blackKingFile];
        if (whiteKingFile > 0) {
            whiteKingFileMask |= fileMasks[whiteKingFile - 1];
        }
        if (whiteKingFile < 7) {
            whiteKingFileMask |= fileMasks[whiteKingFile + 1];
        }
        if (blackKingFile > 0) {
            blackKingFileMask |= fileMasks[blackKingFile - 1];
        }
        if (blackKingFile < 7) {
            blackKingFileMask |= fileMasks[blackKingFile + 1];
        }

        for (int rank = 1; rank <= 6; ++rank) {
            Bitboard whiteRankPawns = board.whitePawns & rankMasks[rank] & whiteKingFileMask;
            Bitboard blackRankPawns = board.blackPawns & rankMasks[7 - rank] & blackKingFileMask;

            // Reward pawn progress
            result += pawnStormBonus[rank] * std::_Popcount(whiteRankPawns);
            result -= pawnStormBonus[rank] * std::_Popcount(blackRankPawns);
        }

        // estimate how safe king is by how many queen moves
        int whiteKingProxyMoves = size(board.generateQueenMoves(board.whiteKing, board.whitePieces, board.blackPieces));
        if (whiteKingProxyMoves <= 1) {
            result -= (2 - whiteKingProxyMoves) * 16;
        }
        else if (whiteKingProxyMoves > 3) {
            result -= whiteKingProxyMoves * 6;
        }
        int blackKingProxyMoves = size(board.generateQueenMoves(board.blackKing, board.blackPieces, board.whitePieces));
        if (blackKingProxyMoves <= 1) {
            result += (2 - blackKingProxyMoves) * 16;
        }
        else if (blackKingProxyMoves > 3) {
            result += blackKingProxyMoves * 6;
        }
    }

    // Calculate white pieces' value and positional value
    result += whiteMaterial;
    result += getPositionalValueWhite(board.whitePawns, pawn_pcsq);
    result += getPositionalValueWhite(board.whiteKnights, knight_pcsq);
    result += getPositionalValueWhite(board.whiteBishops, bishop_pcsq);
    result += (gamePhase * getPositionalValueWhite(board.whiteKing, king_endgame_pcsq)) +
        ((1 - gamePhase) * getPositionalValueWhite(board.whiteKing, king_pcsq));

    // Calculate black pieces' value and positional value
    result -= blackMaterial;
    result -= getPositionalValueBlack(board.blackPawns, pawn_pcsq);
    result -= getPositionalValueBlack(board.blackKnights, knight_pcsq);
    result -= getPositionalValueBlack(board.blackBishops, bishop_pcsq);
    result -= (gamePhase * getPositionalValueBlack(board.blackKing, king_endgame_pcsq)) +
        ((1 - gamePhase) * getPositionalValueBlack(board.blackKing, king_pcsq_black));

    // Penalize double pawns
    for (int file = 0; file < 8; ++file) {
        int whitePawnCount = std::_Popcount(board.whitePawns & fileMasks[file]);
        int blackPawnCount = std::_Popcount(board.blackPawns & fileMasks[file]);
        if (whitePawnCount > 1) result -= 20 * (whitePawnCount - 1);
        if (blackPawnCount > 1) result += 20 * (blackPawnCount - 1);
    }

    if (gamePhase > 0.3) {
        double_t lateGamePawnPos = 0;
        for (int rank = 1; rank <= 6; ++rank) { // Skipping rank 0 and 7 (no pawns can be there)
            Bitboard whiteRankPawns = board.whitePawns & rankMasks[rank];
            Bitboard blackRankPawns = board.blackPawns & rankMasks[7 - rank];

            // Reward pawn progress
            lateGamePawnPos += pawnProgressBonus[rank] * std::_Popcount(whiteRankPawns);
            lateGamePawnPos -= pawnProgressBonus[rank] * std::_Popcount(blackRankPawns);

            // Check for passed pawns
            for (int file = 0; file < 8; ++file) {
                Bitboard whitePawn = whiteRankPawns & fileMasks[file];
                if (whitePawn) {
                    Bitboard blockingPawns = board.blackPawns & (
                        (fileMasks[file] | (file > 0 ? fileMasks[file - 1] : 0) | (file < 7 ? fileMasks[file + 1] : 0)) &
                        (rankMasks[rank + 1] | rankMasks[rank + 2] | rankMasks[rank + 3] | rankMasks[rank + 4] | rankMasks[rank + 5] | rankMasks[rank + 6])
                        );
                    if (!blockingPawns) {
                        lateGamePawnPos += passedPawnBonus[rank];
                    }
                }

                Bitboard blackPawn = blackRankPawns & fileMasks[file];
                if (blackPawn) {
                    Bitboard blockingPawns = board.whitePawns & (
                        (fileMasks[file] | (file > 0 ? fileMasks[file - 1] : 0) | (file < 7 ? fileMasks[file + 1] : 0)) &
                        (rankMasks[6 - rank] | rankMasks[5 - rank] | rankMasks[4 - rank] | rankMasks[3 - rank] | rankMasks[2 - rank] | rankMasks[1 - rank])
                        );
                    if (!blockingPawns) {
                        lateGamePawnPos -= passedPawnBonus[rank];
                    }
                }
            }
        }
        result += lateGamePawnPos * (gamePhase) * 1.5;
    }

    // Reward pawns defending pawns
    Bitboard leftDefendedPawns = (board.whitePawns & ~fileMasks[7]) << 9;
    Bitboard rightDefendedPawns = (board.whitePawns & ~fileMasks[0]) << 7;
    Bitboard defendedPawns = leftDefendedPawns | rightDefendedPawns;
    result += 15 * std::_Popcount(defendedPawns & board.whitePawns);

    leftDefendedPawns = (board.blackPawns & ~fileMasks[7]) >> 7;
    rightDefendedPawns = (board.blackPawns & ~fileMasks[0]) >> 9;
    defendedPawns = leftDefendedPawns | rightDefendedPawns;
    result -= 15 * std::_Popcount(defendedPawns & board.blackPawns);


    // Add incentives for piece mobility
    result += 4 * board.generateBishopMoves(board.whiteBishops, board.whitePieces, board.blackPieces).size();
    result -= 4 * board.generateBishopMoves(board.blackBishops, board.blackPieces, board.whitePieces).size();

    result += 6 * board.generateRookMoves(board.whiteRooks, board.whitePieces, board.blackPieces).size();
    result -= 6 * board.generateRookMoves(board.blackRooks, board.blackPieces, board.whitePieces).size();

    result += 4 * board.generateQueenMoves(board.whiteQueens, board.whitePieces, board.blackPieces).size();
    result -= 4 * board.generateQueenMoves(board.blackQueens, board.blackPieces, board.whitePieces).size();

   
    
    // Define incentive arrays for each piece type (example values)
    int pawnIncentive[9] = { 0, 14, 12, 10, 8, 6, 4, 2, 0 };
    int knightIncentive[9] = { 0, 5, 25, 15, 5, 0, 0, 0, 0 };
    int bishopIncentive[9] = { 0, 21, 18, 15, 12, 9, 6, 3, 0 };
    int rookIncentive[9] = { 0, 28, 24, 20, 16, 12, 8, 4, 0 };
    int queenIncentive[9] = { 0, 35, 30, 25, 20, 15, 10, 5, 0 };
    int defendersMultiplier[6] = { 2, 1.5, 1, 1, 0.5, 0 };
    
    if (gamePhase <= 0.6) {
        //consider how many defenders early game
        //result += addIncentiveForPiece(board.whitePawns, board.blackKing, pawnIncentive);
        //result += addIncentiveForPiece(board.whiteKnights, board.blackKing, knightIncentive) * defendersMultiplier[blackPawnDefenders];
        //result += addIncentiveForPiece(board.whiteBishops, board.blackKing, bishopIncentive) * defendersMultiplier[blackPawnDefenders];
        //result += addIncentiveForPiece(board.whiteRooks, board.blackKing, rookIncentive) * defendersMultiplier[blackPawnDefenders];
        //result += addIncentiveForPiece(board.whiteQueens, board.blackKing, queenIncentive) * defendersMultiplier[blackPawnDefenders];

        //result -= addIncentiveForPiece(board.blackPawns, board.whiteKing, pawnIncentive);
        //result -= addIncentiveForPiece(board.blackKnights, board.whiteKing, knightIncentive) * defendersMultiplier[whitePawnDefenders];
        //result -= addIncentiveForPiece(board.blackBishops, board.whiteKing, bishopIncentive) * defendersMultiplier[whitePawnDefenders];
        //result -= addIncentiveForPiece(board.blackRooks, board.whiteKing, rookIncentive) * defendersMultiplier[whitePawnDefenders];
        //result -= addIncentiveForPiece(board.blackQueens, board.whiteKing, queenIncentive) * defendersMultiplier[whitePawnDefenders];
    }
    
    else {
        //result += addIncentiveForPiece(board.whitePawns, board.blackKing, pawnIncentive);
        //result += addIncentiveForPiece(board.whiteKnights, board.blackKing, knightIncentive);
        //result += addIncentiveForPiece(board.whiteBishops, board.blackKing, bishopIncentive);
        //result += addIncentiveForPiece(board.whiteRooks, board.blackKing, rookIncentive);
        //result += addIncentiveForPiece(board.whiteQueens, board.blackKing, queenIncentive);

        //result -= addIncentiveForPiece(board.blackPawns, board.whiteKing, pawnIncentive);
        //result -= addIncentiveForPiece(board.blackKnights, board.whiteKing, knightIncentive);
        //result -= addIncentiveForPiece(board.blackBishops, board.whiteKing, bishopIncentive);
        //result -= addIncentiveForPiece(board.blackRooks, board.whiteKing, rookIncentive);
        //result -= addIncentiveForPiece(board.blackQueens, board.whiteKing, queenIncentive);
    }
    

    // Lead is expanded as game goes on, incentives trading
    if ((gamePhase > 0.6) && (std::abs(result) > 400)) {
        result = result * (1 + gamePhase / 2.5);
        
        int distBetweenKingsBonus[9] = { 0, 0, 140, 80, 40, 20, 0, -10, -20 };
        int distBetweenKings = kingDistance2(board.blackKing, board.whiteKing); // smaller is better
        if (result > 0) {
            result += distBetweenKingsBonus[distBetweenKings];
        }
        else {
            result -= distBetweenKingsBonus[distBetweenKings];
        }
    }
    return board.whiteToMove ? result : -result;
}

std::vector<Move> generateCaptures2(Board& board, std::vector<Move> allMoves);
std::vector<Move> generateCaptures2(Board& board, std::vector<Move> allMoves) {
    std::vector<Move> captures;

    for (Move& move : allMoves) {
        if (move.isCapture) {
            captures.push_back(move);
        }
    }

    // Optional: sort captures based on some heuristic, e.g., MVV-LVA (Most Valuable Victim - Least Valuable Attacker)
    std::sort(captures.begin(), captures.end(), [](const Move& a, const Move& b) {
        return a.capturedPiece > b.capturedPiece;
        });

    return captures;
}

double_t quiescenceSearch2(Board& board, double_t alpha, double_t beta);
double_t quiescenceSearch2(Board& board, double_t alpha, double_t beta) {
    uint64_t hash = board.generateZobristHash();
    TT_Entry* ttEntry = board.probeTranspositionTable(hash);

    if (ttEntry->key == hash && ttEntry->depth == 0) {
        if (ttEntry->flag == TTFlag::HASH_FLAG_EXACT) return ttEntry->score;
        if (ttEntry->flag == TTFlag::HASH_FLAG_LOWER && ttEntry->score >= beta) return ttEntry->score;
        if (ttEntry->flag == TTFlag::HASH_FLAG_UPPER && ttEntry->score <= alpha) return ttEntry->score;
    }

    double_t stand_pat = evaluate2(board);
    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }
    double_t checkmate = -20000;
    double_t draw = 0;
    std::vector<Move> allMoves = board.generateAllMoves();
    if (allMoves.empty()) {
        return { board.amIInCheck(board.whiteToMove) ? checkmate : draw };
    }
    std::vector<Move> moves = generateCaptures2(board, allMoves);


    moves = orderMoves(board, moves, ttEntry, 0);

    double_t subBestScore;
    Bitboard store = board.enPassantTarget;
    bool whiteKingMovedStore = board.whiteKingMoved;
    bool whiteLRookMovedStore = board.whiteLRookMoved;
    bool whiteRRookMovedStore = board.whiteRRookMoved;
    bool blackKingMovedStore = board.blackKingMoved;
    bool blackLRookMovedStore = board.blackLRookMoved;
    bool blackRRookMovedStore = board.blackRRookMoved;

    for (Move& move : moves) {
        board.makeMove(move);
        if (!board.isThreefoldRepetition()) {
            subBestScore = -quiescenceSearch2(board, -beta, -alpha);
        }
        else {
            subBestScore = 0;
        }

        board.enPassantTarget = store;
        board.whiteKingMoved = whiteKingMovedStore;
        board.whiteLRookMoved = whiteLRookMovedStore;
        board.whiteRRookMoved = whiteRRookMovedStore;
        board.blackKingMoved = blackKingMovedStore;
        board.blackLRookMoved = blackLRookMovedStore;
        board.blackRRookMoved = blackRRookMovedStore;
        board.undoMove(move);

        if (subBestScore >= beta) {
            return beta;
        }
        if (subBestScore > alpha) {
            alpha = subBestScore;
        }
        if (alpha >= beta) {
            break;
        }
    }
    return alpha;
}

std::tuple<Move, double_t> perft2(Board& board, int depth, std::vector<std::tuple<Move, double_t>>& iterativeDeepeningMoves, double_t alpha, double_t beta){
    double bestScore;
    Move bestMove;
    std::tie(bestMove, bestScore) = perftHelper2(board, depth, alpha, beta, depth, iterativeDeepeningMoves, 0, false);
    return { bestMove, bestScore };
}

std::tuple<Move, double_t> perftHelper2(Board& board, int depth, double_t alpha, double_t beta, int startDepth, std::vector<std::tuple<Move, double_t>>& iterativeDeepeningMoves, int totalExtensions, bool lastIterationNull) {
    if (std::chrono::high_resolution_clock::now() > endTime2) {
        return { Move(), -1.2345 };
    }
    const int MAX_EXTENSIONS = 3;
    const int MAX_EXTENSION_DEPTH = 3;
    const double_t INITIAL_WINDOW = 50.0; // Initial aspiration window value
    int extension = 0;
    
    uint64_t hash = board.generateZobristHash();
    TT_Entry* ttEntry = board.probeTranspositionTable(hash);

    if (ttEntry->key == hash) {
        if (depth == startDepth && ttEntry->flag == TTFlag::HASH_BOOK) {
            return { ttEntry->move, 0 };
        }
        else if (ttEntry->depth >= depth) {
            if (ttEntry->flag == TTFlag::HASH_FLAG_EXACT) return { ttEntry->move, ttEntry->score };
            if (ttEntry->flag == TTFlag::HASH_FLAG_LOWER && ttEntry->score >= beta) return { ttEntry->move, ttEntry->score };
            if (ttEntry->flag == TTFlag::HASH_FLAG_UPPER && ttEntry->score <= alpha) return { ttEntry->move, ttEntry->score };
        }
    }

    std::vector<Move> moves;
    if (depth == startDepth && !iterativeDeepeningMoves.empty()) {
        std::transform(iterativeDeepeningMoves.begin(), iterativeDeepeningMoves.end(), std::back_inserter(moves),
            [](const std::tuple<Move, double_t>& pair) { return std::get<0>(pair); });
    } else {
        moves = board.generateAllMoves();
        if (moves.empty()) {
            if (board.amIInCheck(board.whiteToMove)) {
                return { Move(), -20000 }; // checkmate
            } else {
                return { Move(), 0 }; // stalemate
            }
        } else if (depth == 0) {
            double_t val = quiescenceSearch2(board, alpha, beta);
            return { Move(), val };
        }
        moves = orderMoves(board, moves, ttEntry, depth);
    }
  
    // Null Move Pruning
    if (!board.amIInCheck(board.whiteToMove) && depth > 2 && isNullViable(board) && !lastIterationNull && depth != startDepth) {
        board.makeNullMove();
        int R = 2;
        std::tuple<Move, double_t> result = perftHelper2(board, depth - 1 - R, -beta, -beta + 1, startDepth, iterativeDeepeningMoves, totalExtensions, true);
        double_t nullMoveEvaluation = -std::get<1>(result);
        board.undoNullMove();

        if (nullMoveEvaluation >= beta) {
            return { Move(), beta }; // Cutoff
        }

        // mate threat or something?
        if (totalExtensions < MAX_EXTENSIONS) {
            if (nullMoveEvaluation + 100 <= alpha) {
                extension = 1;
            }
        }
    }

    Move bestMove;
    double_t bestScore = -100000;
    Bitboard store = board.enPassantTarget;
    bool whiteKingMovedStore = board.whiteKingMoved;
    bool whiteLRookMovedStore = board.whiteLRookMoved;
    bool whiteRRookMovedStore = board.whiteRRookMoved;
    bool blackKingMovedStore = board.blackKingMoved;
    bool blackLRookMovedStore = board.blackLRookMoved;
    bool blackRRookMovedStore = board.blackRRookMoved;
    double_t subBestScore;
    Move subBestMove;
    std::vector<std::tuple<Move, double_t>> moveScores;

    for (int i = 0; i < moves.size(); i++) {
        Move& move = moves[i];
        board.makeMove(move);

        if (!board.isThreefoldRepetition()) {
            bool needsFullSearch = true;
            if (i >= 3 && extension == 0 && depth >= 4 && !move.isCapture) {
                int depthReduction;

                if (i >= (moves.size() * 4 / 5)) {
                    depthReduction = 2;
                }
                else {
                    depthReduction = 1;
                }
                std::tie(subBestMove, subBestScore) = perftHelper2(board, depth - 1 - depthReduction, -beta, -alpha, startDepth, iterativeDeepeningMoves, totalExtensions, false);
                subBestScore = -subBestScore;

                needsFullSearch = subBestScore > alpha;
            }

            if (needsFullSearch) {
                std::tie(subBestMove, subBestScore) = perftHelper2(board, depth - 1 + extension, -beta, -alpha, startDepth, iterativeDeepeningMoves, totalExtensions + extension, false);
                subBestScore = -subBestScore;
            }
        }
        else {
            subBestScore = 0;
        }

        board.enPassantTarget = store;
        board.whiteKingMoved = whiteKingMovedStore;
        board.whiteLRookMoved = whiteLRookMovedStore;
        board.whiteRRookMoved = whiteRRookMovedStore;
        board.blackKingMoved = blackKingMovedStore;
        board.blackLRookMoved = blackLRookMovedStore;
        board.blackRRookMoved = blackRRookMovedStore;
        board.undoMove(move);

        if (subBestScore == 1.2345) {
            return { Move(), -1.2345 };
        }

        if (depth == startDepth) {
            moveScores.emplace_back(move, subBestScore);
        }

        if (subBestScore >= beta) {
            // Record the killer move if it isnt a capture
            if (board.killerMoves[0][depth] != move && !move.isCapture && board.killerMoves[1][depth] != move) {
                board.killerMoves[1][depth] = board.killerMoves[0][depth];
                board.killerMoves[0][depth] = move;
            }
            board.record_tt_entry(hash, beta, HASH_FLAG_LOWER, move, depth);
            return { move, beta };
        }

        if (subBestScore > bestScore) {
            bestScore = subBestScore;
            bestMove = move;
        }

        if (bestScore > alpha) {
            alpha = bestScore;
        }

        if (alpha >= beta) {
            break;
        }
    }

    if (depth == startDepth) {
        std::sort(moveScores.begin(), moveScores.end(), [](const std::tuple<Move, double_t>& a, const std::tuple<Move, double_t>& b) {
            return std::get<1>(a) > std::get<1>(b); // Sort descending by score
            });
        iterativeDeepeningMoves = moveScores;
    }

    TTFlag flag = (bestScore <= alpha) ? HASH_FLAG_UPPER : HASH_FLAG_EXACT;
    board.record_tt_entry(hash, bestScore, flag, bestMove, depth);

    return { bestMove, bestScore };
}