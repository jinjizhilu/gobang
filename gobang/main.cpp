#include "game.h"
#include "mcts.h"

void main()
{
	Game g;
	string move;
	
	while (g.GetState() == Game::E_NORMAL)
	{
		cout << endl;
		g.Print();

		cout << "Enter your move: ";
		cin >> move;

		g.PutChess(move);
		g.PutRandomChess();
	}
	g.Print();
}