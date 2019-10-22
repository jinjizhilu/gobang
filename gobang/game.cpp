#include "game.h"
#include <cstdlib>
#include <ctime>

void Board::Clear()
{
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
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

Int2 Board::GetNthEmptyGrid(int n)
{
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			if (GetGrid(i, j) == E_EMPTY)
			{
				if (--n == 0)
					return Int2(i, j);
			}
		}
	}
	return Int2(0, 0);
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

Int2 Board::GetCoord(const string &input)
{
	int row = input[0] - 'A';
	int col = input[1] <= '9' ? input[1] - '1' : input[1] - 'a' + 9;
	return Int2(row, col);
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
}

bool Game::PutChess(int row, int col)
{
	if (state != E_NORMAL || board.GetGrid(row, col) != Board::E_EMPTY)
		return false;

	int side = GetSide();
	if (!board.SetGrid(row, col, side))
		return false;


	record.push_back(Int2(row, col));
	++turn;

	if (IsWinThisTurn())
		state = (side == Board::E_BALCK) ? E_BLACK_WIN : E_WHITE_WIN;

	if (turn > BOARD_SIZE * BOARD_SIZE)
		state = E_DRAW;

	return true;
}

bool Game::PutChess(const string &move)
{
	Int2 coord = Board::GetCoord(move);
	return PutChess(coord.x, coord.y);
}

bool Game::PutRandomChess()
{
	int emptyCount = BOARD_SIZE * BOARD_SIZE - turn;
	int id = rand() % emptyCount;
	Int2 grid = board.GetNthEmptyGrid(id);

	return PutChess(grid.x, grid.y);
}

void Game::Print()
{
	string stateText[] = { "Normal", "Black Win!", "White Win!", "Draw" };

	printf("=== Current State: %s ===\n", stateText[state].c_str());
	board.Print();
}

bool Game::IsWinThisTurn()
{
	Int2 chess = record.back();

	int maxLine_L_R = board.GetChessNumInLine(chess.x, chess.y, Board::E_L_R);
	int maxLine_T_B = board.GetChessNumInLine(chess.x, chess.y, Board::E_T_B);
	int maxLine_TL_BR = board.GetChessNumInLine(chess.x, chess.y, Board::E_TL_BR);
	int maxLine_TR_BL = board.GetChessNumInLine(chess.x, chess.y, Board::E_TR_BL);
	int maxInLine = max(maxLine_L_R, max(maxLine_T_B, max(maxLine_TL_BR, maxLine_TR_BL)));

	return maxInLine >= WIN_COUNT;
}