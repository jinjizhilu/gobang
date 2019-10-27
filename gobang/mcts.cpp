#include <fstream>
#include "mcts.h"
#include "cmath"
#include "ctime"
#include "cstdlib"

const char* LOG_FILE = "MCTS.log";
const float Cp = 1 / sqrtf(1);
const float TRY_TIME = 1.0f;
const int FAST_STOP_STEP = 30;
const bool USE_OLD_TREE = true;

TreeNode::TreeNode(TreeNode *p)
{
	visit = 0;
	value = 0;
	score = 0;
	winRate = 0;
	expandFactor = 0;
	emptyGridCount = 0;
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

int MCTS::Search(Game *state)
{
	if (root == NULL || !(USE_OLD_TREE && ReuseOldTree(state)))
	{
		root = NewTreeNode(NULL);
		*(root->game) = *((GameBase*)state);
		root->emptyGridCount = root->game->emptyGridCount;
		root->emptyGrids = root->game->emptyGrids;
	}

	clock_t startTime = clock();
	int counter = 0;

	while (++counter > 0)
	{
		TreeNode *node = TreePolicy(root);
		float value = DefaultPolicy(node);
		UpdateValue(node, value);
		PruneTree(node);

		float elapedTime = float(clock() - startTime) / 1000;
		if (elapedTime > TRY_TIME)
			break;
	}
	
	TreeNode *best = BestChild(root, 0);
	int move = best->game->lastMove;

	maxDepth = 0;
	PrintTree(root);
	printf("time: %.2f, iteration: %d, depth: %d, win: %d/%d\n", float(clock() - startTime) / 1000, counter, maxDepth, (int)best->value, best->visit);

	if (best->game->state != GameBase::E_NORMAL)
	{
		ClearNodes(root);
	}

	return move;
}

TreeNode* MCTS::TreePolicy(TreeNode *node)
{
	while (node->game->state == GameBase::E_NORMAL)
	{
		if (PreExpandTree(node))
			return ExpandTree(node);
		else
			node = BestChild(node, Cp);
	}
	return node;
}

bool MCTS::PreExpandTree(TreeNode *node)
{
	bool skipLonelyGrid = true;
	if (skipLonelyGrid)
	{
		int radius = (node->game->turn < 20) ? 1 : 2;

		while (node->emptyGridCount > 0)
		{
			int id = rand() % node->emptyGridCount;
			swap(node->emptyGrids[id], node->emptyGrids[node->emptyGridCount - 1]);
			int move = node->emptyGrids[node->emptyGridCount - 1];

			if (!node->game->IsLonelyGrid(move, radius))
				break;

			--(node->emptyGridCount);
		}
	}
	else
	{
		int id = rand() % node->emptyGridCount;
		swap(node->emptyGrids[id], node->emptyGrids[node->emptyGridCount - 1]);
	}
	return node->emptyGridCount > 0;
}

TreeNode* MCTS::ExpandTree(TreeNode *node)
{
	int move = node->emptyGrids[node->emptyGridCount - 1];
	--(node->emptyGridCount);

	TreeNode *newNode = NewTreeNode(node);
	node->children.push_back(newNode);
	*(newNode->game) = *(node->game);
	newNode->game->PutChess(move);
	newNode->emptyGridCount = newNode->game->emptyGridCount;
	newNode->emptyGrids = newNode->game->emptyGrids;

	return newNode;
}

bool MCTS::PruneTree(TreeNode *node)
{
	// if game finishes on this node, just discard its parent node which should not be chosen
	if (node->game->state != GameBase::E_NORMAL)
	{
		TreeNode *parentNode = node->parent;
		if (parentNode != root)
		{
			// do not delete last node when failure is inevitable
			if (parentNode->parent->children.size() > 1)
			{
				parentNode->parent->children.remove(parentNode);
				ClearNodes(parentNode);
				return true;
			}
		}
	}
	return false;
}

TreeNode* MCTS::BestChild(TreeNode *node, float c)
{
	TreeNode *result = NULL;
	float bestScore = -1;
	float expandFactorParent = sqrtf(logf(node->visit));
	for (auto child : node->children)
	{
		float score = CalcScoreFast(child, c, expandFactorParent);
		child->score = score;
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
	float winRate = (float)node->value / node->visit;
	float expandFactor = c * sqrtf(logParentVisit / node->visit);

	if (node->game->GetSide() == root->game->GetSide()) // win rate of opponent
		winRate = 1 - winRate;

	return winRate + expandFactor;
}

float MCTS::CalcScoreFast(const TreeNode *node, float c, float expandFactorParent)
{
	return node->winRate + node->expandFactor * expandFactorParent * c;
}

float MCTS::DefaultPolicy(TreeNode *node)
{
	int step = 0;
	gameCache = *(node->game);
	while (gameCache.state == GameBase::E_NORMAL && ++step < FAST_STOP_STEP)
	{
		gameCache.PutRandomChess();
	}

	float value = (gameCache.state == root->game->GetSide()) ? 1.f : 0;

	// just return random result, since random walk result too far away is not accurate at all
	if (step >= FAST_STOP_STEP)
	{
		value = rand() % 2;
	}

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

bool MCTS::ReuseOldTree(Game *state)
{
	auto &record = state->GetRecord();
	int lastSelfMove = record[record.size() - 3];
	int lastOpponentMove = record[record.size() - 2];

	TreeNode *foundNode = NULL;
	for (auto node : root->children)
	{
		if (node->game->lastMove == lastSelfMove)
		{
			for (auto child : node->children)
			{
				if (child->game->lastMove == lastOpponentMove)
				{
					foundNode = child;
					foundNode->parent = NULL;
					node->children.remove(foundNode);
					break;
				}
			}
			break;
		}
	}
	ClearNodes(root);
	root = foundNode;

	return (root != NULL);
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
	node->score = 0;
	node->winRate = 0;
	node->expandFactor = 0;
	node->emptyGridCount = 0;
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

	node->children.sort([this](const TreeNode *a, const TreeNode *b)
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

			float expandFactorParent = sqrtf(logf(node->visit));
			fprintf(fp, "visit: %d, value: %.0f, score: %.4f, children: %d\n", (*it)->visit, (*it)->value, CalcScoreFast(*it, Cp, expandFactorParent), (*it)->children.size());
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
