#pragma once

#include "chess.h"
#include <tuple>
#include <iostream>

std::tuple<Move, double_t> perft(Board& board, int depth, int startDepth);

double_t evaluate(Board& board);