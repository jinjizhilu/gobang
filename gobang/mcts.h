#pragma once
#include <list>
#include <ctime>
#include "game.h"

const int THREAD_NUM_MAX = 32;

class TreeNode
{
public:
	TreeNode(TreeNode *p);
	void Clear();

	int visit;
	float value;
	float winRate;
	float expandFactor;
	int validGridCount;
	int gridLevel;
	GameBase *game;

	TreeNode *parent;
	list<TreeNode*> children;
	array<uint8_t, GRID_NUM> validGrids;
};

class MCTS
{
public:
	MCTS();
	~MCTS();
	int Search(Game *state);

private:
	static void SearchThread(int id, MCTS *mcts, clock_t startTime);

	// standard MCTS process
	TreeNode* TreePolicy(TreeNode *node);
	TreeNode* ExpandTree(TreeNode *node);
	TreeNode* BestChild(TreeNode *node, float c);
	float DefaultPolicy(TreeNode *node, int id);
	void UpdateValue(TreeNode *node, float value);

	// custom optimization
	bool PreExpandTree(TreeNode *node);

	void ClearNodes(TreeNode *node);
	float CalcScore(const TreeNode *node, float c, float logParentVisit);
	float CalcScoreFast(const TreeNode *node, float expandFactorParent_c);
	void PrintTree(TreeNode *node, int level = 0);
	void PrintFullTree(TreeNode *node, int level = 0);

	TreeNode* NewTreeNode(TreeNode *parent);
	void RecycleTreeNode(TreeNode *node);
	void ClearPool();
	
	int maxDepth;
	GameBase gameCache[THREAD_NUM_MAX];
	list<TreeNode*> pool;
	TreeNode *root;
};
