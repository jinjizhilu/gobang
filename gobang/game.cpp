#include "game.h"
#include <cstdlib>
#include <ctime>

#define max(a, b) ((a > b) ? a : b)

Board::Board()
{
	
}

void Board::Clear()
{
	for (int i = 0; i < GRID_NUM; ++i)
	{
		grids[i] = E_EMPTY;
	}
}

void Board::Print()
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
			int grid = GetGrid(i, j);

			if (grid == E_EMPTY)
			{
				cout << "+ ";
			}
			if (grid == E_BALCK)
			{
				cout << "O ";
			}
			if (grid == E_WHITE)
			{
				cout << "@ ";
			}
		}
		cout << endl;
	}
}

int Board::GetGrid(int row, int col)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
		return E_INVALID;

	return grids[row * BOARD_SIZE + col];
}

bool Board::SetGrid(int row, int col, short value)
{
	if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
		return false;

	grids[row * BOARD_SIZE + col] = value;
	return true;
}

int Board::GetChessNumInLine(int row, int col, ChessDirection dir)
{
	Int2 diff = Int2(0, 0);
	switch (dir)
	{
	case Board::E_L_R:
		diff = Int2(1, 0);
		break;
	case Board::E_T_B:
		diff = Int2(0, 1);
		break;
	case Board::E_TL_BR:
		diff = Int2(1, 1);
		break;
	case Board::E_TR_BL:
		diff = Int2(1, -1);
		break;
	}

	int count = 0;
	int side = GetGrid(row, col);

	for (int i = 1; i < BOARD_SIZE; ++i)
	{
		if (GetGrid(row + diff.x * i, col + diff.y * i) != side)
		{
			count += i;
			break;
		}
	}

	for (int i = 1; i < BOARD_SIZE; ++i)
	{
		if (GetGrid(row - diff.x * i, col - diff.y * i) != side)
		{
			count += i;
			break;
		}
	}
	--count;

	return count;
}

///////////////////////////////////////////////////////////////////////

Game::Game()
{
	srand((unsigned)time(NULL));
	Init();
}

void Game::Init()
{
	turn = 1;
	board.Clear();
	record.clear();
	state = E_NORMAL;
	isSimMode = false;
	
	emptyGrids.clear();
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			emptyGrids.push_back(Int2(i, j));
		}
	}
}

bool Game::PutChess(int row, int col)
{
	if (state != E_NORMAL || board.GetGrid(row, col) != Board::E_EMPTY)
		return false;

	int side = GetSide();
	if (!board.SetGrid(row, col, side))
		return false;

	lastMove = Int2(row, col);

	if (!isSimMode)
		record.push_back(lastMove);

	UpdateEmptyGrids();
	++turn;

	if (IsWinThisTurn(lastMove))
		state = (side == Board::E_BALCK) ? E_BLACK_WIN : E_WHITE_WIN;

	if (turn > GRID_NUM)
		state = E_DRAW;

	return true;
}

bool Game::PutRandomChess()
{
	int id = rand() % emptyGrids.size();
	swap(emptyGrids[id], emptyGrids.back());
	Int2 grid = emptyGrids.back();

	return PutChess(grid.x, grid.y);
}

void Game::UpdateEmptyGrids()
{
	if (emptyGrids.back() == lastMove)
	{
		emptyGrids.pop_back();
	}
	else
	{
		for (int i = emptyGrids.size() - 1; i >= 0; --i)
		{
			if (emptyGrids[i] == lastMove)
			{
				swap(emptyGrids[i], emptyGrids.back());
				emptyGrids.pop_back();
				break;
			}
		}
	}
}

void Game::Print()
{
	string stateText[] = { "Normal", "Black Win!", "White Win!", "Draw" };

	printf("=== Current State: %s ===\n", stateText[state].c_str());
	board.Print();
	cout << endl;
}

int Game::GetSide()
{
	return (turn % 2 == 1) ? Board::E_BALCK : Board::E_WHITE;
}

bool Game::IsWinThisTurn(Int2 move)
{
	int maxLine_L_R = board.GetChessNumInLine(move.x, move.y, Board::E_L_R);
	int maxLine_T_B = board.GetChessNumInLine(move.x, move.y, Board::E_T_B);
	int maxLine_TL_BR = board.GetChessNumInLine(move.x, move.y, Board::E_TL_BR);
	int maxLine_TR_BL = board.GetChessNumInLine(move.x, move.y, Board::E_TR_BL);
	int maxInLine = max(maxLine_L_R, max(maxLine_T_B, max(maxLine_TL_BR, maxLine_TR_BL)));

	return maxInLine >= WIN_COUNT;
}

Int2 Game::Str2Coord(const string &str)
{
	int row = str[0] - 'A';
	int col = str[1] <= '9' ? str[1] - '1' : str[1] - 'a' + 9;
	return Int2(row, col);
}

string Game::Coord2Str(Int2 coord)
{
	string result(1, coord.x + 'A');
	result += (coord.y <= 9) ? coord.y + '1' : coord.y + 'a' - 9;
	return result;
}
