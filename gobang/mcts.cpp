#include <fstream>
#include "mcts.h"
#include "cmath"
#include "ctime"

const float Cp = 1 / sqrtf(2);
const int TRY_COUNT = 100000;
const float TRY_TIME = 1.0f;

TreeNode::TreeNode(TreeNode *p)
{
	visit = 0;
	value = 0;
	game = NULL;
	parent = p;
}

Int2 MCTS::Search(Game *state)
{
	root = new TreeNode(NULL);
	root->game = state->Clone();
	root->game->SetSimMode(true);
	root->emptyGrids = state->GetEmptyGrids();

	clock_t startTime = clock();
	int counter = 0;

	while (++counter > 0)
	{
		TreeNode *node = TreePolicy(root);
		float value = DefaultPolicy(node);
		UpdateValue(node, value);

		float elapedTime = float(clock() - startTime) / 1000;
		if (elapedTime > TRY_TIME)
			break;
	}
	
	TreeNode *best = BestChild(root, 0);
	Int2 move = best->game->GetLastMove();
	printf("time elapsed: %.2f, iteration count: %d\n", float(clock() - startTime) / 1000, counter);
	//PrintTree(root);

	ClearNodes(root);

	return move;
}

TreeNode* MCTS::TreePolicy(TreeNode *node)
{
	while (node->game->GetState() == Game::E_NORMAL)
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
		int radius = (node->game->GetTurn() < 20) ? 1 : 2;

		while (!node->emptyGrids.empty())
		{
			int id = rand() % node->emptyGrids.size();
			swap(node->emptyGrids[id], node->emptyGrids.back());
			Int2 move = node->emptyGrids.back();

			if (!node->game->IsLonelyGrid(move.x, move.y, radius))
				break;

			node->emptyGrids.pop_back();
		}
	}
	else
	{
		int id = rand() % node->emptyGrids.size();
		swap(node->emptyGrids[id], node->emptyGrids.back());
	}
	return !node->emptyGrids.empty();
}

TreeNode* MCTS::ExpandTree(TreeNode *node)
{
	Int2 move = node->emptyGrids.back();
	node->emptyGrids.pop_back();

	TreeNode *newNode = new TreeNode(node);
	node->children.push_back(newNode);
	newNode->game = node->game->Clone();
	newNode->game->PutChess(move.x, move.y);
	newNode->emptyGrids = newNode->game->GetEmptyGrids();

	return newNode;
}

TreeNode* MCTS::BestChild(TreeNode *node, float c)
{
	TreeNode *result = NULL;
	float bestScore = -1;
	for (auto child : node->children)
	{
		float score = CalcScore(child, c);
		child->score = score;
		if (score > bestScore)
		{
			bestScore = score;
			result = child;
		}
	}
	return result;
}

float MCTS::CalcScore(const TreeNode *node, float c)
{
	return (float)node->value / node->visit + c * sqrtf(logf(node->parent->visit) / node->visit);
}

float MCTS::DefaultPolicy(TreeNode *node)
{
	Game *newGame = node->game->Clone();
	while (newGame->GetState() == Game::E_NORMAL)
	{
		newGame->PutRandomChess();
	}

	float value = (newGame->GetState() == root->game->GetSide()) ? 1.f : 0;
	delete newGame;

	return value;
}

void MCTS::UpdateValue(TreeNode *node, float value)
{
	while (node != NULL)
	{
		node->visit++;
		node->value += value;
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
		delete node->game;
		delete node;
	}
}

void MCTS::PrintTree(TreeNode *node, int level)
{
	node->children.sort([this](const TreeNode *a, const TreeNode *b)
	{
		//return CalcScore(a, 0) > CalcScore(b, 0);
		return a->visit > b->visit;
	});

	int i = 0;
	for (auto it = node->children.begin(); it != node->children.end(); ++it)
	{
		for (int j = 0; j < level; ++j)
			cout << "   ";

		printf("visit: %d, value: %.0f, score: %.4f\n", (*it)->visit, (*it)->value, CalcScore(*it, Cp));
		PrintTree(*it, level + 1);

		if (++i > 5)
			break;
	}
}
