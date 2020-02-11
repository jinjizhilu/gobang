#include "game.h"
#include <cstdlib>
#include <cassert>

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

void Board::PrintScore(int side)
{
	int i0 = (side == Board::E_BLACK) ? 0 : 1;

	cout << " ";
	for (int i = 1; i <= BOARD_SIZE; ++i)
	{
		if (i < 10)
			printf("%5d", i);
		else
			printf("    %c", 'a' + i - 10);
	}
	cout << endl;

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		printf("%c", 'A' + i);

		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			int id = Board::Coord2Id(i, j);
			int score = scoreInfo[i0][id];

			if (score != 0)
				printf("%5d", score);
			else
				printf("     ");
		}
		cout << endl << endl;
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
array<short, GRID_NUM> Board::scoreInfoTemplate;
array<array<char, 8>, GRID_NUM> Board::numInfoTemplate;
array<array<char, 8>, GRID_NUM> Board::maxNumInfoTemplate;

void Board::InitNumInfoTemplates()
{
	for (int i = 0; i < GRID_NUM; ++i)
	{
		Board::scoreInfoTemplate[i] = 0;

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
		scoreInfo[i] = Board::scoreInfoTemplate;
		numInfo[i] = Board::numInfoTemplate;
		numInfoEx[i] = Board::numInfoTemplate;
		maxNumInfo[i] = Board::maxNumInfoTemplate;
	}

	keyGrid = 0xff;
	greatGridNum = 0;
	goodGridNum = 0;
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
				UpdateScore(i0, id1);
			}

			if (chess != side)
				break;
		}

		// update numInfoEx for same side & 1 empty
		id1 = Board::Coord2Id(row0, col0);
		length = maxNumInfo[i0][id][j];
		int emptyCount = 0;

		for (int k = 1; k <= length; ++k)
		{
			id1 += dx + dy;

			int chess = grids[id1];

			if (chess == E_EMPTY)
			{
				if (emptyCount == 1)
				{
					numInfoEx[i0][id1][7 - j] = numInfo[i0][id][7 - j] + k;
					UpdateScore(i0, id1);
				}
				++emptyCount;
			}

			if (chess != side && emptyCount > 1)
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

				char &countEx = numInfoEx[i1][id1][7 - j];
				if (countEx > k - 1)
					countEx = 0;

				UpdateScore(i1, id1);
			}

			if (chess == side)
				break;
		}
	}

	for (int i = 0; i < 2; ++i)
		scoreInfo[i][id] = -side;

	UpdateGridsInfo(i1); // grids info for next turn
}

