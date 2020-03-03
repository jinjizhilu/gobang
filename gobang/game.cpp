#include "game.h"
#include <cstdlib>
#include <cassert>

#define max(a, b) ((a > b) ? a : b)
#define PRINT_SCORE 0
#define PRINT_PRIORITY 0

Board::Board()
{
	if (!isLineScoreDictReady)
		InitLineScoreDict();

	Clear();
}

void Board::Clear()
{
	grids.fill(E_EMPTY);
	scoreInfo[0].fill(0);
	scoreInfo[1].fill(0);
	gridCheckStatus.fill(E_PRIORITY_MAX);
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

void Board::PrintPriority()
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
			int grid = grids[id];

			if (grid == E_EMPTY)
			{
				int priority = gridCheckStatus[id];
				if (priority < E_OTHER)
				{
					printf("%d ", priority + 1);
				}
				else
				{
					printf("  ");
				}
			}
			if (grid == E_BLACK)
			{
				cout << "@ ";
			}
			if (grid == E_WHITE)
			{
				cout << "X ";
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

	int row0, col0;
	Board::Id2Coord(id, row0, col0);

	int dx, dy;
	Board::Direction2DxDy(direction, dx, dy);

	int leftCount = 0;
	int row = row0, col = col0;
	for (int i = 0; i < 4; ++i)
	{
		row -= dy; col -= dx;

		if (GetGrid(row, col) != side)
			break;

		++leftCount;
	}

	int rightCount = 0;
	row = row0; col = col0;
	for (int i = 0; i < 4; ++i)
	{
		row += dy; col += dx;

		if (GetGrid(row, col) != side)
			break;

		++rightCount;
	}

	int num = leftCount + rightCount + 1;

	return num;
}

bool Board::isLineScoreDictReady = false;
array<int, LINE_ID_MAX> Board::lineScoreDict;

void Board::InitLineScoreDict()
{
	FILE *fp;
	fopen_s(&fp, "line_dict.log", "w");
	char strmap[4] = { ' ', '@', 'O', 'X' };

	int maxId = pow(4, 9);

	for (int i = 0; i < maxId; ++i)
	{
		array<char, 9> line;
		char lineStr[12];
		bool isValid = true;

		int key = i;
		for (int j = 0; j < 9; ++j)
		{
			line[j] = key % 4;
			key = key >> 2;
			lineStr[j + 1] = strmap[line[j]];

			if (j == 4 && (line[j] == E_INVALID || line[j] == E_EMPTY))
			{
				isValid = false;
				break;
			}
		}
		lineStr[0] = lineStr[10] = '|';
		lineStr[11] = 0;

		int leftCount = 0;
		for (int j = 0; j < 4; ++j)
		{
			if (line[j] != E_INVALID)
				++leftCount;

			if (line[j] == E_INVALID && leftCount > 0)
			{
				isValid = false;
				break;
			}
		}

		int rightCount = 0;
		for (int j = 8; j > 4; --j)
		{
			if (line[j] != E_INVALID)
				++rightCount;

			if (line[j] == E_INVALID && rightCount > 0)
			{
				isValid = false;
				break;
			}
		}

		if (isValid)
		{
			short score = CalcLineScore(line);
			lineScoreDict[i] = score;

			if (score > 0)
			{
				fprintf(fp, "%8d, %5d,   %s\n", i, score, lineStr);
			}
		}
	}
	fclose(fp);

	isLineScoreDictReady = true;
}

short Board::CalcLineScore(array<char, 9> line)
{
	int side = line[4];
	int otherSide = 3 - side;

	// continuous grids
	int num1 = 0;
	for (int i = 3; i >= 0; --i)
	{
		if (line[i] != side)
			break;

		++num1;
	}

	int num2 = 0;
	for (int i = 5; i <= 8; ++i)
	{
		if (line[i] != side)
			break;

		++num2;
	}

	// continuous grid with 1 empty
	bool isValid = false;
	bool foundEmpty = false;
	int num3 = 0;
	for (int i = 3; i >= 0; --i)
	{
		if (line[i] == otherSide || line[i] == E_INVALID)
			break;

		if (!foundEmpty)
		{
			if (line[i] == E_EMPTY)
				foundEmpty = true;
		}
		else
		{
			if (line[i] == E_EMPTY)
				break;

			if (line[i] == side)
				isValid = true;
		}

		++num3;
	}
	if (!isValid)
		num3 = 0;

	isValid = false;
	foundEmpty = false;
	int num4 = 0;
	for (int i = 5; i <= 8; ++i)
	{
		if (line[i] == otherSide || line[i] == E_INVALID)
			break;

		if (!foundEmpty)
		{
			if (line[i] == E_EMPTY)
				foundEmpty = true;
		}
		else
		{
			if (line[i] == E_EMPTY)
				break;

			if (line[i] == side)
				isValid = true;
		}

		++num4;
	}
	if (!isValid)
		num4 = 0;


	// continuous grid or empty
	int max1 = 0;
	for (int i = 3; i >= 0; --i)
	{
		if (line[i] == otherSide || line[i] == E_INVALID)
			break;

		++max1;
	}

	int max2 = 0;
	for (int i = 5; i <= 8; ++i)
	{
		if (line[i] == otherSide || line[i] == E_INVALID)
			break;

		++max2;
	}

	// calculate line score
	int total1 = num1 + num2 + 1;
	int total2 = max1 + max2 + 1;

	bool isOpen1 = max1 > num1;
	bool isOpen2 = max2 > num2;
	bool isTotalOpen = total2 > WIN_COUNT;

	int total3 = num1 + num4 + 1;
	int total4 = num2 + num3 + 1;
	int total5 = max(total3, total4);

	bool isOpen3 = max1 > num3;
	bool isOpen4 = max2 > num4;

	int score1 = 0, score2 = 0;

	if (total2 >= WIN_COUNT)
	{
		// continuous situations
		if (total1 >= WIN_COUNT) // continuous 5
		{
			score1 = WIN_SCORE;
		}
		else if (total1 == WIN_COUNT - 1)
		{
			if (isOpen1 && isOpen2 && isTotalOpen) // open 4
			{
				score1 = WINNING_SCORE;
			}
			else // half-open 4
			{
				score1 = GOOD_SCORE;
			}
		}
		else if (total1 == WIN_COUNT - 2)
		{
			if (isOpen1 && isOpen2 && isTotalOpen) // open 3
			{
				score1 = GOOD_SCORE;
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
				score2 = WINNING_SCORE;
			}
			else // jump 4
			{
				score2 = GOOD_SCORE;
			}
		}
		else if (total5 == WIN_COUNT - 1)
		{
			if (total3 >= WIN_COUNT - 2 && isOpen1 && isOpen4 && isTotalOpen)
			{
				score2 = GOOD_SCORE; // jump 3
			}
			else if (total4 >= WIN_COUNT - 2 && isOpen2 && isOpen3 && isTotalOpen)
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
	int score = max(score1, score2);

	return score;
}

void Board::UpdatScoreInfo(int id)
{
	int row0, col0;
	Board::Id2Coord(id, row0, col0);

	int side = GetGrid(row0, col0);
	int otherSide = 3 - side;

	int i0 = (side == E_BLACK) ? 0 : 1; // this side
	int i1 = 1 - i0; // other side

	for (int j = 0 ; j < 8; ++j)
	{
		int dx, dy;
		Board::Direction2DxDy((ChessDirection)j, dx, dy);

		bool needUpdate0 = true, needUpdate1 = true;
		int row = row0, col = col0;
		for (int k = 1; k <= 4; ++k)
		{
			row += dy; col += dx;

			int chess = GetGrid(row, col);

			if (chess == E_EMPTY)
			{
				if (needUpdate0)
					UpdateScore(row, col, row0, col0, (ChessDirection)j, side);

				if (needUpdate1)
					UpdateScore(row, col, row0, col0, (ChessDirection)j, otherSide);
			}
			else if (chess == side)
			{
				needUpdate1 = false; // no need to update other side
			}
			else if (chess == otherSide)
			{
				needUpdate0 = false; // no need to udpate this side
			}

			if (!needUpdate0 && !needUpdate1)
				break;
		}
	}

	for (int i = 0; i < 2; ++i)
		scoreInfo[i][id] = -side;

	UpdateGridsInfo(i1); // grids info for next turn
}

void Board::UpdateScore(int row, int col, int rowX, int colX, ChessDirection direction, int side)
{
	int key = side << (4 * 2);
	int key0 = key;

	int dx, dy;
	Board::Direction2DxDy(direction, dx, dy);

	int row1 = row, col1 = col;
	for (int i = 0; i < 4; ++i)
	{
		row1 -= dy; col1 -= dx;

		int value = GetGrid(row1, col1) << ((3 - i) * 2);
		key += value;

		if (!(row1 == rowX && col1 == colX))
			key0 += value;
	}

	row1 = row, col1 = col;
	for (int i = 0; i < 4; ++i)
	{
		row1 += dy; col1 += dx;

		int value = GetGrid(row1, col1) << ((5 + i) * 2);
		key += value;

		if (!(row1 == rowX && col1 == colX))
			key0 += value;
	}

	int lineScore0 = lineScoreDict[key0];
	int lineScore = lineScoreDict[key];

	int i0 = (side == E_BLACK) ? 0 : 1; // this side
	scoreInfo[i0][Board::Coord2Id(row, col)] += lineScore - lineScore0;
}

void Board::FindOtherGrids(int i0, int id)
{
	int side = (i0 == 0) ? E_BLACK : E_WHITE;
	int otherSide = 3 - side;

	int row, col;
	Board::Id2Coord(id, row, col);

	for (int d = 0; d < 4; ++d)
	{
		int dx, dy;
		Board::Direction2DxDy((Board::ChessDirection)d, dx, dy);

		// calc origin key & line score
		int key = side << ( 4 * 2);
		int row1 = row, col1 = col;
		for (int i = 0; i < 4; ++i)
		{
			row1 -= dy; col1 -= dx;

			int value = GetGrid(row1, col1) << ((3 - i) * 2);
			key += value;
		}

		row1 = row, col1 = col;
		for (int i = 0; i < 4; ++i)
		{
			row1 += dy; col1 += dx;

			int value = GetGrid(row1, col1) << ((5 + i) * 2);
			key += value;
		}
		int lineScore = lineScoreDict[key];

		// check valid grids
		row1 = row, col1 = col;
		for (int i = 0; i < 4; ++i)
		{
			row1 -= dy; col1 -= dx;

			int chess = GetGrid(row1, col1);

			if (chess == E_INVALID || chess == otherSide)
				break;

			if (chess == E_EMPTY)
			{
				int value = otherSide << ((3 - i) * 2);
				int key1 = key + value;
				int lineScore1 = lineScoreDict[key1];
				int newScore = scoreInfo[i0][id] + lineScore1 - lineScore;

				if (newScore < WINNING_ATTEMP_THRESHOLD)
				{
					int id1 = Board::Coord2Id(row1, col1);
					gridCheckStatus[id1] = E_GREAT;
				}
			}
		}

		row1 = row, col1 = col;
		for (int i = 0; i < 4; ++i)
		{
			row1 += dy; col1 += dx;

			int chess = GetGrid(row1, col1);

			if (chess == E_INVALID || chess == otherSide)
				break;

			if (chess == E_EMPTY)
			{
				int value = otherSide << ((5 + i) * 2);
				int key1 = key + value;
				int lineScore1 = lineScoreDict[key1];
				int newScore = scoreInfo[i0][id] + lineScore1 - lineScore;

				if (newScore < WINNING_ATTEMP_THRESHOLD)
				{
					int id1 = Board::Coord2Id(row1, col1);
					gridCheckStatus[id1] = E_GREAT;
				}
			}
		}
	}
}

__declspec(noinline)
void Board::UpdateGridsInfo(int i0)
{
	int i1 = 1 - i0;

	int bestScore = 0;
	keyGrid = 0xff;

	gridCheckStatus.fill(E_PRIORITY_MAX);
	hasPriority.fill(false);

	for (int i = 0; i < GRID_NUM; ++i)
	{
		int score0 = scoreInfo[i0][i];
		int score1 = scoreInfo[i1][i];

		if (score0 >= WIN_THRESHOLD)
		{
			keyGrid = i;
			gridCheckStatus[i] = E_WINNING;
			hasPriority[E_WINNING] = true;
			return;
		}
		else if (score1 >= WIN_THRESHOLD)
		{
			keyGrid = i;
			bestScore = score1;
			gridCheckStatus[i] = E_WINNING;
			hasPriority[E_WINNING] = true;
		}
		else if (score0 >= WINNING_THRESHOLD)
		{
			if (score0 > bestScore)
			{
				keyGrid = i;
				bestScore = score0;
				gridCheckStatus[i] = E_WINNING;
				hasPriority[E_WINNING] = true;
			}
		}
		else if (score1 >= WINNING_ATTEMP_THRESHOLD)
		{
			gridCheckStatus[i] = E_GREAT;
			hasPriority[E_GREAT] = true;

			FindOtherGrids(i1, i); // find other possible counter moves
		}
		else if (score0 >= WINNING_ATTEMP_THRESHOLD)
		{
			gridCheckStatus[i] = E_GREAT;
			hasPriority[E_GREAT] = true;
		}
		else if (score0 >= GOOD_THRESHOLD || score1 >= GOOD_THRESHOLD)
		{
			if (gridCheckStatus[i] > E_GOOD) // may be E_GREAT by FindOtherGrids
			{
				gridCheckStatus[i] = E_GOOD;
				hasPriority[E_GOOD] = true;
			}
		}
		else if (score0 > 0 || score1 > 0)
		{
			if (gridCheckStatus[i] > E_POOR) // may be E_GREAT by FindOtherGrids
			{
				gridCheckStatus[i] = E_POOR;
				hasPriority[E_POOR] = true;
			}
		}
		else
		{
			if (gridCheckStatus[i] > E_OTHER) // may be E_GREAT by FindOtherGrids
			{
				gridCheckStatus[i] = E_OTHER;
				hasPriority[E_OTHER] = true;
			}
		}
	}
}

void Board::GetGridsByPriority(ChessPriority priority, array<uint8_t, GRID_NUM> &result, int &count)
{
	count = 0;
	for (int i = 0; i < GRID_NUM; ++i)
	{
		if (gridCheckStatus[i] == priority)
		{
			result[count++] = i;
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

void Board::Direction2DxDy(ChessDirection direction, int &dx, int &dy)
{
	dx = dy = 0;

	switch (direction)
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
	board.UpdatScoreInfo(id);

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
		return;
	}
	
	for (int i = Board::E_GREAT; i < Board::E_PRIORITY_MAX; ++i)
	{
		if (board.hasPriority[i])
		{
			board.GetGridsByPriority((Board::ChessPriority)i, validGrids, validGridCount);
			break;
		}
	}
}

bool GameBase::UpdateValidGridsExtra()
{
	if (board.keyGrid == 0xff && !board.hasPriority[Board::E_GREAT] && 
		board.hasPriority[Board::E_GOOD] && board.hasPriority[Board::E_POOR])
	{
		board.GetGridsByPriority(Board::E_POOR, validGrids, validGridCount);
		return true;
	}
	return false;
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
		board.UpdatScoreInfo(id);
	}
}

void Game::Print()
{
	string stateText[] = { "Normal", "Black Win!", "White Win!", "Draw" };

	printf("=== Current State: %s ===\n", stateText[state].c_str());
	board.Print(lastMove);
	cout << endl;

	if (PRINT_SCORE)
	{
		board.PrintScore(3 - GetSide());
		board.PrintScore(GetSide());
		cout << endl;
	}

	if (PRINT_PRIORITY)
	{
		board.PrintPriority();
		cout << endl;
	}
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
