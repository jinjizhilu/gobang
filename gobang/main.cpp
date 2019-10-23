#include "game.h"
#include "mcts.h"

void main()
{
	MCTS ai;
	Game g;
	string input;
	Int2 move;
	
	bool useAI = true;

	while (g.GetState() == Game::E_NORMAL)
	{
		g.Print();

		cout << "Enter your move: ";
		cin >> input;
		
		move = Game::Str2Coord(input);
		g.PutChess(move.x, move.y);
		g.Print();
		if (g.GetState() != Game::E_NORMAL)
			break;
		
		if (useAI)
		{
			cout << "AI is thinking..." << endl;
			Int2 aiMove = ai.Search(&g);
			cout << "AI's move: " << Game::Coord2Str(aiMove) << endl;
			g.PutChess(aiMove.x, aiMove.y);
		}
	}
	g.Print();
}