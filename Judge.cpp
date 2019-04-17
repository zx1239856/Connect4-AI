#include "Judge.h"
#include "common.h"

bool userWin(const int x, const int y, const int M, const int N, const int *const board){
	//横向检测
    int i, j;
    int count = 0;
    for (i = y; i >= 0; i--)
        if (!(board[x * MAX_BOARD_SIZE + i] == 1))
            break;
    count += (y - i);
    for (i = y; i < N; i++)
        if (!(board[x * MAX_BOARD_SIZE + i] == 1))
            break;
    count += (i - y - 1);
    if (count >= 4) return true;

    //纵向检测
    count = 0;
    for (i = x; i < M; i++)
        if (!(board[i * MAX_BOARD_SIZE + y] == 1))
            break;
    count += (i - x);
    if (count >= 4) return true;

    //左下-右上
    count = 0;
    for (i = x, j = y; i < M && j >= 0; i++, j--)
        if (!(board[i * MAX_BOARD_SIZE + j] == 1))
            break;
    count += (y - j);
    for (i = x, j = y; i >= 0 && j < N; i--, j++)
        if (!(board[i * MAX_BOARD_SIZE + j] == 1))
            break;
    count += (j - y - 1);
    if (count >= 4) return true;

    //左上-右下
    count = 0;
    for (i = x, j = y; i >= 0 && j >= 0; i--, j--)
        if (!(board[i * MAX_BOARD_SIZE + j] == 1))
            break;
    count += (y - j);
    for (i = x, j = y; i < M && j < N; i++, j++)
        if (!(board[i * MAX_BOARD_SIZE + j] == 1))
            break;
    count += (j - y - 1);
    if (count >= 4) return true;

    return false;
}

bool machineWin(const int x, const int y, const int M, const int N, const int *const board){
	//横向检测
    int i, j;
    int count = 0;
    for (i = y; i >= 0; i--)
        if (!(board[x * MAX_BOARD_SIZE + i] == 2))
            break;
    count += (y - i);
    for (i = y; i < N; i++)
        if (!(board[x * MAX_BOARD_SIZE + i] == 2))
            break;
    count += (i - y - 1);
    if (count >= 4) return true;

    //纵向检测
    count = 0;
    for (i = x; i < M; i++)
        if (!(board[i * MAX_BOARD_SIZE + y] == 2))
            break;
    count += (i - x);
    if (count >= 4) return true;

    //左下-右上
    count = 0;
    for (i = x, j = y; i < M && j >= 0; i++, j--)
        if (!(board[i * MAX_BOARD_SIZE + j] == 2))
            break;
    count += (y - j);
    for (i = x, j = y; i >= 0 && j < N; i--, j++)
        if (!(board[i * MAX_BOARD_SIZE + j] == 2))
            break;
    count += (j - y - 1);
    if (count >= 4) return true;

    //左上-右下
    count = 0;
    for (i = x, j = y; i >= 0 && j >= 0; i--, j--)
        if (!(board[i * MAX_BOARD_SIZE + j] == 2))
            break;
    count += (y - j);
    for (i = x, j = y; i < M && j < N; i++, j++)
        if (!(board[i * MAX_BOARD_SIZE + j] == 2))
            break;
    count += (j - y - 1);
    if (count >= 4) return true;

    return false;
}

bool isTie(const int N, const int* top){
	bool tie = true;
    for (int i = 0; i < N; i++)
    {
        if (top[i] > 0)
        {
            tie = false;
            break;
        }
    }
    return tie;
}