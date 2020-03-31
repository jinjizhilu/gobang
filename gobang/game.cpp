#include "game.h"
#include <cstdlib>
#include <cassert>
#include <cmath>

#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a < b) ? a : b)

#define GAME_LOG_FILE "Game.log"

#define USE_BEAUTIFUL_BOARD 1
#define OUTPUT_LINE_SCORE_DICT 0
#define OUTPUT_RESTRICTED_SCORE 0

bool Board::RestrictedMoveRule = false;

bool Board::isLineScoreDictReady = false;
array<int, LINE_ID_MAX> Board::lineScoreDict;

Board::Board()
{
	if (!isLineScoreDictReady)
		InitLineScoreDict();

	Clear();
}

void Board::Clear()
{
	hashKey = 0;
	grids.fill(E_EMPTY);
	scoreInfo[0].fill(0);
	scoreInfo[1].fill(0);
	gridCheckStatus.fill(E_PRIORITY_MAX);
}

void Board::TestPrint()
{
	for (int i = 0; i < GRID_NUM; ++i)
		grids[i] = rand() % 3;

	PrintNew(rand() % 10);
}

void Board::PrintNew(int lastChess)
{
	cout << endl << "  £Á£Â£Ã£Ä£Å£Æ£Ç£È£É£Ê£Ë£Ì£Í£Î£Ï" << endl;

	string digits[] = {"£±", "£²", "£³", "£´", "£µ", "£¶", "£·", "£¸", "£¹", "£á", "£â", "£ã", "£ä", "£å", "£æ"};

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		cout << digits[i];

		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			int id = Board::Coord2Id(i, j);
			int grid = grids[id];

			if (id == lastChess)
			{
				cout << (grid == E_BLACK ? "¡ò": "¡ò");
				continue;
			}

			if (grid == E_EMPTY)
			{
				if (i == 0)
				{
					if (j == 0)
					{
						cout << "©³ ";
					}
					else if (j == BOARD_SIZE - 1)
					{
						cout << "©· ";
					}
					else
					{
						cout << "©Ó ";
					}
				}
				else if (i == BOARD_SIZE - 1)
				{
					if (j == 0)
					{
						cout << "©» ";
					}
					else if (j == BOARD_SIZE - 1)
					{
						cout << "©¿ ";
					}
					else
					{
						cout << "©Û ";
					}
				}
				else if (j == 0)
				{
					cout << "©Ä ";
				}
				else if (j == BOARD_SIZE - 1)
				{
					cout << "©Ì ";
				}
				else
				{
					cout << "©à ";
				}
			}
			if (grid == E_BLACK)
			{
				cout << "¡ñ";
			}
			if (grid == E_WHITE)
			{
				cout << "¡ð";
			}
		}
		cout << endl;
	}
	cout << endl;
}


void Board::Print(int lastChess, bool isLog)
{
	if (USE_BEAUTIFUL_BOARD && !isLog)
	{
		PrintNew(lastChess);
		return;
	}

	const char *format = (isLog ? "%c  " : "%c ");

	cout << (isLog ? "   " : "  ");
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		printf(format, 'A' + i);
	}
	cout << endl;

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		if (i < 9)
			printf(format, '1' + i);
		else
			printf(format, 'a' + i - 9);

		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			int id = Board::Coord2Id(i, j);
			int grid = grids[id];

			if (id == lastChess)
			{
				printf(format, grid == E_BLACK ? 'B' : 'W');
				continue;
			}

			if (grid == E_EMPTY)
			{
				printf(format, '-');
			}
			if (grid == E_BLACK)
			{
				printf(format, '@');
			}
			if (grid == E_WHITE)
			{
				printf(format, 'O');
			}
		}
		cout << endl;
	}
	cout << endl;
}

