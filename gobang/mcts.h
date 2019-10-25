#pragma once
#include <list>
#include "game.h"

class TreeNode
{
public:
	TreeNode(TreeNode *p);

	int visit;
	float value;
	float score;
	Game *game;

	TreeNode *parent;
	list<TreeNode*> children;
	vector<Int2> emptyGrids;
};

class MCTS
{
public:
	MCTS();
	~MCTS();
	Int2 Search(Game *state);

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
	bool ReuseOldTree(Game *state);

	void ClearNodes(TreeNode *node);
	float CalcScore(const TreeNode *node, float c);
	void PrintTree(TreeNode *node, int level = 0);

	TreeNode* NewTreeNode(TreeNode *parent);
	void RecycleTreeNode(TreeNode *node);
	void ClearPool();
	
	list<TreeNode*> pool;
	TreeNode *root;
};
