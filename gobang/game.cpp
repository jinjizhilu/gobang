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
	ClearInfo();
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

int Board::GetChessNumInLine(int id, ChessDirection direction)
{
	int side = grids[id];
	int i0 = (side == E_BLACK) ? 0 : 1; // this side

	int num = numInfo[i0][id][direction] + numInfo[i0][id][7 - direction] + 1;
	return num;
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

bool Board::numInfoTemplatesReady = false;
array<array<char, 8>, GRID_NUM> Board::numInfoTemplate;
array<array<char, 8>, GRID_NUM> Board::maxNumInfoTemplate;

void Board::InitNumInfoTemplates()
{
	for (int i = 0; i < GRID_NUM; ++i)
	{
		int row, col;
		Board::Id2Coord(i, row, col);

		for (int j = 0; j < 8; ++j)
		{
			Board::numInfoTemplate[i][j] = 0;

			switch ((ChessDirection)j)
			{
			case E_LEFT:
				Board::maxNumInfoTemplate[i][j] = col;
				break;
			case E_RIGHT:
				Board::maxNumInfoTemplate[i][j] = BOARD_SIZE - col - 1;
				break;
			case E_UP:
				Board::maxNumInfoTemplate[i][j] = row;
				break;
			case E_DOWN:
				Board::maxNumInfoTemplate[i][j] = BOARD_SIZE - row - 1;
				break;
			case E_UP_LEFT:
				Board::maxNumInfoTemplate[i][j] = min(row, col);
				break;
			case E_DOWN_RIGHT:
				Board::maxNumInfoTemplate[i][j] = min(BOARD_SIZE - row - 1, BOARD_SIZE - col - 1);
				break;
			case E_UP_RIGHT:
				Board::maxNumInfoTemplate[i][j] = min(row, BOARD_SIZE - col - 1);
				break;
			case E_DOWN_LEFT:
				Board::maxNumInfoTemplate[i][j] = min(BOARD_SIZE - row - 1, col);
				break;
			}
			Board::maxNumInfoTemplate[i][j] = min((int)Board::maxNumInfoTemplate[i][j], BOARD_SIZE);
		}
	}

	Board::numInfoTemplatesReady = true;
}

void Board::ClearInfo()
{
	if (!Board::numInfoTemplatesReady)
		Board::InitNumInfoTemplates();

	for (int i = 0; i < 2; ++i)
	{
		numInfo[i] = Board::numInfoTemplate;
		maxNumInfo[i] = Board::maxNumInfoTemplate;
	}
}

void Board::UpdateInfo(int id)
{
	int row0, col0;
	Board::Id2Coord(id, row0, col0);

	int side = GetGrid(row0, col0);

	int i0 = (side == E_BLACK) ? 0 : 1; // this side
	int i1 = 1 - i0; // other side

	for (int j = 0 ; j < 8; ++j)
	{
		int dx = 0, dy = 0;

		switch ((ChessDirection)j)
		{
		case E_LEFT:
			dx = -1;
			break;
		case E_RIGHT:
			dx = 1;
			break;
		case E_UP:
			dy = -1;
			break;
		case E_DOWN:
			dy = 1;
			break;
		case E_UP_LEFT:
			dx = dy = -1;
			break;
		case E_DOWN_RIGHT:
			dx = dy = 1;
			break;
		case E_UP_RIGHT:
			dx = 1; dy = -1;
			break;
		case E_DOWN_LEFT:
			dx = -1; dy = 1;
			break;
		}
		dy *= BOARD_SIZE;

		// update numInfo for same side
		int id1 = Board::Coord2Id(row0, col0);
		int length = maxNumInfo[i0][id][j];

		for (int k = 1; k <= length; ++k)
		{
			id1 += dx + dy;

			int chess = grids[id1];

			if (chess == E_EMPTY)
			{
				numInfo[i0][id1][7 - j] = numInfo[i0][id][7 - j] + k;
			}

			if (chess != side)
				break;
		}

		//update maxNumInfo for other side
		id1 = Board::Coord2Id(row0, col0);
		length = maxNumInfo[i1][id][j];

		for (int k = 1; k <= length; ++k)
		{
			id1 += dx + dy;

			int chess = grids[id1];

			if (chess == E_EMPTY)
			{
				maxNumInfo[i1][id1][7 - j] = k - 1;
			}

			if (chess == side)
				break;
		}
	}
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
	
	validGridCount = GRID_NUM;
	for (int i = 0; i < GRID_NUM; ++i)
	{
		validGrids[i] = i;
	}
}

bool GameBase::PutChess(int id)
{
	if (state != E_NORMAL || board.grids[id] != Board::E_EMPTY)
		return false;

	int side = GetSide();
	board.grids[id] = side;
	board.UpdateInfo(id);

	lastMove = id;

	UpdateValidGrids();
	++turn;

	if (IsWinThisTurn(lastMove))
		state = (side == Board::E_BLACK) ? E_BLACK_WIN : E_WHITE_WIN;

	if (turn > GRID_NUM)
		state = E_DRAW;

	return true;
}

bool GameBase::PutRandomChess()
{
	int id = rand() % validGridCount;
	swap(validGrids[id], validGrids[validGridCount - 1]);
	int gridId = validGrids[validGridCount - 1];

	return PutChess(gridId);
}

void GameBase::UpdateValidGrids()
{
	for (int i = validGridCount - 1; i >= 0; --i)
	{
		if (validGrids[i] == lastMove)
		{
			swap(validGrids[i], validGrids[validGridCount - 1]);
			--validGridCount;
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
	for (int i = 0; i < 4; ++i)
	{
		if (board.GetChessNumInLine(move, (Board::ChessDirection)i) >= WIN_COUNT)
			return true;
	}
	return false;
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
		validGrids[validGridCount++] = record.back();
		board.grids[record.back()] = Board::E_EMPTY;
		record.pop_back();
	}
	RebuildBoardInfo();

	lastMove = record.empty() ? -1 : record.back();
}

void Game::RebuildBoardInfo()
{
	board.Clear();

	for (int i = 0; i < record.size(); ++i)
	{
		int side = (i % 2 == 0) ? Board::E_BLACK : Board::E_WHITE;
		int id = record[i];

		board.grids[id] = side;
		board.UpdateInfo(id);
	}
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
