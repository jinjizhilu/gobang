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

const int WIN_SCORE = 9999; // continuous 5
const int WINNING_SCORE = 1000; // open 4
const int WIN_THRESHOLD = 500;
const int WINNING_ATTEMP_THRESHOLD = 100;
const int CHECKMATE_SCORE = 70; // half-open 4 or jump 4
const int GREAT_SCORE = 60; // open 3
const int GOOD_SCORE = 50; // jump 3
const int OTHER_SCORE = 5; // half-open 3 or open 2 or jump 2
const int GREAT_GRID_MAX = 20;

class Board
{
public:
	enum Chess
	{
		E_EMPTY = 0,
		E_BLACK,
		E_WHITE,
		E_INVALID,
	};
	
	enum ChessDirection {
		E_LEFT = 0,
		E_UP,
		E_UP_LEFT,
		E_UP_RIGHT,
		E_DOWN_LEFT,
		E_DOWN_RIGHT,
		E_DOWN,
		E_RIGHT,
	};

	Board();

	void Clear();
	void Print(int lastChess);
	void PrintScore(int side);
	int GetChessNumInLine(int id, ChessDirection direction);
	bool CheckNeighbourChessNumWithSide(int id, int side, int radius, int num);
	bool CheckNeighbourChessNum(int id, int radius, int num);

	void ClearInfo();
	void UpdateInfo(int id);
	void UpdateScore(int i0, int id);
	void UpdateGridsInfo(int i0);

	static int Coord2Id(int row, int col);
	static void Id2Coord(int id, int &row, int &col);
	static bool IsValidCoord(int row, int col);

	array<char, GRID_NUM> grids;
	array<short, GRID_NUM> scoreInfo[2];
	array<array<char, 8>, GRID_NUM> numInfo[2]; // continuous grids with same color
	array<array<char, 8>, GRID_NUM> numInfoEx[2]; // continuous grids with same color and 1 empty grid
	array<array<char, 8>, GRID_NUM> maxNumInfo[2]; // continuous grids with same color or empty grid

	uint8_t keyGrid;
	array<uint8_t, GREAT_GRID_MAX> greatGrids;
	array<uint8_t, GRID_NUM> otherGrids;
	int greatGridNum;
	int goodGridNum;
	int poorGridNum;

private:
	char GetGrid(int row, int col);
	bool SetGrid(int row, int col, char value);

	static void InitNumInfoTemplates();

	static array<short, GRID_NUM> scoreInfoTemplate;
	static array<array<char, 8>, GRID_NUM> numInfoTemplate;
	static array<array<char, 8>, GRID_NUM> maxNumInfoTemplate;
	static bool numInfoTemplatesReady;
};

class GameBase
{
public:
	enum State
	{
		E_NORMAL,
		E_BLACK_WIN,
		E_WHITE_WIN,
		E_DRAW,
	};

	GameBase();
	void Init();
	bool PutChess(int id);
	int GetSide();
	bool IsLonelyGrid(int id, int radius);
	bool IsWinThisTurn(int move);
	void UpdateValidGrids();
	int GetNextMove();

	Board board;
	int state;
	int turn;
	int lastMove;
	int validGridCount;
	array<uint8_t, GRID_NUM> validGrids;
};

class Game : private GameBase
{
public:
	int GetState() { return state; }
	int GetTurn() { return turn; }

	bool PutChess(int id);
	void Regret(int step = 2);
	void Print();

	const vector<uint8_t>& GetRecord() { return record; }
	static int Str2Id(const string &str);
	static string Id2Str(int id);

private:
	void RebuildBoardInfo();

	vector<uint8_t> record;
};

