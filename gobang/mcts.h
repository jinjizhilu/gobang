#pragma once
#include <list>
#include "game.h"

class TreeNode
{
public:
	TreeNode(TreeNode *p);

	int visit;
	float value;
	Game *game;

	TreeNode *parent;
	list<TreeNode*> children;
	vector<Int2> emptyGrids;
};

class MCTS
{
public:
	Int2 Search(Game *state);

private:
	TreeNode* TreePolicy(TreeNode *node);
	TreeNode* ExpandTree(TreeNode *node);
	TreeNode* BestChild(TreeNode *node, float c);
	float DefaultPolicy(TreeNode *node);
	void UpdateValue(TreeNode *node, float value);
	void ClearNodes(TreeNode *node);
	float CalcScore(const TreeNode *node, float c);
	void PrintTree(TreeNode *node, int level = 0);
	
	TreeNode *root;
};
