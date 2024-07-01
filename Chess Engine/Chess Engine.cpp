// Chess Engine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "chess.h"
#include "engine.h"
#include <chrono>
#include <string>

int main() {
    Board board;
    std::string fen1 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
    board.createBoardFromFEN(fen1);
    board.printBoard();
    
    int depth = 4; // Set desired depth
    int startDepth = depth;
    auto start = std::chrono::high_resolution_clock::now();
    double bestScore;
    Move bestMove;
    std::tie(bestMove, bestScore) = perft(board, depth, startDepth);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Best Move: " << numToBoardPosition(bestMove.from) << numToBoardPosition(bestMove.to) << std::endl;
    std::cout << "Best Score : " << bestScore << std::endl;
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
    
    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
