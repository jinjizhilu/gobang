#include <fstream>
#include <thread>
#include <mutex>
#include <cmath>
#include <cstdlib>
#include "mcts.h"

const char* LOG_FILE = "MCTS.log";
const char* LOG_FILE_FULL = "MCTS_FULL.log";
const float Cp = 2.0f;
const float SEARCH_TIME = 2.0f;
const int	EXPAND_THRESHOLD = 3;
const bool	ENABLE_MULTI_THREAD = true;
const int	FAST_STOP_STEP = 30;
const int	TRY_MORE_NODE_THRESHOLD = 1000;

TreeNode::TreeNode(TreeNode *p)
{
	visit = 0;
	value = 0;
	winRate = 0;
	expandFactor = 0;
	validGridCount = 0;
	gridLevel = 0;
	game = NULL;
	parent = p;
}

FILE *fp;

MCTS::MCTS()
{
	root = NULL;

	// clear log file
	fopen_s(&fp, LOG_FILE, "w");
	fclose(fp);
}

MCTS::~MCTS()
{
	ClearPool();
}

mutex mtx;

void MCTS::SearchThread(int id, MCTS *mcts, clock_t startTime)
{
	float elapsedTime = 0;

	while (1)
	{
		mtx.lock();
		TreeNode *node = mcts->TreePolicy(mcts->root);
		mtx.unlock();

		float value = mcts->DefaultPolicy(node, id);

		mtx.lock();
		mcts->UpdateValue(node, value);
		mtx.unlock();

		elapsedTime = float(clock() - startTime) / 1000;
		if (elapsedTime > SEARCH_TIME)
		{
			mtx.lock();
			TreeNode *mostVisit = *max_element(mcts->root->children.begin(), mcts->root->children.end(), [](const TreeNode *a, const TreeNode *b)
			{
				return a->visit < b->visit;
			});

			TreeNode *bestScore = mcts->BestChild(mcts->root, 0);
			mtx.unlock();

			if (mostVisit == bestScore)
				break;
		}
	}
}

int MCTS::Search(Game *state)
{
	root = NewTreeNode(NULL);
	*(root->game) = *((GameBase*)state);
	root->validGridCount = root->game->validGridCount;
	root->validGrids = root->game->validGrids;

	clock_t startTime = clock();

	thread threads[THREAD_NUM_MAX];
	int thread_num = ENABLE_MULTI_THREAD ? thread::hardware_concurrency() : 1;

	for (int i = 0; i < thread_num; ++i)
		threads[i] = thread(SearchThread, i, this, startTime);

	for (int i = 0; i < thread_num; ++i)
		threads[i].join();
	
	TreeNode *best = BestChild(root, 0);
	int move = best->game->lastMove;

	maxDepth = 0;
	PrintTree(root);
	PrintFullTree(root);
	printf("time: %.2f, iteration: %d, depth: %d, win: %d/%d\n", float(clock() - startTime) / 1000, root->visit, maxDepth, (int)best->value, best->visit);

	ClearNodes(root);

	return move;
}

TreeNode* MCTS::TreePolicy(TreeNode *node)
{
	while (node->game->state == GameBase::E_NORMAL)
	{
		if (node->visit < EXPAND_THRESHOLD)
			return node;

		if (PreExpandTree(node))
			return ExpandTree(node);
		else
			node = BestChild(node, Cp);
	}
	return node;
}

bool MCTS::PreExpandTree(TreeNode *node)
{
	if (node->validGridCount > 0)
	{
		int id = rand() % node->validGridCount;
		swap(node->validGrids[id], node->validGrids[node->validGridCount - 1]);
	}
	else
	{
		// try grids with lower priority after certain visits
		if (node->gridLevel == 0 && node->visit > TRY_MORE_NODE_THRESHOLD)
		{
			if (node->game->UpdateValidGridsExtra())
			{
				node->gridLevel++;
				node->validGrids = node->game->validGrids;
				node->validGridCount = node->game->validGridCount;

				int id = rand() % node->validGridCount;
				swap(node->validGrids[id], node->validGrids[node->validGridCount - 1]);
			}
		}
	}
	return node->validGridCount > 0;
}

TreeNode* MCTS::ExpandTree(TreeNode *node)
{
	int move = node->validGrids[node->validGridCount - 1];
	--(node->validGridCount);

	TreeNode *newNode = NewTreeNode(node);
	node->children.push_back(newNode);
	*(newNode->game) = *(node->game);
	newNode->game->PutChess(move);
	newNode->validGridCount = newNode->game->validGridCount;
	newNode->validGrids = newNode->game->validGrids;

	return newNode;
}

TreeNode* MCTS::BestChild(TreeNode *node, float c)
{
	TreeNode *result = NULL;
	float bestScore = -1;
	float expandFactorParent_c = sqrtf(logf(node->visit)) * c;

	for (auto child : node->children)
	{
		float score = CalcScoreFast(child, expandFactorParent_c);
		if (score > bestScore)
		{
			bestScore = score;
			result = child;
		}
	}
	return result;
}

