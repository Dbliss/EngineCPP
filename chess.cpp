#include "chess.h"
#include <iostream>
#include <sstream>
#include <string>

// Constructor to initialize the board
Board::Board() {
    createBoard();
}

// Function to initialize the board with the starting positions
void Board::createBoard() {
    // Initialize the board with starting positions
    whitePawns = 0x000000000000FF00;
    blackPawns = 0x00FF000000000000;
    whiteRooks = 0x0000000000000081;
    blackRooks = 0x8100000000000000;
    whiteKnights = 0x0000000000000042;
    blackKnights = 0x4200000000000000;
    whiteBishops = 0x0000000000000024;
    blackBishops = 0x2400000000000000;
    whiteQueens = 0x0000000000000010;
    blackQueens = 0x1000000000000000;
    whiteKing = 0x0000000000000008;
    blackKing = 0x0800000000000000;
    whitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
    blackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;
    whiteToMove = true;

    whiteKingMoved = false;
    whiteLRookMoved = false;
    whiteRRookMoved = false;
    blackKingMoved = false;
    blackLRookMoved = false;
    blackRRookMoved = false;
}

void Board::createBoardFromFEN(const std::string& fen) {
    parseFEN(fen, *this);
}

void Board::printBoard() {
    auto getPieceChar = [this](int index) -> char {
        if ((whitePawns >> index) & 1) return 'p';
        if ((blackPawns >> index) & 1) return 'P';
        if ((whiteRooks >> index) & 1) return 'r';
        if ((blackRooks >> index) & 1) return 'R';
        if ((whiteKnights >> index) & 1) return 'n';
        if ((blackKnights >> index) & 1) return 'N';
        if ((whiteBishops >> index) & 1) return 'b';
        if ((blackBishops >> index) & 1) return 'B';
        if ((whiteQueens >> index) & 1) return 'q';
        if ((blackQueens >> index) & 1) return 'Q';
        if ((whiteKing >> index) & 1) return 'k';
        if ((blackKing >> index) & 1) return 'K';
        return '.';
    };

    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 7; file >= 0; --file) {
            int index = rank * 8 + file;
            std::cout << getPieceChar(index) << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

std::vector<Move> Board::generatePawnMoves(Bitboard pawns, Bitboard ownPieces, Bitboard opponentPieces) {
    std::vector<Move> moves;
    Bitboard emptySquares = ~(ownPieces | opponentPieces);

    // Single pawn moves
    Bitboard singlePush = whiteToMove ? (pawns << 8) & emptySquares : (pawns >> 8) & emptySquares;
    Bitboard singlePushMask = singlePush;
    while (singlePushMask) {
        int to = __builtin_ctzll(singlePushMask);
        int from = whiteToMove ? to - 8 : to + 8;
        singlePushMask &= singlePushMask - 1;
        moves.emplace_back(from, to);
    }

    // Double pawn moves (only from the starting position)
    Bitboard startRankMask = whiteToMove ? 0x000000000000FF00 : 0x00FF000000000000;
    Bitboard doublePush = whiteToMove ? ((pawns & startRankMask) << 16) & (emptySquares << 8) & emptySquares
                                      : ((pawns & startRankMask) >> 16) & (emptySquares >> 8) & emptySquares;
    Bitboard doublePushMask = doublePush;
    while (doublePushMask) {
        int to = __builtin_ctzll(doublePushMask);
        int from = whiteToMove ? to - 16 : to + 16;
        doublePushMask &= doublePushMask - 1;
        moves.emplace_back(from, to);
    }

    // Pawn captures
    Bitboard leftCaptures = whiteToMove ? (pawns << 9) & opponentPieces & 0xFEFEFEFEFEFEFEFE
                                        : (pawns >> 9) & opponentPieces & 0x7F7F7F7F7F7F7F7F;
    Bitboard rightCaptures = whiteToMove ? (pawns << 7) & opponentPieces & 0x7F7F7F7F7F7F7F7F
                                         : (pawns >> 7) & opponentPieces & 0xFEFEFEFEFEFEFEFE;
    Bitboard captureMask = leftCaptures;
    while (captureMask) {
        int to = __builtin_ctzll(captureMask);
        int from = whiteToMove ? to - 9 : to + 9;
        captureMask &= captureMask - 1;
        moves.emplace_back(from, to, 0, false, true);
    }
    captureMask = rightCaptures;
    while (captureMask) {
        int to = __builtin_ctzll(captureMask);
        int from = whiteToMove ? to - 7 : to + 7;
        captureMask &= captureMask - 1;
        moves.emplace_back(from, to, 0, false, true);
    }

    return moves;
}

std::vector<Move> Board::generateBishopMoves(Bitboard bishops, Bitboard ownPieces, Bitboard opponentPieces) {
    std::vector<Move> moves;
    Bitboard occupiedSquares = ownPieces | opponentPieces;
    const int directions[4] = {9, 7, -9, -7};

    while (bishops) {
        int from = __builtin_ctzll(bishops);
        bishops &= bishops - 1;

        for (int dir : directions) {
            int to = from;
            while (true) {
                to += dir;
                if (to < 0 || to >= 64 || (to % 8 == 0 && dir == 9) || (to % 8 == 7 && dir == 7) ||
                    (to % 8 == 0 && dir == -7) || (to % 8 == 7 && dir == -9)) break;

                if (occupiedSquares & (1ULL << to)) {
                    if (opponentPieces & (1ULL << to)) moves.emplace_back(from, to, 0, false, true);
                    break;
                } else {
                    moves.emplace_back(from, to);
                }
            }
        }
    }
    return moves;
}

std::vector<Move> Board::generateRookMoves(Bitboard rooks, Bitboard ownPieces, Bitboard opponentPieces) {
    std::vector<Move> moves;
    Bitboard occupiedSquares = ownPieces | opponentPieces;
    const int directions[4] = {8, -8, 1, -1};

    while (rooks) {
        int from = __builtin_ctzll(rooks);
        rooks &= rooks - 1;

        for (int dir : directions) {
            int to = from;
            while (true) {
                to += dir;
                if (to < 0 || to >= 64 || (to % 8 == 0 && dir == 1) || (to % 8 == 7 && dir == -1)) break;

                if (occupiedSquares & (1ULL << to)) {
                    if (opponentPieces & (1ULL << to)) moves.emplace_back(from, to, 0, false, true);
                    break;
                } else {
                    moves.emplace_back(from, to);
                }
            }
        }
    }
    return moves;
}

std::vector<Move> Board::generateKnightMoves(Bitboard knights, Bitboard ownPieces) {
    std::vector<Move> moves;
    const int knightMoves[8] = {17, 15, 10, 6, -17, -15, -10, -6};

    while (knights) {
        int from = __builtin_ctzll(knights);
        knights &= knights - 1;

        for (int move : knightMoves) {
            int to = from + move;
            if (to < 0 || to >= 64 || (abs(from % 8 - to % 8) > 2)) continue;

            if (!(ownPieces & (1ULL << to))) {
                moves.emplace_back(from, to);
            }
        }
    }
    return moves;
}

std::vector<Move> Board::generateKingMoves(Bitboard king, Bitboard ownPieces, Bitboard opponentPieces) {
    std::vector<Move> moves;
    const int kingMoves[8] = {8, -8, 1, -1, 9, 7, -9, -7};

    int from = __builtin_ctzll(king);
    for (int move : kingMoves) {
        int to = from + move;
        if (to < 0 || to >= 64 || (from % 8 == 0 && (move == -1 || move == 7 || move == -9)) || 
            (from % 8 == 7 && (move == 1 || move == -7 || move == 9))) continue;

        if (!(ownPieces & (1ULL << to))) {
            moves.emplace_back(from, to);
        }
    }

    // Add castling moves
    if (whiteToMove) {
        // Kingside castling
        if (!whiteKingMoved && !whiteRRookMoved && !(ownPieces & 0x0000000000000006) && !(opponentPieces & 0x0000000000000006)) {
            if (!amIInCheck(whiteToMove) && !isSquareAttacked(from - 1, whiteToMove) && !isSquareAttacked(from - 2, whiteToMove)) {
                moves.emplace_back(from, from - 2); // Kingside castling
            }
        }
        // Queenside castling
        if (!whiteKingMoved && !whiteLRookMoved && !(ownPieces & 0x0000000000000070) && !(opponentPieces & 0x0000000000000070)) {
            if (!amIInCheck(whiteToMove) && !isSquareAttacked(from + 1, whiteToMove) && !isSquareAttacked(from + 2, whiteToMove)) {
                moves.emplace_back(from, from + 2); // Queenside castling
            }
        }
    } else {
        // Kingside castling
        if (!blackKingMoved && !blackRRookMoved && !(ownPieces & 0x600000000000000) && !(opponentPieces & 0x600000000000000)) {
            if (!amIInCheck(whiteToMove) && !isSquareAttacked(from - 1, whiteToMove) && !isSquareAttacked(from - 2, whiteToMove)) {
                moves.emplace_back(from, from + 2); // Kingside castling
            }
        }
        // Queenside castling
        if (!blackKingMoved && !blackLRookMoved && !(ownPieces & 0x7000000000000000) && !(opponentPieces & 0x7000000000000000)) {
            if (!amIInCheck(whiteToMove) && !isSquareAttacked(from + 1, whiteToMove) && !isSquareAttacked(from + 2, whiteToMove)) {
                moves.emplace_back(from, from - 2); // Queenside castling
            }
        }
    }

    return moves;
}

bool Board::isSquareAttacked(int square, bool byWhite) {
    Bitboard opponentPawns = byWhite ? blackPawns : whitePawns;
    Bitboard opponentRooks = byWhite ? blackRooks : whiteRooks;
    Bitboard opponentKnights = byWhite ? blackKnights : whiteKnights;
    Bitboard opponentBishops = byWhite ? blackBishops : whiteBishops;
    Bitboard opponentQueens = byWhite ? blackQueens : whiteQueens;
    Bitboard opponentKing = byWhite ? blackKing : whiteKing;

    // Check for pawn attacks
    Bitboard pawnAttacks = byWhite ? ((opponentPawns >> 7) & 0xFEFEFEFEFEFEFEFE) | ((opponentPawns >> 9) & 0x7F7F7F7F7F7F7F7F)
                                   : ((opponentPawns << 7) & 0x7F7F7F7F7F7F7F7F) | ((opponentPawns << 9) & 0xFEFEFEFEFEFEFEFE);
    if (pawnAttacks & (1ULL << square)) return true;

    // Check for knight attacks
    const int knightMoves[8] = {17, 15, 10, 6, -17, -15, -10, -6};
    for (int move : knightMoves) {
        int to = square + move;
        if (to >= 0 && to < 64 && abs((square % 8) - (to % 8)) <= 2) {
            if (opponentKnights & (1ULL << to)) return true;
        }
    }

    // Check for bishop/queen diagonal attacks
    const int bishopDirections[4] = {9, 7, -9, -7};
    for (int dir : bishopDirections) {
        int to = square;
        while (true) {
            to += dir;
            if (to < 0 || to >= 64 || (to % 8 == 0 && (dir == 9 || dir == -7)) || (to % 8 == 7 && (dir == 7 || dir == -9))) break;
            Bitboard posMask = 1ULL << to;
            if (opponentBishops & posMask || opponentQueens & posMask) return true;
            if (whitePieces & posMask || blackPieces & posMask) break;
        }
    }

    // Check for rook/queen straight attacks
    const int rookDirections[4] = {8, -8, 1, -1};
    for (int dir : rookDirections) {
        int to = square;
        while (true) {
            to += dir;
            if (to < 0 || to >= 64 || (to % 8 == 0 && dir == 1) || (to % 8 == 7 && dir == -1)) break;
            Bitboard posMask = 1ULL << to;
            if (opponentRooks & posMask || opponentQueens & posMask) return true;
            if (whitePieces & posMask || blackPieces & posMask) break;
        }
    }

    // Check for king attacks
    const int kingMoves[8] = {8, -8, 1, -1, 9, 7, -9, -7};
    for (int move : kingMoves) {
        int to = square + move;
        if (to >= 0 && to < 64 && abs((square % 8) - (to % 8)) <= 1) {
            if (opponentKing & (1ULL << to)) return true;
        }
    }

    return false;
}

std::vector<Move> Board::generateQueenMoves(Bitboard queens, Bitboard ownPieces, Bitboard opponentPieces) {
    std::vector<Move> moves;
    std::vector<Move> bishopMoves = generateBishopMoves(queens, ownPieces, opponentPieces);
    std::vector<Move> rookMoves = generateRookMoves(queens, ownPieces, opponentPieces);

    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    return moves;
}

std::vector<Move> Board::generateAllMoves() {
    std::vector<Move> allMoves;

    Bitboard ownPieces = whiteToMove ? whitePieces : blackPieces;
    Bitboard opponentPieces = whiteToMove ? blackPieces : whitePieces;

    // Generate moves for all pieces
    std::vector<Move> pawnMoves = generatePawnMoves(whiteToMove ? whitePawns : blackPawns, ownPieces, opponentPieces);
    std::vector<Move> bishopMoves = generateBishopMoves(whiteToMove ? whiteBishops : blackBishops, ownPieces, opponentPieces);
    std::vector<Move> rookMoves = generateRookMoves(whiteToMove ? whiteRooks : blackRooks, ownPieces, opponentPieces);
    std::vector<Move> knightMoves = generateKnightMoves(whiteToMove ? whiteKnights : blackKnights, ownPieces);
    std::vector<Move> kingMoves = generateKingMoves(whiteToMove ? whiteKing : blackKing, ownPieces, opponentPieces);
    std::vector<Move> queenMoves = generateQueenMoves(whiteToMove ? whiteQueens : blackQueens, ownPieces, opponentPieces);

    // Combine all generated moves into one vector
    allMoves.insert(allMoves.end(), pawnMoves.begin(), pawnMoves.end());
    allMoves.insert(allMoves.end(), bishopMoves.begin(), bishopMoves.end());
    allMoves.insert(allMoves.end(), rookMoves.begin(), rookMoves.end());
    allMoves.insert(allMoves.end(), knightMoves.begin(), knightMoves.end());
    allMoves.insert(allMoves.end(), kingMoves.begin(), kingMoves.end());
    allMoves.insert(allMoves.end(), queenMoves.begin(), queenMoves.end());

    // Filter out moves that put the king in check
    std::vector<Move> legalMoves;
    for (Move& move : allMoves) {
        makeMove(move);
        if (!amIInCheck(!whiteToMove)) {
            legalMoves.push_back(move);
        }
        undoMove(move);
    }

    return legalMoves;
}

void Board::makeMove(Move& move) {

    Bitboard fromMask = 1ULL << move.from;
    Bitboard toMask = 1ULL << move.to;

    move.capturedPiece = 0; // Reset captured piece

    if (whiteToMove) {
        // Update piece bitboards for white
        if (whitePawns & fromMask) {
            whitePawns ^= fromMask | toMask;
        } else if (whiteRooks & fromMask) {
            whiteRooks ^= fromMask | toMask;
            // Update castling rights
            if (move.from == 0) whiteRRookMoved = true; // a1
            if (move.from == 7) whiteLRookMoved = true; // h1
        } else if (whiteKnights & fromMask) {
            whiteKnights ^= fromMask | toMask;
        } else if (whiteBishops & fromMask) {
            whiteBishops ^= fromMask | toMask;
        } else if (whiteQueens & fromMask) {
            whiteQueens ^= fromMask | toMask;
        } else if (whiteKing & fromMask) {
            whiteKing ^= fromMask | toMask;
            // Update castling rights
            whiteKingMoved = true;
            // Handle castling
            if (move.to == move.from - 2) { // Kingside castling
                whiteRooks ^= 0x0000000000000005; // Move the rook
            } else if (move.to == move.from + 2) { // Queenside castling
                whiteRooks ^= 0x0000000000000090; // Move the rook
            }
        }

        // Handle captures
        if (move.isCapture) {
            if (blackPawns & toMask) {
                move.capturedPiece = 'p';
                blackPawns &= ~toMask;
            } else if (blackRooks & toMask) {
                move.capturedPiece = 'r';
                blackRooks &= ~toMask;
            } else if (blackKnights & toMask) {
                move.capturedPiece = 'n';
                blackKnights &= ~toMask;
            } else if (blackBishops & toMask) {
                move.capturedPiece = 'b';
                blackBishops &= ~toMask;
            } else if (blackQueens & toMask) {
                move.capturedPiece = 'q';
                blackQueens &= ~toMask;
            } else if (blackKing & toMask) {
                move.capturedPiece = 'k';
                blackKing &= ~toMask;
            }
        }

        whitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
        blackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;

    } else {
        // Update piece bitboards for black
        if (blackPawns & fromMask) {
            blackPawns ^= fromMask | toMask;
        } else if (blackRooks & fromMask) {
            blackRooks ^= fromMask | toMask;
            // Update castling rights
            if (move.from == 56) blackRRookMoved = true; // a1
            if (move.from == 63) blackLRookMoved = true; // h1
        } else if (blackKnights & fromMask) {
            blackKnights ^= fromMask | toMask;
        } else if (blackBishops & fromMask) {
            blackBishops ^= fromMask | toMask;
        } else if (blackQueens & fromMask) {
            blackQueens ^= fromMask | toMask;
        } else if (blackKing & fromMask) {
            blackKing ^= fromMask | toMask;
            // Update castling rights
            blackKingMoved = true;

            // Handle castling
            if (move.to == move.from - 2) { // Kingside castling
                blackRooks ^= 0x0500000000000000; // Move the rook
            } else if (move.to == move.from + 2) { // Queenside castling
                blackRooks ^= 0x9000000000000000; // Move the rook
            }
        }

        // Handle captures
        if (move.isCapture) {
            if (whitePawns & toMask) {
                move.capturedPiece = 'P';
                whitePawns &= ~toMask;
            } else if (whiteRooks & toMask) {
                move.capturedPiece = 'R';
                whiteRooks &= ~toMask;
            } else if (whiteKnights & toMask) {
                move.capturedPiece = 'N';
                whiteKnights &= ~toMask;
            } else if (whiteBishops & toMask) {
                move.capturedPiece = 'B';
                whiteBishops &= ~toMask;
            } else if (whiteQueens & toMask) {
                move.capturedPiece = 'Q';
                whiteQueens &= ~toMask;
            } else if (whiteKing & toMask) {
                move.capturedPiece = 'K';
                whiteKing &= ~toMask;
            }
        }

        whitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
        blackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;
    }

    // Handle promotions
    if (move.promotion) {
        if (whiteToMove) {
            whitePawns &= ~toMask;
            switch (move.promotion) {
                case 'q': whiteQueens |= toMask; break;
                case 'r': whiteRooks |= toMask; break;
                case 'b': whiteBishops |= toMask; break;
                case 'n': whiteKnights |= toMask; break;
            }
        } else {
            blackPawns &= ~toMask;
            switch (move.promotion) {
                case 'q': blackQueens |= toMask; break;
                case 'r': blackRooks |= toMask; break;
                case 'b': blackBishops |= toMask; break;
                case 'n': blackKnights |= toMask; break;
            }
        }
    }

    whiteToMove = !whiteToMove;
}

void Board::undoMove(const Move& move) {
    Bitboard fromMask = 1ULL << move.from;
    Bitboard toMask = 1ULL << move.to;

    if (!whiteToMove) {
        // Undo move for white
        if (whitePawns & toMask) {
            whitePawns ^= fromMask | toMask;
        } else if (whiteRooks & toMask) {
            whiteRooks ^= fromMask | toMask;
            // restore castling rights
            if (move.from == 0) whiteRRookMoved = true; // a1
            if (move.from == 7) whiteLRookMoved = true; // h1
        } else if (whiteKnights & toMask) {
            whiteKnights ^= fromMask | toMask;
        } else if (whiteBishops & toMask) {
            whiteBishops ^= fromMask | toMask;
        } else if (whiteQueens & toMask) {
            whiteQueens ^= fromMask | toMask;
        } else if (whiteKing & toMask) {
            whiteKing ^= fromMask | toMask;
            whiteKingMoved = false;
            // Restore castling rights
            if (move.to == move.from - 2) { // Kingside castling
                whiteRooks ^= 0x0000000000000005; // Move the rook
            } else if (move.to == move.from + 2) { // Queenside castling
                whiteRooks ^= 0x0000000000000090; // Move the rook
            }
        }

        // Handle captures
        if (move.isCapture) {
            switch (move.capturedPiece) {
                case 'p': blackPawns |= toMask; break;
                case 'r': blackRooks |= toMask; break;
                case 'n': blackKnights |= toMask; break;
                case 'b': blackBishops |= toMask; break;
                case 'q': blackQueens |= toMask; break;
                case 'k': blackKing |= toMask; break;
            }
        }

        whitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
        blackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;
    } else {
        // Undo move for black
        if (blackPawns & toMask) {
            blackPawns ^= fromMask | toMask;
        } else if (blackRooks & toMask) {
            blackRooks ^= fromMask | toMask;
            // restore castling rights
            if (move.from == 56) blackRRookMoved = true;
            if (move.from == 63) blackLRookMoved = true;
        } else if (blackKnights & toMask) {
            blackKnights ^= fromMask | toMask;
        } else if (blackBishops & toMask) {
            blackBishops ^= fromMask | toMask;
        } else if (blackQueens & toMask) {
            blackQueens ^= fromMask | toMask;
        } else if (blackKing & toMask) {
            blackKing ^= fromMask | toMask;
            // Restore castling rights
            blackKingMoved = false;
            if (move.from == 60) { // Kingside castling
                blackRooks ^= 0x0500000000000000; // Move the rook back
            } else if (move.from == 56) { // Queenside castling
                blackRooks ^= 0x9000000000000000; // Move the rook back
            }
        }

        // Handle captures
        if (move.isCapture) {
            switch (move.capturedPiece) {
                case 'P': whitePawns |= toMask; break;
                case 'R': whiteRooks |= toMask; break;
                case 'N': whiteKnights |= toMask; break;
                case 'B': whiteBishops |= toMask; break;
                case 'Q': whiteQueens |= toMask; break;
                case 'K': whiteKing |= toMask; break;
            }
        }

        whitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
        blackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;
    }

    // Handle promotions
    if (move.promotion) {
        if (!whiteToMove) {
            whitePawns |= fromMask;
            switch (move.promotion) {
                case 'q': whiteQueens &= ~toMask; break;
                case 'r': whiteRooks &= ~toMask; break;
                case 'b': whiteBishops &= ~toMask; break;
                case 'n': whiteKnights &= ~toMask; break;
            }
        } else {
            blackPawns |= fromMask;
            switch (move.promotion) {
                case 'q': blackQueens &= ~toMask; break;
                case 'r': blackRooks &= ~toMask; break;
                case 'b': blackBishops &= ~toMask; break;
                case 'n': blackKnights &= ~toMask; break;
            }
        }
    }

    whiteToMove = !whiteToMove;
}

bool Board::amIInCheck(bool player) {
    Bitboard ownKing = player ? whiteKing : blackKing;
    Bitboard opponentPawns = player ? blackPawns : whitePawns;
    Bitboard opponentRooks = player ? blackRooks : whiteRooks;
    Bitboard opponentKnights = player ? blackKnights : whiteKnights;
    Bitboard opponentBishops = player ? blackBishops : whiteBishops;
    Bitboard opponentQueens = player ? blackQueens : whiteQueens;
    Bitboard opponentKing = player ? blackKing : whiteKing;
    Bitboard ownPieces = player ? whitePieces : blackPieces;

    int kingPos = __builtin_ctzll(ownKing);

    // Check for pawn attacks
    Bitboard pawnAttacks = player ? ((opponentPawns >> 7) & 0xFEFEFEFEFEFEFEFE) | ((opponentPawns >> 9) & 0x7F7F7F7F7F7F7F7F)
                                  : ((opponentPawns << 7) & 0x7F7F7F7F7F7F7F7F) | ((opponentPawns << 9) & 0xFEFEFEFEFEFEFEFE);
    

    if (pawnAttacks & ownKing) {
        return true;
    }

    // Check for knight attacks
    const int knightMoves[8] = {17, 15, 10, 6, -17, -15, -10, -6};
    for (int move : knightMoves) {
        int to = kingPos + move;
        if (to >= 0 && to < 64 && abs((kingPos % 8) - (to % 8)) <= 2) {
            if (opponentKnights & (1ULL << to)) return true;
        }
    }

    // Check for bishop/queen diagonal attacks
    const int bishopDirections[4] = {9, 7, -9, -7};
    for (int dir : bishopDirections) {
        int to = kingPos;
        while (true) {
            to += dir;
            if (to < 0 || to >= 64 || (to % 8 == 0 && (dir == 9 || dir == -7)) || (to % 8 == 7 && (dir == 7 || dir == -9))) break;
            Bitboard posMask = 1ULL << to;
            if (opponentBishops & posMask || opponentQueens & posMask) return true;
            if (ownPieces & posMask) break;
        }
    }

    // Check for rook/queen straight attacks
    const int rookDirections[4] = {8, -8, 1, -1};
    for (int dir : rookDirections) {
        int to = kingPos;
        while (true) {
            to += dir;
            if (to < 0 || to >= 64 || (to % 8 == 0 && dir == 1) || (to % 8 == 7 && dir == -1)) break;
            Bitboard posMask = 1ULL << to;
            if (opponentRooks & posMask || opponentQueens & posMask) return true;
            if (ownPieces & posMask) break;
        }
    }

    // Check for king attacks
    const int kingMoves[8] = {8, -8, 1, -1, 9, 7, -9, -7};
    for (int move : kingMoves) {
        int to = kingPos + move;
        if (to >= 0 && to < 64 && abs((kingPos % 8) - (to % 8)) <= 1) {
            if (opponentKing & (1ULL << to)) return true;
        }
    }

    return false;
}

void setBit(Bitboard& bitboard, int square) {
    bitboard |= 1ULL << square;
}

void parseFEN(const std::string& fen, Board& board) {
    std::istringstream iss(fen);
    std::string boardStr, activeColor, castling, enPassant, halfmove, fullmove;

    // Read the components of the FEN string
    iss >> boardStr >> activeColor >> castling >> enPassant >> halfmove >> fullmove;

    // Reset all bitboards
    board.whitePawns = board.blackPawns = 0;
    board.whiteRooks = board.blackRooks = 0;
    board.whiteKnights = board.blackKnights = 0;
    board.whiteBishops = board.blackBishops = 0;
    board.whiteQueens = board.blackQueens = 0;
    board.whiteKing = board.blackKing = 0;

    // Reset castling rights
    board.whiteKingMoved = false;
    board.whiteLRookMoved = false;
    board.whiteRRookMoved = false;
    board.blackKingMoved = false;
    board.blackLRookMoved = false;
    board.blackRRookMoved = false;

    for (char a : castling){
        if (a == 'K') {
            board.whiteRRookMoved = false;
        } else if (a == 'k') {
            board.blackRRookMoved = false;
        } else if (a == 'Q') {
            board.whiteLRookMoved = false;
        } else if (a == 'q') {
            board.blackLRookMoved = false;
        }
    }

    // Parse the board string
    int square = 63;
    for (char c : boardStr) {
        if (c == '/') {
            continue;
        } else if (isdigit(c)) {
            square -= (c - '0');
        } else {
            switch (c) {
                case 'P': setBit(board.whitePawns, square); break;
                case 'R': setBit(board.whiteRooks, square); break;
                case 'N': setBit(board.whiteKnights, square); break;
                case 'B': setBit(board.whiteBishops, square); break;
                case 'Q': setBit(board.whiteQueens, square); break;
                case 'K': setBit(board.whiteKing, square); break;
                case 'p': setBit(board.blackPawns, square); break;
                case 'r': setBit(board.blackRooks, square); break;
                case 'n': setBit(board.blackKnights, square); break;
                case 'b': setBit(board.blackBishops, square); break;
                case 'q': setBit(board.blackQueens, square); break;
                case 'k': setBit(board.blackKing, square); break;
            }
            square--;
        }
    }

    // Set the active color
    board.whiteToMove = (activeColor == "w");

    // Update the overall piece bitboards
    board.whitePieces = board.whitePawns | board.whiteRooks | board.whiteKnights |
                        board.whiteBishops | board.whiteQueens | board.whiteKing;
    board.blackPieces = board.blackPawns | board.blackRooks | board.blackKnights |
                        board.blackBishops | board.blackQueens | board.blackKing;
}