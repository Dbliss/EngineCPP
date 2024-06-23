#ifndef ENGINE_H
#define ENGINE_H

#include "chess.h"
#include <tuple>

std::tuple<int, int, int> perft(Board& board, int depth);

#endif // ENGINE_H
