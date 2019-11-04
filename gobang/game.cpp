#include "game.h"
#include <cstdlib>

#define max(a, b) ((a > b) ? a : b)

Board::Board()
{
	Clear();
}

void Board::Clear()
{
	for (int i = 0; i < GRID_NUM; ++i)
	{
		grids[i] = E_EMPTY;
	}
}

void Board::Print(int lastChess)
{
	cout << " ";
	for (int i = 1; i <= BOARD_SIZE; ++i)
	{
		if (i < 10)
			printf("%2d", i);
		else
			printf(" %c", 'a' + i - 10);
	}
	cout << endl;

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		printf("%c ", 'A' + i);

		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			int id = Board::Coord2Id(i, j);
			if (id == lastChess)
			{
				cout << "X ";
				continue;
			}

			int grid = grids[id];

			if (grid == E_EMPTY)
			{
				cout << "+ ";
			}
			if (grid == E_BLACK)
			{
				cout << "@ ";
			}
			if (grid == E_WHITE)
			{
				cout << "O ";
			}
		}
		cout << endl;
	}
}

char Board::GetGrid(int row, int col)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
		return E_INVALID;

	return grids[Board::Coord2Id(row, col)];
}

bool Board::SetGrid(int row, int col, char value)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
		return false;

	grids[Board::Coord2Id(row, col)] = value;
	return true;
}

int Board::GetChessNumInLine(int id, ChessDirection dir)
{
	int row = 0, col = 0;
	Board::Id2Coord(id, row, col);

	int dx = 0, dy = 0;
	switch (dir)
	{
	case Board::E_L_R:
		dx = 1;
		break;
	case Board::E_T_B:
		dy = 1;
		break;
	case Board::E_TL_BR:
		dx = dy = 1;
		break;
	case Board::E_TR_BL:
		dx = 1;
		dy = -1;
		break;
	}

	int count = 0;
	int side = GetGrid(row, col);

	for (int i = 1; i < BOARD_SIZE; ++i)
	{
		if (GetGrid(row + dx * i, col + dy * i) != side)
		{
			count += i;
			break;
		}
	}

	for (int i = 1; i < BOARD_SIZE; ++i)
	{
		if (GetGrid(row - dx * i, col - dy * i) != side)
		{
			count += i;
			break;
		}
	}
	--count;

	return count;
}

bool Board::CheckNeighbourChessNumWithSide(int id, int side, int radius, int num)
{
	int row, col;
	Board::Id2Coord(id, row, col);

	int count = 0;
	for (int i = row - radius; i <= row + radius; ++i)
	{
		for (int j = col - radius; j <= col + radius; ++j)
		{
			if (Board::IsValidCoord(i, j) && GetGrid(i, j) == side)
			{
				if (++count >= num)
					return true;
			}
		}
	}
	return false;
}

bool Board::CheckNeighbourChessNum(int id, int radius, int num)
{
	int row, col;
	Board::Id2Coord(id, row, col);

	int count = 0;
	for (int i = row - radius; i <= row + radius; ++i)
	{
		for (int j = col - radius; j <= col + radius; ++j)
		{
			if (Board::IsValidCoord(i, j) && GetGrid(i, j) != E_EMPTY)
			{
				if (++count >= num)
					return true;
			}
		}
	}
	return false;
}

int Board::Coord2Id(int row, int col)
{
	return row * BOARD_SIZE + col;
}

void Board::Id2Coord(int id, int &row, int &col)
{
	row = id / BOARD_SIZE;
	col = id % BOARD_SIZE;
}

bool Board::IsValidCoord(int row, int col)
{
	return (0 <= row && row < BOARD_SIZE && 0 <= col && col < BOARD_SIZE);
}

///////////////////////////////////////////////////////////////////////

GameBase::GameBase()
{
	Init();
}

void GameBase::Init()
{
	turn = 1;
	lastMove = -1;
	board.Clear();
	state = E_NORMAL;
	
	emptyGridCount = GRID_NUM;
	for (int i = 0; i < GRID_NUM; ++i)
	{
		emptyGrids[i] = i;
	}
}

bool GameBase::PutChess(int id)
{
	if (state != E_NORMAL || board.grids[id] != Board::E_EMPTY)
		return false;

	int side = GetSide();
	board.grids[id] = side;

	lastMove = id;

	UpdateEmptyGrids();
	++turn;

	if (IsWinThisTurn(lastMove))
		state = (side == Board::E_BLACK) ? E_BLACK_WIN : E_WHITE_WIN;

	if (turn > GRID_NUM)
		state = E_DRAW;

	return true;
}

bool GameBase::PutRandomChess()
{
	int id = rand() % emptyGridCount;
	swap(emptyGrids[id], emptyGrids[emptyGridCount - 1]);
	int gridId = emptyGrids[emptyGridCount - 1];

	return PutChess(gridId);
}

void GameBase::UpdateEmptyGrids()
{
	for (int i = emptyGridCount - 1; i >= 0; --i)
	{
		if (emptyGrids[i] == lastMove)
		{
			swap(emptyGrids[i], emptyGrids[emptyGridCount - 1]);
			--emptyGridCount;
			break;
		}
	}
}

bool GameBase::IsLonelyGrid(int id, int radius)
{
	//int oppnentSide = ((turn + 1) % 2 == 1) ? Board::E_BALCK : Board::E_WHITE;
	int hasNeighbour = board.CheckNeighbourChessNum(id, radius, 1);
	return !hasNeighbour;
}

int GameBase::GetSide()
{
	return (turn % 2 == 1) ? Board::E_BLACK : Board::E_WHITE;
}

bool GameBase::IsWinThisTurn(int move)
{
	int maxLine_L_R = board.GetChessNumInLine(move, Board::E_L_R);
	int maxLine_T_B = board.GetChessNumInLine(move, Board::E_T_B);
	int maxLine_TL_BR = board.GetChessNumInLine(move, Board::E_TL_BR);
	int maxLine_TR_BL = board.GetChessNumInLine(move, Board::E_TR_BL);
	int maxInLine = max(maxLine_L_R, max(maxLine_T_B, max(maxLine_TL_BR, maxLine_TR_BL)));

	return maxInLine >= WIN_COUNT;
}

///////////////////////////////////////////////////////////////////

bool Game::PutChess(int Id)
{
	if (GameBase::PutChess(Id))
	{
		record.push_back(lastMove);
		return true;
	}
	return false;
}

void Game::Regret(int step)
{
	while (!record.empty() && --step >= 0)
	{
		--turn;
		emptyGrids[emptyGridCount++] = record.back();
		board.grids[record.back()] = Board::E_EMPTY;
		record.pop_back();
	}

	lastMove = record.empty() ? -1 : record.back();
}

void Game::Print()
{
	string stateText[] = { "Normal", "Black Win!", "White Win!", "Draw" };

	printf("=== Current State: %s ===\n", stateText[state].c_str());
	board.Print(lastMove);
	cout << endl;
}

int Game::Str2Id(const string &str)
{
	int row = str[0] - 'A';
	int col = str[1] <= '9' ? str[1] - '1' : str[1] - 'a' + 9;
	if (!Board::IsValidCoord(row, col))
		return -1;

	int id = Board::Coord2Id(row, col);
	return id;
}

string Game::Id2Str(int id)
{
	int row, col;
	Board::Id2Coord(id, row, col);
	string result(1, row + 'A');
	result += (col < 9) ? col + '1' : col + 'a' - 9;
	return result;
}
