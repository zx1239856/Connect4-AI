#include "MCT.h"
#include "Judge.h"
#include "common.h"
#include <cmath>
#include <cstdlib>
#include <tuple>

int num_node = 0; // number of total situations(nodes)
int tree_node[MAX_NODE][MAX_BOARD_SIZE]; // MCT  nodes, N-ary tree
int score[MAX_NODE][MAX_BOARD_SIZE]; // scores of the nodes
int visit_child[MAX_NODE][MAX_BOARD_SIZE]; // visit time of child nodes
int visit_parent[MAX_NODE]; // visit time of parent nodes

namespace stack
{
	int tree_stack[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
	int board_stack[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
	int stack_top = 0;

	inline void clearStack()
	{
		stack_top = 0;
	}

	inline void pushStack(const int &node, const int &col)
	{
		tree_stack[stack_top++] = node;
		board_stack[stack_top - 1] = col;
	}

	inline int stackSize()
	{
		return stack_top;
	}

	inline std::pair<int, int> popStack()
	{
		if (--stack_top >= 0)
			return { tree_stack[stack_top], board_stack[stack_top] };
		else
			return { -1, -1 };
	}

	inline std::pair<int, int> stackTop()
	{
		if (stack_top)
			return { tree_stack[stack_top], board_stack[stack_top] };
		else
			return { -1, -1 };
	}
}

int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
int top[MAX_BOARD_SIZE];
int M, N;
int noX, noY;
int lastX, lastY;

/********** Local Helpers ***********
*************************************/

inline void placeChess(const int &y, const int &player)
{
	board[--top[y]][y] = player;
	if (noX == top[y] - 1 && noY == y)
		--top[y];
}

inline void removeChess(const int &y)
{
	if (noX == top[y] && noY == y)
		++top[y];
	board[top[y]++][y] = 0;
}

inline bool colFull(const int &y)
{
	return top[y] == 0;
}

// find a point either machine or human wins
int evalBestPoint(const int &player)
{
	auto judger = player == 1 ? userWin : machineWin;
	int best = -1;
	for (int y = 0; y < N; ++y)
	{
		int x = top[y] - 1;
		if (x >= 0)
		{
			placeChess(y, player);
			bool win = judger(x, y, M, N, (const int *const)board);
			removeChess(y);
			if (win)
			{
				if (best == -1)
					best = y;
				else
				{
					best = best * N + y; break; // in this case, wherever 3 - player puts, he's doomed to fail
				}
			}
		}
	}
	return best;
}

/********** Implementations **********
*************************************/

void printBoard()
{
	_cprintf("===============================================\n");
	for (int i = 0; i < M; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (i == noX && j == noY)
				_cprintf("X ");
			else
				_cprintf("%c ", '0' + board[i][j]);
		}
		_cprintf("\n");
	}
	_cprintf("===============================================\n");
}

int decideWhichChild(const int &curr)
{
	bool has_unvisited = false;

	for (int i = 0; i < N; ++i)
		if (!colFull(i) && !visit_child[curr][i]) // has unvisited child, randomly pick-up one
		{
			has_unvisited = true;  break;
		}

	if (has_unvisited)
	{
		int col = std::rand() % N;
		while (colFull(col)) col = std::rand() % N;
		return col;
	}

	// use UCT to evaluate best child
	double max_score = -INFINITY;
	int candidate = -1;
	for (int i = 0; i < N; ++i)
	{
		if (!colFull(i))
		{
			double val = static_cast<double>(score[curr][i]) / visit_child[curr][i] + UCT_C * std::sqrt(2 * std::log(visit_parent[curr]) / visit_child[curr][i]);
			if (val > max_score)
				max_score = val, candidate = i;
		}
	}
	return candidate;
}

void copyBoardInfo(const int _M, const int _N, const int * _top, const int * _board, const int _lastX, const int _lastY, const int _noX, const int _noY)
{
	stack::clearStack();
	// reset previous
	for (int i = 0; i < num_node; ++i)
	{
		visit_parent[i] = 0;
		for (int j = 0; j < _N; ++j)
			score[i][j] = 0, visit_child[i][j] = 0, tree_node[i][j] = 0;
	}
	num_node = 1; // root node always exists
	// copy current
	M = _M, N = _N;
	noX = _noX, noY = _noY;
	lastX = _lastX, lastY = _lastY;
	for (int i = 0; i < _M; i++) {
		for (int j = 0; j < _N; j++) {
			board[i][j] = _board[i * _N + j];
		}
	}
	for (int i = 0; i < _N; ++i)
		top[i] = _top[i];
}


void MCTMain(const int _player, const int _curr)
{
	int player = _player, curr = _curr;
	int winner = 0;
	
	while (true)
	{
		int child = decideWhichChild(curr);
		if (child == -1)
		{
			winner = 0; // tie
			break;
		}

		int best = evalBestPoint(3 - player);
		if (best >= N) // player fail
		{
			winner = 3 - player;
			break;
		}
		else if (best > -1)
		{
#ifdef DEBUG_ON
			if (curr == 0 && player == 2)
				_cprintf("Critical!!!(Put here or fail), col: %d\n", best);
#endif
			child = best;
		}
		best = evalBestPoint(player);
		if (best > -1)
		{
			child = best % N;
			score[curr][child] += 1;
			++visit_child[curr][child], ++visit_parent[curr];
			winner = player; // player win
			break;
		}

		if (visit_child[curr][child] == 0) // create a new node
		{
			tree_node[curr][child] = num_node++;
		}
		placeChess(child, player);
		stack::pushStack(curr, child);
		player = 3 - player, curr = tree_node[curr][child];
	}
	while (stack::stackSize())
	{
		player = 3 - player;
		auto parent = stack::popStack();
		removeChess(parent.second);
		if (winner == player)
		{
			++score[parent.first][parent.second];
		}
		else if (winner == 3 - player)
		{
			score[parent.first][parent.second];
		}
		++visit_child[parent.first][parent.second], ++visit_parent[parent.first];
	}
}

int getResult()
{
	double max_score = -INFINITY;
	int best = -1;
	AllocConsole();
	for (int i = 0; i < N; ++i)
	{
		double s = static_cast<double>(score[0][i]) / max(1, visit_child[0][i]);
#ifdef DEBUG_ON
		_cprintf("%.3f\t", s);
#endif
		if (s > max_score)
			max_score = s, best = i;
	}
#ifdef DEBUG_ON
	_cprintf("\n");
#endif
	return best;
}

size_t getNumNode()
{
	return num_node;
}

