#include "ChessAI_Versions.h"


Move ChessAI_V1::GetAIMove()
{
	auto moves{ m_pChessBoard->GetPossibleMoves() };
	auto it{ moves.begin()};

	auto randomIndex{ rand() % moves.size() };
	std::advance(it, randomIndex);

	return *it;
}

Move ChessAI_V2::GetAIMove()
{
	auto possibleMoves{ m_pChessBoard->GetPossibleMoves() };
	Move currentBestMove{};
	int currentBestValue{INT_MIN};

	for (int index{}; index < possibleMoves.size(); ++index)
	{

		auto it{ possibleMoves.begin() };
		std::advance(it, index);
		Move move{ *it };

		m_pChessBoard->MakeMove(move);

		int moveValue{ DepthSearch(2) };
		if (moveValue > currentBestValue) { currentBestMove = move; currentBestValue = moveValue; }

		m_pChessBoard->UnMakeLastMove();
	}

	return currentBestMove;
}
int ChessAI_V2::DepthSearch(int depth)
{
	auto possibleMoves{ m_pChessBoard->GetPossibleMoves() };
	int currentMoveValue{ (depth & 1) ? INT_MAX : INT_MIN};

	for (int index{}; index < possibleMoves.size(); ++index)
	{

		auto it{ possibleMoves.begin() };
		std::advance(it, index);
		Move move{ *it };

		m_pChessBoard->MakeMove(move);

		int moveValue{};
		if (depth == 1)
		{
			moveValue = BoardValueEvaluation();
		}
		else
		{
			int moveValue{ DepthSearch(depth - 1) };
		}

		(depth & 1) ? currentMoveValue = min(currentMoveValue, moveValue) : currentMoveValue = max(currentMoveValue, moveValue);
		

		m_pChessBoard->UnMakeLastMove();
	}

	return currentMoveValue;
}
int ChessAI_V2::BoardValueEvaluation()
{
	int whiteValue{};
	int blackValue{};

	constexpr int pawnValue{1};
	constexpr int knightValue{13};
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


