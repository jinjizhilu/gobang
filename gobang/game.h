#pragma once
#include <iostream>
#include <vector>

using namespace std;

const int BOARD_SIZE = 15;
const int WIN_COUNT = 5;

struct Int2
{
	Int2(int x, int y) : x(x), y(y) {}
	int x, y;
};

class Board
{
public:
	enum Chess
	{
		E_EMPTY,
		E_BALCK,
		E_WHITE,
		E_INVALID,
	};
	
	enum ChessDirection {
		E_L_R,
		E_T_B,
		E_TL_BR,
		E_TR_BL,
	};

	void Clear();
	void Print();
	Int2 GetNthEmptyGrid(int n);
	int GetGrid(int row, int col);
	void SetGrid(int row, int col, short value);
	int GetChessNumInLine(int row, int col, ChessDirection dir);

	short grids[BOARD_SIZE * BOARD_SIZE];
};

class Game
{
public:
	enum State
	{
		E_NORMAL,
		E_BLACK_WIN,
		E_WHITE_WIN,
		E_DRAW,
	};

	Game();
	void Init();
	bool PutChess(int row, int col);
	bool PutRandomChess();
	void Print();
	int GetState() { return state; }

private:
	bool IsWinThisTurn();

	int GetSide()
	{
		return (turn % 2 == 1) ? Board::E_BALCK : Board::E_WHITE;
	}

	Board board;
	int state;
	int turn;
	vector<Int2> record;
};

