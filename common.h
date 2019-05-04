#pragma once
#define DEBUG_OFF
#ifdef DEBUG_ON
#include <conio.h>
#include <atlstr.h>
#endif

constexpr size_t MAX_NODE = 5000000;
constexpr double TIME = 2.;  // time limit of MCTS
constexpr size_t MAX_BOARD_SIZE = 12;
constexpr double UCT_C = 1/1.414;  // param for score(curr)/visit(curr) + "C" * sqrt(2* ln(visit(parent)) / visit(curr))
// the bigger C is, the more exploration will occur, conversely, more conservative and adhere to better results