void Board::PrintScore(int side, bool isLog)
{
	int i0 = (side == Board::E_BLACK) ? 0 : 1;

	cout << "  ";
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		printf("    %c", 'A' + i);
	}
	cout << endl << endl;

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		if (i < 9)
			printf("%d ", i + 1);
		else
			printf("%c ", 'a' + i - 9);

		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			int id = Board::Coord2Id(i, j);
			int score = scoreInfo[i0][id];

			if (grids[id] == E_EMPTY)
			{
				if (score != 0)
					printf("%5d", score);
				else
					printf("     ");
			}
			else
			{
				printf("%5d", -grids[id]);
			}
		}
		cout << endl << endl;
	}
}

void Board::PrintPriority(bool isLog)
{
	const char *format = (isLog ? "%c  " : "%c ");

	cout << (isLog ? "   " : "  ");
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		printf(format, 'A' + i);
	}
	cout << endl;

	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		if (i < 9)
			printf(format, '1' + i);
		else
			printf(format, 'a' + i - 9);

		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			int id = Board::Coord2Id(i, j);
			int grid = grids[id];

			if (grid == E_EMPTY)
			{
				int priority = gridCheckStatus[id];
				if (priority < E_LOWEST)
				{
					printf((isLog ? "%d  " : "%d "), priority);
				}
				else
				{
					printf(format, ' ');
				}
			}
			if (grid == E_BLACK)
			{
				printf(format, '+');
			}
			if (grid == E_WHITE)
			{
				printf(format, '-');
			}
		}
		cout << endl;
	}
	cout << endl;
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

bool Board::IsWin(int id)
{
	int side = grids[id];
	int i0 = (side == E_BLACK) ? 0 : 1;
	int score0 = scoreInfo[i0][id];

	if (score0 >= FIVE_SCORE)
	{
		if (Board::RestrictedMoveRule && side == E_BLACK && IsRestrictedMove(score0))
			return false;

		return true;
	}

	return false;
}

bool Board::IsLose(int id)
{
	if (Board::RestrictedMoveRule && grids[id] == E_BLACK)
	{
		int score0 = scoreInfo[0][id];
		if (IsRestrictedMove(score0))
			return true;
	}
	return false;
}

int Board::CalcBoardScore(int side)
{
	int i0 = (side == E_BLACK) ? 0 : 1;

	int boardScore = 0;
	for (int i = 0; i < GRID_NUM; ++i)
	{
		if (grids[i] == E_EMPTY)
		{
			if (RestrictedMoveRule && side == E_BLACK)
			{
				if (Board::IsRestrictedMove(scoreInfo[i0][i]))
					continue;
			}
			boardScore += scoreInfo[i0][i];
		}
	}
	return boardScore;
}

void Board::InitLineScoreDict()
{
	FILE *fp;

	if (OUTPUT_RESTRICTED_SCORE)
	{
		fopen_s(&fp, "restricted_score.log", "w");
		for (int i = 0; i < 10000; ++i)
		{
			if (Board::IsRestrictedMove(i))
			{
				fprintf(fp, "%d\n", i);
			}
		}
		fclose(fp);
	}

	char strmap[4] = { ' ', '@', 'O', 'X' };

	if (OUTPUT_LINE_SCORE_DICT)
		fopen_s(&fp, "line_dict.log", "w");

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

			if (OUTPUT_LINE_SCORE_DICT && score > 0)
			{
				fprintf(fp, "%8d, %5d,   %s\n", i, score, lineStr);
			}
		}
	}

	if (OUTPUT_LINE_SCORE_DICT)
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
			score1 = FIVE_SCORE;

			if (Board::RestrictedMoveRule && side == E_BLACK && total1 > WIN_COUNT)
				return RESTRICTED_SCORE;
		}
		else if (total1 == WIN_COUNT - 1)
		{
			if (isOpen1 && isOpen2 && isTotalOpen) // open 4
			{
				score1 = OPEN_FOUR_SCORE;
			}
			else // half-open 4
			{
				score1 = CLOSE_FOUR_SCORE;
			}
		}
		else if (total1 == WIN_COUNT - 2)
		{
			if (isOpen1 && isOpen2 && isTotalOpen) // open 3
			{
				score1 = OPEN_THREE_SCORE;
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
				score2 = OPEN_FOUR_SCORE;

				if (Board::RestrictedMoveRule && side == E_BLACK)
					return RESTRICTED_SCORE;
			}
			else // jump 4
			{
				score2 = CLOSE_FOUR_SCORE;
			}
		}
		else if (total5 == WIN_COUNT - 1)
		{
			if (total3 >= WIN_COUNT - 2 && isOpen1 && isOpen4 && isTotalOpen)
			{
				score2 = OPEN_THREE_SCORE; // jump 3
			}
			else if (total4 >= WIN_COUNT - 2 && isOpen2 && isOpen3 && isTotalOpen)
			{
				score2 = OPEN_THREE_SCORE; // jump 3
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


void Board::UpdatScoreInfo(int id, int turn)
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

void Board::FindOtherGrids(int i0, int id, GridType type)
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

				if (newScore < THREE_THREE_SCORE)
				{
					int id1 = Board::Coord2Id(row1, col1);
					gridCheckStatus[id1] = min(gridCheckStatus[id1], type);
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

				if (newScore < THREE_THREE_SCORE)
				{
					int id1 = Board::Coord2Id(row1, col1);
					gridCheckStatus[id1] = min(gridCheckStatus[id1], type);
				}
			}
		}
	}
}

