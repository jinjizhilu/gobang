#include "game.h"
#include "mcts.h"

void main()
{
	Game g;
	
	for (int i = 0; i < 100; ++i)
	{
		if (!g.PutRandomChess())
			break;
	}
	g.Print();
}