#include "engine.h"

std::tuple<int, int, int> perftHelper(Board& board, int depth);

std::tuple<int, int, int> perft(Board& board, int depth) {
    return perftHelper(board, depth);
}

// moves, checks, checkmates
std::tuple<int, int, int> perftHelper(Board& board, int depth) {
    if (depth == 0) {
        if (board.amIInCheck(board.whiteToMove)) {
            if (board.generateAllMoves().empty()) {
                return {1, 1, 1};
            } else {
                return {1, 1, 0};
            }
        } else {
            return {1, 0, 0};
        }
    }

    int totalMoves = 0;
    int checks = 0;
    int checkmates = 0;

    std::vector<Move> moves = board.generateAllMoves();
    for (Move& move : moves) {
        board.makeMove(move);
        if (depth > 1 && board.amIInCheck(board.whiteToMove)) {
            checks++;
        }
        auto [subMoves, subChecks, subCheckmates] = perftHelper(board, depth - 1);
        totalMoves += subMoves;
        checks += subChecks;
        checkmates += subCheckmates;
        board.undoMove(move);
    }

    return {totalMoves, checks, checkmates};
}