__declspec(noinline)
void Board::UpdateGridsInfo(int i0)
{
	int i1 = 1 - i0;

	gridCheckStatus.fill(E_GRID_TYPE_MAX);
	hasGridType.fill(false);

	for (int i = 0; i < GRID_NUM; ++i)
	{
		if (grids[i] != E_EMPTY)
			continue;

		int score0 = scoreInfo[i0][i];
		int score1 = scoreInfo[i1][i];

		if (Board::RestrictedMoveRule)
		{
			if (i1 == 0 && score1 >= THREE_THREE_SCORE && IsRestrictedMove(score1))
			{
				score1 = 0;

				if (score0 < THREE_THREE_SCORE) // leave opponent's restricted move untouched if not neccessary
				{
					gridCheckStatus[i] = E_OTHER;
					continue;
				}
			}

			if (i0 == 0 && score0 >= THREE_THREE_SCORE && IsRestrictedMove(score0))
			{
				gridCheckStatus[i] = E_RESTRICTED;
				continue;
			}
		}

		if (score0 >= THREE_THREE_SCORE || score1 >= THREE_THREE_SCORE)
		{
			if (score0 >= FIVE_SCORE)
			{
				gridCheckStatus[i] = E_FIVE;
				hasGridType[E_FIVE] = true;
			}
			else if (score1 >= FIVE_SCORE)
			{
				gridCheckStatus[i] = E_COUNTER_FIVE;
				hasGridType[E_COUNTER_FIVE] = true;
			}
			else if (score0 >= OPEN_FOUR_SCORE)
			{
				gridCheckStatus[i] = E_OPEN_FOUR;
				hasGridType[E_OPEN_FOUR] = true;
			}
			else if (score0 >= FOUR_THREE_SCORE)
			{
				gridCheckStatus[i] = E_FOUR_THREE;
				hasGridType[E_FOUR_THREE] = true;
			}
			else if (score0 >= CLOSE_FOUR_SCORE)
			{
				gridCheckStatus[i] = E_CLOSE_FOUR;
				hasGridType[E_CLOSE_FOUR] = true;
			}
			else if (score1 >= OPEN_FOUR_SCORE)
			{
				gridCheckStatus[i] = E_COUNTER_OPEN_FOUR;
				hasGridType[E_COUNTER_OPEN_FOUR] = true;

				FindOtherGrids(i1, i, E_COUNTER_OPEN_FOUR); // find other possible counter moves
			}
			else if (score1 >= FOUR_THREE_SCORE)
			{
				gridCheckStatus[i] = E_COUNTER_FOUR_THREE;
				hasGridType[E_COUNTER_FOUR_THREE] = true;

				FindOtherGrids(i1, i, E_COUNTER_FOUR_THREE); // find other possible counter moves
			}
			else if (score0 >= THREE_THREE_SCORE)
			{
				gridCheckStatus[i] = E_THREE_THREE;
				hasGridType[E_THREE_THREE] = true;
			}
			else //if (score1 >= THREE_THREE_SCORE)
			{
				gridCheckStatus[i] = E_COUNTER_THREE_THREE;
				hasGridType[E_COUNTER_THREE_THREE] = true;

				FindOtherGrids(i1, i, E_COUNTER_THREE_THREE); // find other possible counter moves
			}
		}
		else
		{
			if (score0 >= TWO_TWO_SCORE || score1 >= TWO_TWO_SCORE)
			{
				if (score0 >= OPEN_THREE_SCORE)
				{
					gridCheckStatus[i] = min(gridCheckStatus[i], E_OPEN_THREE);
				}
				else if (score1 >= OPEN_THREE_SCORE)
				{
					gridCheckStatus[i] = min(gridCheckStatus[i], E_COUNTER_OPEN_THREE);
				}
				else //if (score0 >= TWO_TWO_SCORE || score1 >= TWO_TWO_SCORE)
				{
					gridCheckStatus[i] = min(gridCheckStatus[i], E_TWO_TWO);
				}
			}
			else
			{
				if (score0 > 0)
				{
					gridCheckStatus[i] = min(gridCheckStatus[i], E_OPEN_TWO);
				}
				else
				{
					gridCheckStatus[i] = min(gridCheckStatus[i], E_OTHER);
				}
			}
		}
	}

	int bestType = E_GRID_TYPE_MAX;
	for (int i = 0; i < E_GRID_TYPE_MAX; ++i)
	{
		if (hasGridType[i])
		{
			bestType = i;
			break;
		}
	}

	keyGrid = 0xff;
	hasPriority.fill(false);

	for (int i = 0; i < GRID_NUM; ++i)
	{
		int priority = E_PRIORITY_MAX;

		switch (gridCheckStatus[i])
		{
		case E_FIVE:
		case E_COUNTER_FIVE:
		case E_OPEN_FOUR:
		case E_FOUR_THREE:
			if (bestType == gridCheckStatus[i])
			{
				priority = E_HIGHEST;
				keyGrid = i;
			}
			else
			{
				priority = E_HIGH;
			}
			break;
		case E_CLOSE_FOUR:
			priority = (bestType <= E_COUNTER_THREE_THREE) ? E_HIGH : E_MIDDLE; // try to win before opponent
			break;
		case E_COUNTER_OPEN_FOUR:
		case E_COUNTER_FOUR_THREE:
			priority = E_HIGH;
			break;
		case E_THREE_THREE:
		case E_COUNTER_THREE_THREE:
			priority = (bestType <= E_COUNTER_FOUR_THREE) ? E_MIDDLE : E_HIGH; // counter 4 + 3 first
			break;
		case E_OPEN_THREE:
			priority = (bestType == E_COUNTER_THREE_THREE) ? E_HIGH : E_MIDDLE; // try to win before opponent
			break;
		case E_COUNTER_OPEN_THREE:
		case E_TWO_TWO:
			priority = E_LOW;
			break;
		case E_OPEN_TWO:
			priority = E_LOW;
			break;
		case E_OTHER:
		case E_RESTRICTED:
			priority = E_LOWEST;
			break;
		}

		gridCheckStatus[i] = priority;
		hasPriority[priority] = true;
	}
}

