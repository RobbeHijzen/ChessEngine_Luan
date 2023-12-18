#include "ChessAI_Versions.h"


Move ChessAI_V1::GetAIMove()
{
	auto moves{ m_pChessBoard->GetPossibleMoves() };
	auto it{ moves.begin()};

	auto randomIndex{ rand() % moves.size() };
	std::advance(it, randomIndex);

	return *it;
}

Move ChessAI_V2_AlphaBeta::GetAIMove()
{
	int depth{ 6 };

	auto possibleMoves{ m_pChessBoard->GetPossibleMoves() };
	Move currentBestMove{};
	int currentBestValue{INT_MIN};

	int alpha{INT_MIN};
	int beta{INT_MAX};

	for (int index{}; index < possibleMoves.size(); ++index)
	{

		auto it{ possibleMoves.begin() };
		std::advance(it, index);
		Move move{ *it };

		m_pChessBoard->MakeMove(move);

		int moveValue{ DepthSearch(depth - 1, alpha, beta) };
		if (moveValue > currentBestValue) { currentBestMove = move; currentBestValue = moveValue; }

		m_pChessBoard->UnMakeLastMove();
	}

	return currentBestMove;
}
int ChessAI_V2_AlphaBeta::DepthSearch(int depth, int alpha, int beta)
{
	bool isMinimizer{ bool(depth & 1) };

	if (depth == 0) return BoardValueEvaluation();

	auto possibleMoves{ m_pChessBoard->GetPossibleMoves() };
	int currentMoveValue{ isMinimizer ? INT_MAX : INT_MIN};

	for (const auto& move : possibleMoves)
	{
		m_pChessBoard->MakeMove(move);
		int moveValue{ DepthSearch(depth - 1, alpha, beta) };
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
int ChessAI_V2_AlphaBeta::BoardValueEvaluation()
{
	int whiteValue{};
	int blackValue{};

	constexpr int pawnValue{1};
	constexpr int knightValue{3};
	constexpr int bishopValue{3};
	constexpr int rookValue{5};
	constexpr int queenValue{9};

	GameState gameState{ m_pChessBoard->GetCurrentGameState() };

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


Move ChessAI_V2_MCST::GetAIMove()
{

	auto possibleMoves{ m_pChessBoard->GetPossibleMoves() };
	Move currentBestMove{};
	
	int N{};

	while (N < 500)
	{
		float UCB1{FLT_MIN};
		int UCB1Index{};

		for (const auto& move : possibleMoves)
		{


		}
		++N;
	}


	return currentBestMove;
}
int ChessAI_V2_MCST::DepthSearch()
{
	
	return 0;
}
int ChessAI_V2_MCST::BoardValueEvaluation()
{
	int whiteValue{};
	int blackValue{};

	constexpr int pawnValue{ 1 };
	constexpr int knightValue{ 3 };
	constexpr int bishopValue{ 3 };
	constexpr int rookValue{ 5 };
	constexpr int queenValue{ 9 };

	GameState gameState{ m_pChessBoard->GetCurrentGameState() };

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


