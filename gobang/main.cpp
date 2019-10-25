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

		while (1)
		{
			cout << "Enter your move: ";
			cin >> input;

			if (input == "undo" && g.GetTurn() > 2)
			{
				g.Regret(2);
				g.Print();
				continue;
			}

			move = Game::Str2Coord(input);
			if (g.PutChess(move.x, move.y))
				break;

			cout << "!Invalid move!" << endl;
		}

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