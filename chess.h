#ifndef CHESS_H
#define CHESS_H

#include <vector>
#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>
#include <cctype>

typedef uint64_t Bitboard;

class Move {
public:
    int from;
    int to;
    char promotion;
    bool isEnPassant;
    bool isCapture;
    char capturedPiece;

    Move(int f, int t, char p = 0, bool e = false, bool c = false, char captured = 0)
        : from(f), to(t), promotion(p), isEnPassant(e), isCapture(c), capturedPiece(captured){}
};

class Board {
public:
    Bitboard whitePawns;
    Bitboard blackPawns;
    Bitboard whiteBishops;
    Bitboard blackBishops;
    Bitboard whiteRooks;
    Bitboard blackRooks;
    Bitboard whiteKnights;
    Bitboard blackKnights;
    Bitboard whiteQueens;
    Bitboard blackQueens;
    Bitboard whiteKing;
    Bitboard blackKing;
    Bitboard whitePieces;
    Bitboard blackPieces;
    bool whiteToMove;

    //bool whiteKingsideCastle;
    //bool whiteQueensideCastle;
    //bool blackKingsideCastle;
    //bool blackQueensideCastle;

    bool whiteKingMoved;
    bool whiteLRookMoved;
    bool whiteRRookMoved;

    bool blackKingMoved;
    bool blackLRookMoved;
    bool blackRRookMoved;

    Board();
    void createBoard();
    void createBoardFromFEN(const std::string& fen);
    void printBoard();
    std::vector<Move> generatePawnMoves(Bitboard pawns, Bitboard ownPieces, Bitboard opponentPieces);
    std::vector<Move> generateBishopMoves(Bitboard bishops, Bitboard ownPieces, Bitboard opponentPieces);
    std::vector<Move> generateRookMoves(Bitboard rooks, Bitboard ownPieces, Bitboard opponentPieces);
    std::vector<Move> generateKnightMoves(Bitboard knights, Bitboard ownPieces);
    std::vector<Move> generateKingMoves(Bitboard king, Bitboard ownPieces, Bitboard opponentPieces);
    std::vector<Move> generateQueenMoves(Bitboard queens, Bitboard ownPieces, Bitboard opponentPieces);
    std::vector<Move> generateAllMoves();
    bool amIInCheck(bool player);
    void makeMove(Move& move);
    void undoMove(const Move& move);
    bool isSquareAttacked(int square, bool byWhite);
};

// Helper functions
void setBit(Bitboard& bitboard, int square);
void parseFEN(const std::string& fen, Board& board);

#endif // CHESS_H
