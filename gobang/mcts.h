#pragma once
#include <list>
#include "game.h"

class TreeNode
{
public:
	TreeNode(TreeNode *p);
	void Clear();

	int visit;
	float value;
	float score;
	float winRate;
	float expandFactor;
	int emptyGridCount;
	GameBase *game;

	TreeNode *parent;
	list<TreeNode*> children;
	array<uint8_t, GRID_NUM> emptyGrids;
};

class MCTS
{
public:
	MCTS();
	~MCTS();
	int Search(Game *state);

private:
	// standard MCTS process
	TreeNode* TreePolicy(TreeNode *node);
	TreeNode* ExpandTree(TreeNode *node);
	TreeNode* BestChild(TreeNode *node, float c);
	float DefaultPolicy(TreeNode *node);
	void UpdateValue(TreeNode *node, float value);

	// custom optimization
	bool PreExpandTree(TreeNode *node);
	bool PruneTree(TreeNode *node);
	TreeNode* ReuseOldTree(Game *state);

	void ClearNodes(TreeNode *node);
	float CalcScore(const TreeNode *node, float c, float logParentVisit);
	float CalcScoreFast(const TreeNode *node, float c, float expandFactorParent);
	void PrintTree(TreeNode *node, int level = 0);

	TreeNode* NewTreeNode(TreeNode *parent);
	void RecycleTreeNode(TreeNode *node);
	void ClearPool();
	
	int maxDepth;
	GameBase gameCache;
	list<TreeNode*> pool;
	TreeNode *root;
};
