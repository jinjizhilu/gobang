#include "game.h"
#include "mcts.h"
#include "ctime"

int main()
{
	srand((unsigned)time(NULL));

	MCTS ai;
	Game g;
	string input;
	int move;
	
	bool useAI = true;
	bool AIFrist = rand() % 2;

	while (g.GetState() == GameBase::E_NORMAL)
	{
		g.Print();

		if (g.GetTurn() > 1 || !AIFrist)
		{
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

				move = Game::Str2Id(input);
				if (move != -1)
				{
					if (g.PutChess(move))
						break;
				}

				cout << "Invalid move! (format: H8 | undo)" << endl;
			}

			g.Print();
			if (g.GetState() != GameBase::E_NORMAL)
				break;
		}

		if (useAI)
		{
			cout << "AI is thinking..." << endl;
			int aiMove = ai.Search(&g);
			cout << "AI's move: " << Game::Id2Str(aiMove) << endl;
			g.PutChess(aiMove);
		}
	}
	g.Print();

	return 0;
}