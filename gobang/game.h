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

const int FIVE_SCORE = 1000; // continuous 5
const int OPEN_FOUR_SCORE = 250; // open 4
const int CLOSE_FOUR_SCORE = 60; // half-open 4, jump 4
const int OPEN_THREE_SCORE = 50; // open 3, jump 3
const int OTHER_SCORE = 3; // half-open 3 or open 2 or jump 2

const int FOUR_THREE_SCORE = OPEN_THREE_SCORE + CLOSE_FOUR_SCORE; // 3 + 4, 4 + 4
const int THREE_THREE_SCORE = OPEN_THREE_SCORE * 2; // 3 + 3, 3 + 4, 4 + 4
const int TWO_TWO_SCORE = OTHER_SCORE * 2;

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
	
	enum ChessDirection
	{
		E_LEFT = 0,
		E_UP,
		E_UP_LEFT,
		E_UP_RIGHT,
		E_DOWN_LEFT,
		E_DOWN_RIGHT,
		E_DOWN,
		E_RIGHT,
	};

	enum GridType
	{
		E_FIVE = 0,
		E_COUNTER_FIVE,
		E_OPEN_FOUR,
		E_FOUR_THREE,
		E_CLOSE_FOUR,
		E_COUNTER_FOUR_THREE,
		E_THREE_THREE,
		E_COUNTER_THREE_THREE,
		E_OPEN_THREE,
		E_COUNTER_OPEN_THREE,
		E_TWO_TWO,
		E_OPEN_TWO,
		E_OTHER,
		E_GRID_TYPE_MAX,
	};

	enum ChessPriority
	{
		E_HIGHEST = 0,
		E_HIGH,
		E_MIDDLE,
		E_LOW,
		E_LOWEST,
		E_PRIORITY_MAX,
	};

	Board();

	void Clear();
	void Print(int lastChess);
	void PrintScore(int side);
	void PrintPriority();

	int GetChessNumInLine(int id, ChessDirection direction);
	void GetGridsByPriority(ChessPriority priority, array<uint8_t, GRID_NUM> &result, int &count);

	void UpdatScoreInfo(int id);

	static int Coord2Id(int row, int col);
	static void Id2Coord(int id, int &row, int &col);
	static bool IsValidCoord(int row, int col);
	static void Direction2DxDy(ChessDirection direction, int &dx, int &dy);
	
	uint8_t	keyGrid;
	array<char, GRID_NUM> grids;
	array<short, GRID_NUM> scoreInfo[2];
	array<char, GRID_NUM> gridCheckStatus;
	array<bool, E_GRID_TYPE_MAX> hasGridType;
	array<bool, E_PRIORITY_MAX> hasPriority;

private:
	char GetGrid(int row, int col);
	bool SetGrid(int row, int col, char value);

	void UpdateScore(int row, int col, int rowX, int colX, ChessDirection direction, int side);
	void UpdateGridsInfo(int i0);
	void FindOtherGrids(int i0, int id, GridType type);

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

