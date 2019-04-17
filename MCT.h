#pragma once
#include <conio.h>
#include <atlstr.h>
void printBoard();

void copyBoardInfo(const int M, const int N, const int* top, const int* board,
	const int _lastX, const int _lastY, const int _noX, const int _noY);

void MCTMain(const int player, const int curr);

int getResult();

size_t getNumNode();