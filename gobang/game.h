#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <list>

#pragma warning (disable:4244)
#pragma warning (disable:4018)

using namespace std;

const int BOARD_SIZE = 15;
const int WIN_COUNT = 5;
const int GRID_NUM = BOARD_SIZE * BOARD_SIZE;

class Board
{
public:
	enum Chess
	{
		E_EMPTY,
		E_BLACK,
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
	void Print(int lastChess);
	int GetChessNumInLine(int id, ChessDirection dir);
	bool CheckNeighbourChessNumWithSide(int id, int side, int radius, int num);
	bool CheckNeighbourChessNum(int id, int radius, int num);

	static int Coord2Id(int row, int col);
	static void Id2Coord(int id, int &row, int &col);
	static bool IsValidCoord(int row, int col);

	array<char, GRID_NUM> grids;

private:
	char GetGrid(int row, int col);
	bool SetGrid(int row, int col, char value);
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
	bool PutChess(int id);
	bool PutRandomChess();
	void Regret(int step = 2);
	void Print();
	int GetState() { return state; }

	static int Str2Id(const string &str);
	static string Id2Str(int id);

	int GetTurn() { return turn; }
	int GetLastMove() { return lastMove;}
	int GetSide();
	int GetEmptyGridCount() { return emptyGridCount; }
	array<uint8_t, GRID_NUM>& GetEmptyGrids() { return emptyGrids; }
	bool IsLonelyGrid(int id, int radius);
	const vector<uint8_t>& GetRecord() { return record; }

	Game* Clone() { return new Game(*this);	}
	void SetSimMode(bool value) { isSimMode = value; }

private:
	bool IsWinThisTurn(int move);
	void UpdateEmptyGrids();

	Board board;
	bool isSimMode;
	int state;
	int turn;
	int lastMove;
	int emptyGridCount;
	vector<uint8_t> record;
	array<uint8_t, GRID_NUM> emptyGrids;
};