__declspec(noinline)
bool Board::IsRestrictedMove(int score)
{
	if (score >= RESTRICTED_SCORE)
	{
		if (score % RESTRICTED_SCORE >= FIVE_SCORE)
			return false; // five has higher priority than restricted move
		else
			return true;
	}

	if (score >= FIVE_SCORE) // five has higher priority than restricted move
		return false;

	if (score >= THREE_THREE_SCORE)
	{
		if (score >= CLOSE_FOUR_SCORE + OPEN_THREE_SCORE && score < CLOSE_FOUR_SCORE * 2) // 4 + 3
			return false;

		if (score >= OPEN_FOUR_SCORE && score < OPEN_FOUR_SCORE + CLOSE_FOUR_SCORE) // 4 or 4 + 3
			return false;

		return true;
	}

	return false;
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

int Board::CalcDistance(int id1, int id2)
{
	int row1, col1, row2, col2;
	Board::Id2Coord(id1, row1, col1);
	Board::Id2Coord(id2, row2, col2);
	
	int dx = abs(col1 - col2);
	int dy = abs(row1 - row2);
	return max(dx, dy);
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
	board.UpdatScoreInfo(id, turn);

	lastMove = id;

	UpdateValidGrids();
	++turn;

	if (board.IsWin(lastMove))
		state = (side == Board::E_BLACK) ? E_BLACK_WIN : E_WHITE_WIN;

	if (side == Board::E_BLACK && board.IsLose(lastMove)) // lose due to restricted move
		state = E_WHITE_WIN;

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
	
	for (int i = Board::E_HIGH; i < Board::E_PRIORITY_MAX; ++i)
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
	if (board.keyGrid == 0xff)
	{
		if (!board.hasPriority[Board::E_HIGH] && board.hasPriority[Board::E_MIDDLE] && board.hasPriority[Board::E_LOW])
		{
			board.GetGridsByPriority(Board::E_LOW, validGrids, validGridCount);
			return true;
		}
	}
	return false;
}

int GameBase::GetSide()
{
	return (turn % 2 == 1) ? Board::E_BLACK : Board::E_WHITE;
}

int GameBase::GetNextMove()
{
	int id = rand() % validGridCount;
	return validGrids[id];
}

int GameBase::CalcBetterSide()
{
	int otherSide = 3 - GetSide();
	int score0 = board.CalcBoardScore(GetSide());
	int score1 = board.CalcBoardScore(otherSide);

	return (score0 > score1) ? GetSide() : otherSide;
}
///////////////////////////////////////////////////////////////////

Game::Game()
{
	// clear log file
	fopen_s(&fp, GAME_LOG_FILE, "w");
	fclose(fp);
}

bool Game::PutChess(int Id)
{
	if (GameBase::PutChess(Id))
	{
		record.push_back(lastMove);
		OutputLog();
		return true;
	}
	return false;
}

void Game::Regret(int step)
{
	while (!record.empty() && --step >= 0)
	{
		record.pop_back();
	}
	RebuildBoard();
}

void Game::Reset()
{
	GameBase::Init();
	record.clear();
}

void Game::RebuildBoard()
{
	GameBase::Init();

	for (int i = 0; i < record.size(); ++i)
	{
		GameBase::PutChess(record[i]);
	}
}

void Game::Print()
{
	string sideText[] = { "Black", "White" };
	string stateText[] = { "Normal", "Black Win", "White Win", "Draw" };

	cout << endl;
	if (state == E_WHITE_WIN && GetSide() == Board::E_WHITE)
		printf("Black lose for restricted move!\n");

	printf("=== Turn %03d | %s's turn ===\n", GetTurn(), sideText[GetSide() - 1].c_str());
	printf("===  Current State: %s  ===\n", stateText[state].c_str());
	board.Print(lastMove);
}

void Game::OutputLog()
{
	freopen_s(&fp, GAME_LOG_FILE, "a+", stdout);
	board.Print(lastMove, true);
	board.PrintScore(3 - GetSide(), true);
	board.PrintScore(GetSide(), true);
	board.PrintPriority(true);
	fclose(stdout);
	freopen_s(&fp, "CON", "w", stdout);
}

int Game::Str2Id(const string &str)
{
	int col = str[0] - 'A';
	int row = str[1] <= '9' ? str[1] - '1' : str[1] - 'a' + 9;
	if (!Board::IsValidCoord(row, col))
		return -1;

	int id = Board::Coord2Id(row, col);
	return id;
}

string Game::Id2Str(int id)
{
	int row, col;
	Board::Id2Coord(id, row, col);
	string result(1, col + 'A');
	result += (row < 9) ? row + '1' : row + 'a' - 9;
	return result;
}
