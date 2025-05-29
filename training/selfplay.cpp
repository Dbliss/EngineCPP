#include "chess.h"
#include "engine.h"
#include "engine2.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " FEN [tablefile]" << std::endl;
        return 1;
    }

    std::string fen = argv[1];
    if (argc >= 3) {
        loadPieceSquareTables(argv[2]);
    }

    initializeZobristTable();
    Board board;
    board.createBoardFromFEN(fen);
    board.configureTranspositionTableSize(32);

    const int timeLimit = 50;  // milliseconds per move
    const int moveLimit = 100;
    bool engine1Turn = true;
    int moves = 0;

    while (true) {
        if (board.generateAllMoves().empty()) {
            if (board.amIInCheck(board.whiteToMove)) {
                std::cout << (engine1Turn ? "engine2" : "engine1") << std::endl;
            } else {
                std::cout << "draw" << std::endl;
            }
            break;
        }
        if (board.isThreefoldRepetition(false) || moves >= moveLimit) {
            std::cout << "draw" << std::endl;
            break;
        }

        Move m = engine1Turn ? getEngineMove1(board, timeLimit)
                              : getEngineMove2(board, timeLimit);
        board.makeMove(m);
        board.lastMove = m;
        board.updatePositionHistory(true);
        engine1Turn = !engine1Turn;
        moves++;
    }

    return 0;
}