float MCTS::CalcScore(const TreeNode *node, float c, float logParentVisit)
{
	float winRate = node->value / node->visit;
	float expandFactor = c * sqrtf(logParentVisit / node->visit);

	if (node->game->GetSide() == root->game->GetSide()) // win rate of opponent
		winRate = 1 - winRate;

	return winRate + expandFactor;
}

float MCTS::CalcScoreFast(const TreeNode *node, float expandFactorParent_c)
{
	return node->winRate + node->expandFactor * expandFactorParent_c;
}

float MCTS::DefaultPolicy(TreeNode *node, int id)
{
	gameCache[id] = *(node->game);

	int step = 0;
	while (gameCache[id].state == GameBase::E_NORMAL)
	{
		int move = gameCache[id].GetNextMove();
		gameCache[id].PutChess(move);

		if (++step > FAST_STOP_STEP)
			return 0.5;
	}
	float value = (gameCache[id].state == root->game->GetSide()) ? 1.f : 0;

	return value;
}

void MCTS::UpdateValue(TreeNode *node, float value)
{
	while (node != NULL)
	{
		node->visit++;
		node->value += value;

		node->expandFactor = sqrtf(1.f / node->visit);
		node->winRate = node->value / node->visit;

		if (node->game->GetSide() == root->game->GetSide()) // win rate of opponent
			node->winRate = 1 - node->winRate;

		node = node->parent;
	}
}

void MCTS::ClearNodes(TreeNode *node)
{
	if (node != NULL)
	{
		for (auto child : node->children)
		{
			ClearNodes(child);
		}

		RecycleTreeNode(node);
	}
}

TreeNode* MCTS::NewTreeNode(TreeNode *parent)
{
	if (pool.empty())
	{
		TreeNode *node = new TreeNode(parent);
		node->game = new GameBase();
		return node;
	}

	TreeNode *node = pool.back();
	node->parent = parent;
	pool.pop_back();

	return node;
}

void MCTS::RecycleTreeNode(TreeNode *node)
{
	node->parent = NULL;
	node->visit = 0;
	node->value = 0;
	node->winRate = 0;
	node->expandFactor = 0;
	node->validGridCount = 0;
	node->gridLevel = 0;
	node->children.clear();

	pool.push_back(node);
}

void MCTS::ClearPool()
{
	for (auto node : pool)
	{
		delete node->game;
		delete node;
	}
}

void MCTS::PrintTree(TreeNode *node, int level)
{
	if (level == 0)
	{
		fopen_s(&fp, LOG_FILE, "a+");
		fprintf(fp, "===============================PrintTree=============================\n");
	}
	
	if (level > maxDepth)
		maxDepth = level;

	node->children.sort([](const TreeNode *a, const TreeNode *b)
	{
		return a->visit > b->visit;
	});

	int i = 1;
	for (auto it = node->children.begin(); it != node->children.end(); ++it)
	{
		if ((float)(*it)->visit / root->visit > 0.000)
		{
			fprintf(fp, "%d", level);
			for (int j = 0; j < level; ++j)
				fprintf(fp, "   ");

			float expandFactorParent_c = sqrtf(logf(node->visit)) * Cp;
			fprintf(fp, "visit: %d, value: %.1f, score: %.4f, children: %d, move: %s\n", (*it)->visit, (*it)->value, CalcScoreFast(*it, expandFactorParent_c), (*it)->children.size(), Game::Id2Str((*it)->game->lastMove).c_str());
			PrintTree(*it, level + 1);
		}

		if (++i > 3)
			break;
	}

	if (level == 0)
	{
		fprintf(fp,"================================TreeEnd============================\n\n");
		fclose(fp);
	}
}

void MCTS::PrintFullTree(TreeNode *node, int level)
{
	if (level == 0)
	{
		fopen_s(&fp, LOG_FILE_FULL, "w");
		fprintf(fp, "===============================PrintFullTree=============================\n");
	}

	node->children.sort([](const TreeNode *a, const TreeNode *b)
	{
		return a->visit > b->visit;
	});

	int i = 1;
	for (auto it = node->children.begin(); it != node->children.end(); ++it)
	{
		fprintf(fp, "%d", level);
		for (int j = 0; j < level; ++j)
			fprintf(fp, "   ");

		float expandFactorParent_c = sqrtf(logf(node->visit)) * Cp;
		fprintf(fp, "visit: %d, value: %.1f, score: %.4f, children: %d, move: %s\n", (*it)->visit, (*it)->value, CalcScoreFast(*it, expandFactorParent_c), (*it)->children.size(), Game::Id2Str((*it)->game->lastMove).c_str());
		PrintFullTree(*it, level + 1);
	}

	if (level == 0)
	{
		fprintf(fp, "================================TreeEnd============================\n\n");
		fclose(fp);
	}
}
