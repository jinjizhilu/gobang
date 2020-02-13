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

const int WIN_SCORE = 5000; // continuous 5
const int WINNING_SCORE = 1000; // open 4
const int CHECKMATE_SCORE = 70; // half-open 4 or jump 4
const int GREAT_SCORE = 60; // open 3
const int GOOD_SCORE = 50; // jump 3
const int OTHER_SCORE = 5; // half-open 3 or open 2 or jump 2

const int WIN_THRESHOLD = 500;
const int WINNING_ATTEMP_THRESHOLD = 100;
const int COUNTER_WINNING_ATTEMP_THRESHOLD = 50;
const int GOOD_THRESHOLD = 10;

const int LINE_ID_MAX = 262144; // 4 ^ 9

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

	void UpdatScoreInfo(int id);
	void UpdateScore(int row, int col, int rowX, int colX, ChessDirection direction, int side);
	void UpdateGridsInfo(int i0);

	static int Coord2Id(int row, int col);
	static void Id2Coord(int id, int &row, int &col);
	static bool IsValidCoord(int row, int col);
	static void Direction2DxDy(ChessDirection direction, int &dx, int &dy);

	array<char, GRID_NUM> grids;
	array<short, GRID_NUM> scoreInfo[2];

	uint8_t keyGrid;
	array<uint8_t, GRID_NUM> goodGrids;
	array<uint8_t, GRID_NUM> otherGrids;
	int greatGridNum;
	int goodGridNum;
	int poorGridNum;
	int otherGridNum;

private:
	char GetGrid(int row, int col);
	bool SetGrid(int row, int col, char value);

	static void InitLineScoreDict();
	static short CalcLineScore(array<char, 9> line);

	static array<int, LINE_ID_MAX> lineScoreDict;
	static bool isLineScoreDictReady;
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
	bool UpdateValidGridsExtra();
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

