#include "engine.h"
#include <iostream>
#include <chrono>


const int64_t pawn_pcsq[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
      5,  10,  15,  20,  20,  15,  10,   5,
      4,   8,  12,  16,  16,  12,   8,   4,
      3,   6,   9,  12,  12,   9,   6,   3,
      2,   4,   6,   8,   8,   6,   4,   2,
      1,   2,   3, -10, -10,   3,   2,   1,
      0,   0,   0, -40, -40,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0
};

const int64_t knight_pcsq[64] = {
    -10, -10, -10, -10, -10, -10, -10, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10,   0,   5,   5,   5,   5,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   5,  10,  10,   5,   0, -10,
    -10,   0,   5,   5,   5,   5,   0, -10,
    -10,   0,   0,   0,   0,   0,   0, -10,
    -10, -30, -10, -10, -10, -10, -30, -10
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

const int64_t king_endgame_pcsq[64] = {
      0,  10,  20,  30,  30,  20,  10,   0,
     10,  20,  30,  40,  40,  30,  20,  10,
     20,  30,  40,  50,  50,  40,  30,  20,
     30,  40,  50,  60,  60,  50,  40,  30,
     30,  40,  50,  60,  60,  50,  40,  30,
     20,  30,  40,  50,  50,  40,  30,  20,
     10,  20,  30,  40,  40,  30,  20,  10,
      0,  10,  20,  30,  30,  20,  10,   0
};

unsigned int ctzll2(unsigned long long x) {
    unsigned long index; // Variable to store the result
    // _BitScanForward64 returns 0 if x is zero, so handle this case:
    if (_BitScanForward64(&index, x))
        return index;
    else
        return 64; // Define behavior for x == 0
}

double_t evaluate(Board& board) {
    double_t result = 0;

    const double_t pawnValue = 100;
    const double_t knightValue = 300;
    const double_t bishopValue = 350;
    const double_t rookValue = 550;
    const double_t queenValue = 950;
    const double_t kingValue = 10000;

    // Helper function to get the positional value of a bitboard
    auto getPositionalValueWhite = [](int64_t pieces, const int64_t values[]) {
        double_t positionalValue = 0;
        while (pieces) {
            int index = ctzll2(pieces);
            positionalValue += values[63 - index];
            pieces &= pieces - 1;  // Clear the least significant bit set
        }
        return positionalValue;
        };

    // Helper function to get the positional value of a bitboard
    auto getPositionalValueBlack = [](int64_t pieces, const int64_t values[]) {
        double_t positionalValue = 0;
        while (pieces) {
            int index = ctzll2(pieces);
            positionalValue += values[index];
            pieces &= pieces - 1;  // Clear the least significant bit set
        }
        return positionalValue;
        };

    // Calculate white pieces' value and positional value
    result += pawnValue * std::_Popcount(board.whitePawns) + getPositionalValueWhite(board.whitePawns, pawn_pcsq);
    result += knightValue * std::_Popcount(board.whiteKnights) + getPositionalValueWhite(board.whiteKnights, knight_pcsq);
    result += bishopValue * std::_Popcount(board.whiteBishops) + getPositionalValueWhite(board.whiteBishops, bishop_pcsq);
    result += rookValue * std::_Popcount(board.whiteRooks);
    result += queenValue * std::_Popcount(board.whiteQueens);
    result += kingValue * std::_Popcount(board.whiteKing) + getPositionalValueWhite(board.whiteKing, king_pcsq);

    // Calculate black pieces' value and positional value
    result -= pawnValue * std::_Popcount(board.blackPawns) + getPositionalValueBlack(board.blackPawns, pawn_pcsq);
    result -= knightValue * std::_Popcount(board.blackKnights) + getPositionalValueBlack(board.blackKnights, knight_pcsq);
    result -= bishopValue * std::_Popcount(board.blackBishops) + getPositionalValueBlack(board.blackBishops, bishop_pcsq);
    result -= rookValue * std::_Popcount(board.blackRooks);
    result -= queenValue * std::_Popcount(board.blackQueens);
    result -= kingValue * std::_Popcount(board.blackKing) + getPositionalValueBlack(board.blackKing, king_pcsq);

    return board.whiteToMove ? result : -result;
}

std::string numToBoardPosition2(int num) {
    // Ensure the number is within valid range
    if (num < 0 || num > 63) {
        return "Invalid num";
    }

    // Calculate the rank and file
    int rank = num / 8;
    int file = num % 8;

    // Convert rank and file to chess notation
    char rankChar = '1' + rank;
    char fileChar = 'h' - file;

    // Return the board position as a string
    return std::string(1, fileChar) + rankChar;
}

std::tuple<Move, double_t> perftHelper(Board& board, int depth, int startDepth);

std::tuple<Move, double_t> perft(Board& board, int depth, int startDepth) {
    double bestScore;
    Move bestMove;
    std::tie(bestMove, bestScore) = perftHelper(board, depth, startDepth);
    return { bestMove, bestScore };
}

std::tuple<Move, double_t> perftHelper(Board& board, int depth, int startDepth) {
    if (depth == 0) {
        if (board.amIInCheck(board.whiteToMove)) {
            if (board.generateAllMoves().empty()) {
                return {Move(), -10000}; // checkmate
            }
            else {
                return {Move(), evaluate(board) };
            }
        }
        else {
            return {Move(), evaluate(board) };
        }
    }

    int totalMoves = 0;
    int checks = 0;
    int checkmates = 0;
    Move bestMove;
    double_t bestScore = -100000;

    std::vector<Move> moves = board.generateAllMoves();
    Bitboard store = board.enPassantTarget;
    bool whiteKingMovedStore = board.whiteKingMoved;
    bool whiteLRookMovedStore = board.whiteLRookMoved;
    bool whiteRRookMovedStore = board.whiteRRookMoved;
    bool blackKingMovedStore = board.blackKingMoved;
    bool blackLRookMovedStore = board.blackLRookMoved;
    bool blackRRookMovedStore = board.blackRRookMoved;

    for (Move& move : moves) {
        board.makeMove(move);
        Move subBestMove;
        double_t subBestScore;
        std::tie(subBestMove, subBestScore) = perftHelper(board, depth - 1, startDepth);
        subBestScore = -subBestScore;
        if (subBestScore > bestScore) {
            bestScore = subBestScore;
            bestMove = move;
        }

        board.enPassantTarget = store;
        board.whiteKingMoved = whiteKingMovedStore;
        board.whiteLRookMoved = whiteLRookMovedStore;
        board.whiteRRookMoved = whiteRRookMovedStore;
        board.blackKingMoved = blackKingMovedStore;
        board.blackLRookMoved = blackLRookMovedStore;
        board.blackRRookMoved = blackRRookMovedStore;
        board.undoMove(move);
    }

    return { bestMove, bestScore };
}
