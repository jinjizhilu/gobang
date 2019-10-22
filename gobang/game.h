#pragma once
#include <iostream>
#include <string>
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
	bool SetGrid(int row, int col, short value);
	int GetChessNumInLine(int row, int col, ChessDirection dir);

	static Int2 GetCoord(const string &input);

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
	bool PutChess(const string &move);
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