void Board::UpdateScore(int i0, int id)
{
	int totalScore = 0;

	for (int i = 0; i < 4; ++i)
	{
		int score1 = 0, score2 = 0;

		int num1 = numInfo[i0][id][i];
		int num2 = numInfo[i0][id][7 - i];
		int total1 = num1 + num2 + 1;

		int max1 = maxNumInfo[i0][id][i];
		int max2 = maxNumInfo[i0][id][7 - i];
		int total2 = max1 + max2 + 1;

		bool isOpen1 = max1 > num1;
		bool isOpen2 = max2 > num2;
		bool isTotalOpen = total2 > WIN_COUNT;

		int num3 = numInfoEx[i0][id][i];
		int num4 = numInfoEx[i0][id][7 - i];
		int total3 = num1 + num4 + 1;
		int total4 = num2 + num3 + 1;
		int total5 = max(total3, total4);

		bool isOpen3 = max1 > num3;
		bool isOpen4 = max2 > num4;

		if (total2 >= WIN_COUNT)
		{
			if (total1 >= WIN_COUNT) // continuous 5
			{
				totalScore = WIN_SCORE;
				break;
			}
			else if (total1 == WIN_COUNT - 1)
			{
				if (isOpen1 && isOpen2 && isTotalOpen) // open 4
				{
					score1 = WINNING_SCORE;
				}
				else // half-open 4
				{
					score1 = CHECKMATE_SCORE;
				}
			}
			else if (total1 == WIN_COUNT - 2)
			{
				if (isOpen1 && isOpen2 && isTotalOpen) // open 3
				{
					score1 = GREAT_SCORE;
				}
				else // half-open 3
				{
					score1 = OTHER_SCORE;
				}
			}
			else if (total1 == WIN_COUNT - 3)
			{
				if (isOpen1 && isOpen2 && isTotalOpen) // open 2
				{
					score1 = OTHER_SCORE;
				}
			}

			// jump situations
			if (total5 >= WIN_COUNT)
			{
				if (total3 >= WIN_COUNT && total4 >= WIN_COUNT) // 2 jump 4
				{
					score2 = CHECKMATE_SCORE * 2;
				}
				else // jump 4
				{
					score2 = CHECKMATE_SCORE;
				}
			}
			else if (total5 == WIN_COUNT - 1)
			{
				if (total3 == WIN_COUNT - 2 && isOpen1 && isOpen4 && isTotalOpen)
				{
					score2 = GOOD_SCORE; // jump 3
				}
				else if (total4 == WIN_COUNT - 2 && isOpen2 && isOpen3 && isTotalOpen)
				{
					score2 = GOOD_SCORE; // jump 3
				}
				else
				{
					score2 = OTHER_SCORE; // half-open jump 3
				}
			}
			else if (total5 == WIN_COUNT - 2 && isTotalOpen)
			{
				if (total3 == WIN_COUNT - 2 && isOpen1 && isOpen4)
				{
					score2 = OTHER_SCORE; // jump 2
				}
				else if (total4 == WIN_COUNT - 2 && isOpen2 && isOpen3)
				{
					score2 = OTHER_SCORE; // jump 2
				}
			}
		}
		totalScore += max(score1, score2);
	}
	scoreInfo[i0][id] = totalScore;
}

void Board::UpdateGridsInfo(int i0)
{
	int i1 = 1 - i0;

	int bestScore = WIN_THRESHOLD - 1;
	keyGrid = 0xff;

	greatGridNum = 0;
	goodGridNum = 0;
	poorGridNum = 0;

	for (int i = 0; i < GRID_NUM; ++i)
	{
		int score0 = scoreInfo[i0][i];
		int score1 = scoreInfo[i1][i];

		if (score0 + score1 / 2 > bestScore)
		{
			bestScore = score0 + score1 / 2;
			keyGrid = i;
		}
		else if (score0 >= WINNING_ATTEMP_THRESHOLD || score1 >= WINNING_ATTEMP_THRESHOLD)
		{
			assert(greatGridNum < GREAT_GRID_MAX);
			greatGrids[greatGridNum++] = i;
		}
		else if (score0 > 0 || score1 > 0)
		{
			otherGrids[goodGridNum++] = i;
		}
		else
		{
			otherGrids[GRID_NUM - ++poorGridNum] = i;
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

void GameBase::UpdateValidGrids()
{
	if (board.keyGrid != 0xff)
	{
		validGrids[0] = board.keyGrid;
		validGridCount = 1;
	}
	else if (board.greatGridNum > 0)
	{
		for (int i = 0; i < board.greatGridNum; ++i)
		{
			validGrids[i] = board.greatGrids[i];
		}
		validGridCount = board.greatGridNum;
	}
	else if (board.goodGridNum > 0)
	{
		for (int i = 0; i < board.goodGridNum; ++i)
		{
			validGrids[i] = board.otherGrids[i];
		}
		validGridCount = board.goodGridNum;
	}
	else
	{
		for (int i = 0; i < board.poorGridNum; ++i)
		{
			validGrids[i] = board.otherGrids[GRID_NUM - i - 1];
		}
		validGridCount = board.poorGridNum;
	}
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

int GameBase::GetNextMove()
{
	int id = rand() % validGridCount;
	return validGrids[id];
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

//	board.PrintScore(3 - GetSide());
//	cout << endl;
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
