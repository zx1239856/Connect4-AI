#pragma once
#define DEBUG_OFF
#ifdef DEBUG_ON
#include <conio.h>
#include <atlstr.h>
#endif

constexpr size_t MAX_NODE = 5000000;
constexpr double TIME = 2.3;
constexpr size_t MAX_BOARD_SIZE = 12;
constexpr double UCT_C = 1.;  // param for score(curr)/visit(curr) + "C" * sqrt(2* ln(visit(parent)) / visit(curr))
