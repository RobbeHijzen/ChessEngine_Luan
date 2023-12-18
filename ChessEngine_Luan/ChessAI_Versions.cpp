#include "ChessAI_Versions.h"

#define FLOAT_MAX FLT_MAX
#define FLOAT_MIN -FLOAT_MAX

Move ChessAI_V0::GetAIMove()
{
	auto moves{ m_pChessBoard->GetPossibleMoves() };
	auto it{ moves.begin()};

	auto randomIndex{ rand() % moves.size() };
	std::advance(it, randomIndex);

	return *it;
}

Move ChessAI_V1_AlphaBeta::GetAIMove()
{
	int depth{ 3 };

	auto possibleMoves{ m_pChessBoard->GetPossibleMoves() };
	Move currentBestMove{};
	float currentBestValue{FLOAT_MIN};

	float alpha{FLOAT_MIN};
	float beta{FLOAT_MAX};

	for (int index{}; index < possibleMoves.size(); ++index)
	{

		auto it{ possibleMoves.begin() };
		std::advance(it, index);
		Move move{ *it };

		m_pChessBoard->MakeMove(move);

		float moveValue{ DepthSearch(depth - 1, alpha, beta) };
		if (moveValue > currentBestValue) { currentBestMove = move; currentBestValue = moveValue; }

		m_pChessBoard->UnMakeLastMove();
	}

	return currentBestMove;
}
float ChessAI_V1_AlphaBeta::DepthSearch(int depth, float alpha, float beta)
{
	bool isMinimizer{ !bool(depth & 1) };

	if (depth == 0) return BoardValueEvaluation(m_pChessBoard->GetCurrentGameState());

	auto possibleMoves{ m_pChessBoard->GetPossibleMoves() };
	float currentMoveValue{ isMinimizer ? FLOAT_MAX : FLOAT_MIN};

	for (const auto& move : possibleMoves)
	{
		m_pChessBoard->MakeMove(move);
		float moveValue{ DepthSearch(depth - 1, alpha, beta) };
		m_pChessBoard->UnMakeLastMove();

		if (!isMinimizer)
		{
			currentMoveValue = max(currentMoveValue, moveValue);
			if (currentMoveValue > beta) 
				break;
			
			alpha = max(alpha, currentMoveValue);
		}
		else
		{
			currentMoveValue = min(currentMoveValue, moveValue);
			if (currentMoveValue < alpha) 
				break;
			
			beta = min(beta, currentMoveValue);
		}		

	}

	return currentMoveValue;
}
float ChessAI_V1_AlphaBeta::BoardValueEvaluation(GameState gameState)
{
	switch (gameState.gameProgress)
	{
	case GameProgress::InProgress:
	{
		break;
	}
	case GameProgress::Draw:
	{
		return 0;
	}
	case GameProgress::WhiteWon:
	{
		return m_ControllingWhite ? FLOAT_MAX - 100.f : FLOAT_MIN + 100.f;
	}
	case GameProgress::BlackWon:
	{
		return m_ControllingWhite ? FLOAT_MIN + 100.f : FLOAT_MAX - 100.f;
	}
	default:
		break;
	}



	float whiteValue{};
	float blackValue{};

	constexpr float pawnValue{ 1 };
	constexpr float knightValue{ 3 };
	constexpr float bishopValue{ 3 };
	constexpr float rookValue{ 5 };
	constexpr float queenValue{ 9 };

	for (int index{}; index < 64; ++index)
	{
		uint64_t mask{ static_cast<unsigned long long>(1) << index };

		if (mask & gameState.bitBoards.whitePawns) whiteValue += pawnValue;
		else if (mask & gameState.bitBoards.whiteKnights) whiteValue += knightValue;
		else if (mask & gameState.bitBoards.whiteBishops) whiteValue += bishopValue;
		else if (mask & gameState.bitBoards.whiteRooks) whiteValue += rookValue;
		else if (mask & gameState.bitBoards.whiteQueens) whiteValue += queenValue;

		else if (mask & gameState.bitBoards.blackPawns) blackValue += pawnValue;
		else if (mask & gameState.bitBoards.blackKnights) blackValue += knightValue;
		else if (mask & gameState.bitBoards.blackBishops) blackValue += bishopValue;
		else if (mask & gameState.bitBoards.blackRooks) blackValue += rookValue;
		else if (mask & gameState.bitBoards.blackQueens) blackValue += queenValue;
	}

	return m_ControllingWhite ? whiteValue - blackValue : blackValue - whiteValue;
}


