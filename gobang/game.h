#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <list>

#pragma warning (disable:4244)
#pragma warning (disable:4018)

using namespace std;

const int BOARD_SIZE = 15;
const int WIN_COUNT = 5;
const int GRID_NUM = BOARD_SIZE * BOARD_SIZE;

struct Int2
{
	Int2() : x(-1), y(-1) {}
	Int2(int x, int y) : x(x), y(y) {}
	int x, y;

	bool operator==(const Int2& other)
	{
		return x == other.x && y == other.y;
	}
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

	Board();

	void Clear();
	void Print(Int2 lastChess);
	int GetGrid(int row, int col);
	bool SetGrid(int row, int col, short value);
	int GetChessNumInLine(int row, int col, ChessDirection dir);
	bool CheckNeighbourChessNumWithSide(int row, int col, int side, int radius, int num);
	bool CheckNeighbourChessNum(int row, int col, int radius, int num);

	short grids[GRID_NUM];
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
	void Regret(int step = 2);
	void Print();
	int GetState() { return state; }
	static Int2 Str2Coord(const string &str);
	static string Coord2Str(Int2 coord);

	int GetTurn() { return turn; }
	Int2 GetLastMove() { return lastMove;}
	int GetSide();
	vector<Int2>& GetEmptyGrids() { return emptyGrids; }
	bool IsLonelyGrid(int row, int col, int radius);
	const vector<Int2>& GetRecord() { return record; }

	Game* Clone() { return new Game(*this);	}
	void SetSimMode(bool value) { isSimMode = value; }

private:
	bool IsWinThisTurn(Int2 move);
	void UpdateEmptyGrids();

	Board board;
	bool isSimMode;
	int state;
	int turn;
	Int2 lastMove;
	vector<Int2> record;
	vector<Int2> emptyGrids;
};

