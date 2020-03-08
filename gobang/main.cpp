#include "game.h"
#include "mcts.h"
#include "ctime"

void TurnHuman(Game &g, bool useAI)
{
	string input;
	int move;

	while (1)
	{
		cout << "Enter your move: ";
		cin >> input;

		int regretStep = useAI ? 2 : 1;
		if (input == "undo" && g.GetTurn() > regretStep)
		{
			g.Regret(regretStep);
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
}

void TurnAI(MCTS &ai, Game &g)
{
	cout << "AI is thinking..." << endl;
	int aiMove = ai.Search(&g);
	cout << "AI's move: " << Game::Id2Str(aiMove) << endl;
	g.PutChess(aiMove);
}

int main()
{
	srand((unsigned)time(NULL));

	MCTS ai;
	Game g;
	
	bool useAI = false;
	bool AIFirst = false;
	bool AISecond = false;

	cout << "Available game mode:" << endl;
	cout << "1: AI first" << endl;
	cout << "2: human first" << endl;
	cout << "3: no AI" << endl;
	cout << "4: no human" << endl;

	int mode = 0;
	while (1)
	{
		cout << "select mode:";
		cin >> mode;

		if (mode >= 1 && mode <= 4)
			break;

		cout << "Invalid mode!" << endl << endl;
	}

	if (mode == 1)
	{
		useAI = true;
		AIFirst = true;
	}
	else if (mode == 2)
	{
		useAI = true;
		AISecond = true;
	}
	else if (mode == 4)
	{
		useAI = true;
		AIFirst = true;
		AISecond = true;
	}

	g.Print();
	while (g.GetState() == GameBase::E_NORMAL)
	{
		if (useAI && AIFirst)
		{
			TurnAI(ai, g);
		}
		else
		{
			TurnHuman(g, useAI);
		}

		g.Print();
		if (g.GetState() != GameBase::E_NORMAL)
		{
			break;
		}

		if (useAI && AISecond)
		{
			TurnAI(ai, g);
		}
		else
		{
			TurnHuman(g, useAI);
		}
	
		g.Print();
	}

	return 0;
}