#include "MCT.h"
#include "Judge.h"
#include "common.h"
#include <cmath>
#include <cstdlib>
#include <tuple>

int num_node = 0; // number of total situations(nodes)
int tree_node[MAX_NODE][MAX_BOARD_SIZE]; // MCT nodes, N-ary tree
int score[MAX_NODE][MAX_BOARD_SIZE]; // scores of the nodes
int visit[MAX_NODE];
int feasible[MAX_BOARD_SIZE];
int unvisited_feasible[MAX_BOARD_SIZE];

inline int relu(const int &val)
{
	return val >= 1 ? val : 1;
}

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


constexpr int GAME_WIN = 2;
constexpr int GAME_TIE = 1;
constexpr int GAME_CONTINUE = 0;

inline int gameOver(const int &player, int x, int y)
{
	auto judge = player == 1 ? userWin : machineWin;
	if (judge(x, y, M, N, (const int * const)board))
		return GAME_WIN;
	else
	{
		for (int i = 0; i < N; ++i)
			if (!colFull(i))
				return GAME_CONTINUE;
		return GAME_TIE;
	}
}

// find a point either machine or human wins
inline int evalBestPoint(const int &player)
{
	int best = -1;
	for (int y = 0; y < N; ++y)
	{
		int x = top[y] - 1;
		if (x >= 0)
		{
			placeChess(y, player);
			int code = gameOver(player, x, y);
			removeChess(y);
			if (code == GAME_WIN)
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

inline int UCTBestChild(const int &curr, const size_t &feasible_cnt)
{
	// use UCT to evaluate best child
	double max_score = -INFINITY;
	int candidate = -1;
	for (size_t i = 0; i < feasible_cnt; ++i)
	{
		double val = static_cast<double>(score[curr][feasible[i]]) / visit[tree_node[curr][feasible[i]]] + UCT_C * std::sqrt(2 * std::log(visit[curr]) / visit[tree_node[curr][feasible[i]]]);
		if (val > max_score)
			max_score = val, candidate = feasible[i];
	}
	return candidate;
}

int treePolicy(int &player, int &curr)
{
	while (true)
	{
		size_t cnt = 0;
		int best = evalBestPoint(3 - player);
		if (best >= 0 && best < N)
			feasible[0] = best, cnt = 1;
		best = evalBestPoint(player);
		if (best >= 0)
		{
			best %= N;
			// prune if player wins
			if (!tree_node[curr][best])
				tree_node[curr][best] = num_node++;
			placeChess(best, player);
			stack::pushStack(curr, best);
			return GAME_WIN;
		}
		if (!cnt)
		{
			for (int i = 0; i < N; ++i)
				if (!colFull(i))
					feasible[cnt++] = i;
		}
		if (cnt)
		{
			size_t unvisited_cnt = 0;
			for (size_t i = 0; i < cnt; ++i)
			{
				if (!tree_node[curr][feasible[i]])
					unvisited_feasible[unvisited_cnt++] = feasible[i];
			}
			if (!unvisited_cnt) // all visited, use UCT to select best
			{
				int child = UCTBestChild(curr, cnt);
				placeChess(child, player);
				stack::pushStack(curr, child);
				curr = tree_node[curr][child];
				player = 3 - player;
				continue; // continue to play
			}
			else // have unvisited child, randomly pick up one
			{
				int child = unvisited_feasible[rand() % unvisited_cnt];
				placeChess(child, player);
				stack::pushStack(curr, child);
				tree_node[curr][child] = num_node;
				curr = num_node++ - 1;
				return GAME_CONTINUE;
			}
		}
		else return GAME_TIE;
	}
}

void copyBoardInfo(const int _M, const int _N, const int * _top, const int * _board, const int _lastX, const int _lastY, const int _noX, const int _noY)
{
	stack::clearStack();
	// reset previous
	for (int i = 0; i < num_node; ++i)
	{
		visit[i] = 0;
		for (int j = 0; j < _N; ++j)
			score[i][j] = 0, tree_node[i][j] = 0;
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
	
	int stat = treePolicy(player, curr);
	int bonus = 0;
	int player_tree_node = player;

	constexpr int REWARD = 5;

	// default policy
	if (stat == GAME_CONTINUE)
	{
		int sim_cnt = 0;
		while (true)
		{
			player = 3 - player;
			++sim_cnt;
			int f_cnt = 0;
			int best = evalBestPoint(3 - player);
			if (best >= 0 && best < N)
				feasible[0] = best, f_cnt = 1;
			best = evalBestPoint(player);
			if (best >= 0)
				feasible[0] = best % N, f_cnt = 1;
			if (!f_cnt)
			{
				for (int i = 0; i < N; ++i)
					if (!colFull(i))
						feasible[f_cnt++] = i;
			}
			if (f_cnt)
			{
				int choose = feasible[rand() % f_cnt];
				int x = top[choose] - 1;
				placeChess(choose, player);
				stack::pushStack(-1, choose);
				stat = gameOver(player, x, choose);
			}
			else
			{
				winner = 0;
				break;
			}
			if (stat == GAME_TIE)
			{
				winner = 0;
				break;
			}
			else if (stat == GAME_WIN)
			{
				winner = player;
				bonus = sim_cnt == 1 ? REWARD : 1;
				break;
			}
		}
	}
	else if (stat == GAME_WIN)
		winner = player_tree_node, bonus = REWARD;
	player = player_tree_node;
	// backup
	while (stack::stackSize())
	{
		auto parent = stack::popStack();
		removeChess(parent.second);
		if (parent.first == -1)
			continue;
		if (winner == player)
		{
			score[parent.first][parent.second] += (1 + bonus);
		}
		else if (winner == 3 - player)
		{
			score[parent.first][parent.second] -= relu(bonus - 1);
		}
		if (bonus > 0)
			--bonus;
		player = 3 - player;
		++visit[parent.first], ++visit[tree_node[parent.first][parent.second]];
	}
}

int getResult()
{
	double max_score = -INFINITY;
	int best = -1;
#ifdef DEBUG_ON
	AllocConsole();
#endif
	for (int i = 0; i < N; ++i)
	{
		if (!tree_node[0][i])
			continue;
		double s = static_cast<double>(score[0][i]) / visit[tree_node[0][i]];
#ifdef DEBUG_ON
		_cprintf("%.4f\t", s);
#endif
		if (s > max_score && !colFull(i))
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

