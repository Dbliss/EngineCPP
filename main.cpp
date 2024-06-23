#include <iostream>
#include "chess.h"
#include "engine.h"
#include <chrono>

int main() {
    Board board;
    std::string fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    board.createBoardFromFEN(fen);
    //board.createBoard();
    std::cout << "Board from FEN:\n";
    board.whiteToMove ? std::cout << "White Turn\n": std::cout << "Black Turn\n";
    board.printBoard();
    board.whiteToMove = !board.whiteToMove;
    Move move(63, 62);
    board.makeMove(move);
    board.printBoard();
    std::vector moves = board.generateAllMoves();
    for (auto move : moves){
        std::cout << "From: " << move.from << "To: " << move.to << "Promo: " << move.promotion << "isCapture: " << move.isCapture << std::endl;
        board.makeMove(move);
        board.printBoard();
        board.undoMove(move);
    }
    int depth = 2; // Set desired depth

    auto start = std::chrono::high_resolution_clock::now();
    auto [totalMoves, checks, checkmates] = perft(board, depth);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    
    std::cout << "Total Moves: " << totalMoves << std::endl;
    std::cout << "Checks: " << checks << std::endl;
    std::cout << "Checkmates: " << checkmates << std::endl;
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;

    return 0;
}