Move ChessAI_V1_MCST::GetAIMove()
{
	auto pRoot{ std::make_unique<Node>() };
	// Initializing the children of pRoot to start the algorithm
	{
		pRoot->gameState = m_pChessBoard->GetCurrentGameState();
		for (const auto& move : m_pChessBoard->GetPossibleMoves())
		{
			m_pChessBoard->MakeMove(move);
			pRoot->children.push_back(new Node{ move, pRoot.get(), m_pChessBoard->GetCurrentGameState() });
			m_pChessBoard->UnMakeLastMove();
		}
	}

	// Do the MCTS
	for (int i = 0; i < m_Iterations; ++i) 
	{
		Node* selectedNode = SelectNode(pRoot.get());
		
		// If the selectedNode has been visited already	(the .visits is always 0 or 1 because of the while loop in SelectNode(...) )
		if (selectedNode->visits == 1)
		{
			ExpandNode(selectedNode);
			selectedNode = selectedNode->children.at(rand() % selectedNode->children.size());
		}

		float nodeValue = Rollout(selectedNode);
		Backpropagate(selectedNode, nodeValue);
	}

	// After the iterations, choose the best move based on statistics
	Node* bestChild = nullptr;
	float bestScore = FLOAT_MIN;

	for (Node* child : pRoot->children)
	{
		float childScore = child->totalScore / child->visits;

		if (childScore > bestScore) 
		{
			bestScore = childScore;
			bestChild = child;
		}
	}

	if(bestChild) return bestChild->move;
	return Move{};
}
float ChessAI_V1_MCST::BoardValueEvaluation(GameState gameState)
{
	switch (gameState.gameProgress)
	{
		case GameProgress::InProgress:
		{
			break;
		}
		case GameProgress::Draw:
		{
			return 0;
		}
		case GameProgress::WhiteWon:
		{
			return m_ControllingWhite ? 50.f : -50.f;
		}
		case GameProgress::BlackWon:
		{
			return m_ControllingWhite ? -50.f : 50.f;
		}
	default:
		break;
	}



	float whiteValue{};
	float blackValue{};

	constexpr float pawnValue{ 1 };
	constexpr float knightValue{ 3 };
	constexpr float bishopValue{ 3 };
	constexpr float rookValue{ 5 };
	constexpr float queenValue{ 9 };

	for (int index{}; index < 64; ++index)
	{
		uint64_t mask{ static_cast<unsigned long long>(1) << index };

		if (mask & gameState.bitBoards.whitePawns) whiteValue += pawnValue;
		else if (mask & gameState.bitBoards.whiteKnights) whiteValue += knightValue;
		else if (mask & gameState.bitBoards.whiteBishops) whiteValue += bishopValue;
		else if (mask & gameState.bitBoards.whiteRooks) whiteValue += rookValue;
		else if (mask & gameState.bitBoards.whiteQueens) whiteValue += queenValue;

		else if (mask & gameState.bitBoards.blackPawns) blackValue += pawnValue;
		else if (mask & gameState.bitBoards.blackKnights) blackValue += knightValue;
		else if (mask & gameState.bitBoards.blackBishops) blackValue += bishopValue;
		else if (mask & gameState.bitBoards.blackRooks) blackValue += rookValue;
		else if (mask & gameState.bitBoards.blackQueens) blackValue += queenValue;
	}

	return m_ControllingWhite == gameState.whiteToMove ? whiteValue - blackValue : blackValue - whiteValue;
}

Node* ChessAI_V1_MCST::SelectNode(Node* node) 
{
	do
	{
		bool isEnemyNode{ node->gameState.whiteToMove != m_ControllingWhite };

		float bestUCB1{isEnemyNode? FLOAT_MAX : FLOAT_MIN};
		Node* selectedChild{ node->children.back()};

		for (Node* child : node->children)
		{
			if (child->visits == 0) { selectedChild = child; break; }


			constexpr float C = 0.42f; // Constant C for UCB1 formula

			const float exploitationTerm{ child->totalScore / child->visits };
			const float explorationTerm{ sqrt(logf(float(node->visits)) / child->visits) };
			const float ucb1 = exploitationTerm + C * explorationTerm;
			
			if (!isEnemyNode)
			{
				if (ucb1 > bestUCB1)
				{
					bestUCB1 = ucb1;
					selectedChild = child;
				}
			}
			else
			{
				if (ucb1 < bestUCB1)
				{
					bestUCB1 = ucb1;
					selectedChild = child;
				}
			}
		}

		node = selectedChild;

	} 
	while (node->visits > 1);
	

	return node;
}
void ChessAI_V1_MCST::ExpandNode(Node* node)
{
	for (const auto& move : node->gameState.possibleMoves) 
	{
		m_pChessBoard->MakeMove(node->move);

		Node* newNode = new Node{move, node, m_pChessBoard->GetCurrentGameState() };
		node->children.push_back(newNode);

		m_pChessBoard->UnMakeLastMove();
	}

}
float ChessAI_V1_MCST::Rollout(Node* node) 
{
	//int moveCounter{};
	//while (m_pChessBoard->GetGameProgress() == GameProgress::InProgress) 
	//{
	//	auto possibleMoves = m_pChessBoard->GetPossibleMoves();
	//	if (possibleMoves.empty()) break;
	//
	//	int randomIndex = rand() % possibleMoves.size();
	//	auto it{ possibleMoves.begin() };
	//	std::advance(it, randomIndex);
	//
	//	m_pChessBoard->MakeMove(*it);
	//	++moveCounter;
	//}

	// Evaluate the final position using a simple heuristic
	float evalValue = BoardValueEvaluation(node->gameState);
	//m_pChessBoard->UnMakeLastMove(moveCounter);
	return evalValue;
}
void ChessAI_V1_MCST::Backpropagate(Node* node, float score) {
	while (node != nullptr) 
	{
		++node->visits;
		node->totalScore += score;
		node = node->parent;
	}
}
