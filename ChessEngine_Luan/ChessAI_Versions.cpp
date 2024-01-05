#include "ChessAI_Versions.h"
#include <execution>
#include <ranges>

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

#pragma region AlphaBeta

#pragma region V1
Move ChessAI_V1_AlphaBeta::GetAIMove()
{
	m_StartTimePoint = std::chrono::steady_clock::now();

	int depth{ 3 };

	auto possibleMoves{ m_pChessBoard->GetPossibleMoves() };
	if (possibleMoves.size() == 1) return possibleMoves.front();
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
	m_CurrentTimePoint = std::chrono::steady_clock::now();

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
#pragma endregion

#pragma region V2
Move ChessAI_V2_AlphaBeta::GetAIMove()
{
	m_StartTimePoint = std::chrono::steady_clock::now();

	int depth{ 5 };

	auto possibleMoves{ m_pChessBoard->GetPossibleMoves() };
	if (possibleMoves.size() == 1) return possibleMoves.front();
	
	Move currentBestMove{ possibleMoves.front() };
	float currentBestValue{ FLOAT_MIN };

	float alpha{ FLOAT_MIN };
	float beta{ FLOAT_MAX };

	std::for_each(std::execution::par, possibleMoves.begin(), possibleMoves.end(), [&](Move move)
	{
		ChessBoard copyBoard{ *m_pChessBoard };
		copyBoard.MakeMove(move);

		float moveValue{ DepthSearch(depth - 1, alpha, beta, &copyBoard) };
		if (moveValue > currentBestValue) { currentBestMove = move; currentBestValue = moveValue; }		
	});
	return currentBestMove;
}
float ChessAI_V2_AlphaBeta::DepthSearch(int depth, float alpha, float beta, ChessBoard* pChessBoard)
{
	m_CurrentTimePoint = std::chrono::steady_clock::now();

	bool isMinimizer{ !bool(depth & 1) };


	if (depth == 0) return BoardValueEvaluation(pChessBoard->GetCurrentGameState());

	auto possibleMoves{ pChessBoard->GetPossibleMoves() };
	float currentMoveValue{ isMinimizer ? FLOAT_MAX : FLOAT_MIN };

	for (const auto& move : possibleMoves)
	{
		pChessBoard->MakeMove(move);
		float moveValue{ DepthSearch(depth - 1, alpha, beta, pChessBoard) };
		pChessBoard->UnMakeLastMove();

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
float ChessAI_V2_AlphaBeta::BoardValueEvaluation(GameState gameState)
{
	switch (gameState.gameProgress){
		case GameProgress::Draw: return 0;
		case GameProgress::WhiteWon: return m_ControllingWhite ? FLOAT_MAX - 100.f : FLOAT_MIN + 100.f;
		case GameProgress::BlackWon: return m_ControllingWhite ? FLOAT_MIN + 100.f : FLOAT_MAX - 100.f;
		default: break; }


	float boardValue{};
	boardValue = MaterialBalance(gameState) +
				 MoveBalance(gameState);

	

	return m_ControllingWhite ? boardValue : -boardValue;
}
float ChessAI_V2_AlphaBeta::MaterialBalance(GameState gameState)
{
	int K{ AmountOfPieces(gameState.bitBoards.whiteKing) };
	int Q{ AmountOfPieces(gameState.bitBoards.whiteQueens) };
	int R{ AmountOfPieces(gameState.bitBoards.whiteRooks) };
	int B{ AmountOfPieces(gameState.bitBoards.whiteBishops) };
	int N{ AmountOfPieces(gameState.bitBoards.whiteKnights) };
	int P{ AmountOfPieces(gameState.bitBoards.whitePawns) };
	
	int k{ AmountOfPieces(gameState.bitBoards.blackKing) };
	int q{ AmountOfPieces(gameState.bitBoards.blackQueens) };
	int r{ AmountOfPieces(gameState.bitBoards.blackRooks) };
	int b{ AmountOfPieces(gameState.bitBoards.blackBishops) };
	int n{ AmountOfPieces(gameState.bitBoards.blackKnights) };
	int p{ AmountOfPieces(gameState.bitBoards.blackPawns) };
	
	int totalPieceAmount{ K + Q + R + B + N + P + k + q + r + b + n + p};
	float gameStagePercent{totalPieceAmount / 32.f};

	float value{};
	for (int index{}; index < 64; ++index)
	{
		uint64_t mask{ static_cast<unsigned long long>(1) << index };

		if		(mask & gameState.bitBoards.whitePawns)   value += m_PieceTables.pawnStartBlack[ 63 - index]	* gameStagePercent + m_PieceTables.pawnEndBlack[63 - index]		* (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.whiteKnights) value += m_PieceTables.knightStartBlack[63 - index]	* gameStagePercent + m_PieceTables.knightEndBlack[63 - index]	* (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.whiteBishops) value += m_PieceTables.bishopStartBlack[63 - index]	* gameStagePercent + m_PieceTables.bishopEndBlack[63 - index]	* (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.whiteRooks)	  value += m_PieceTables.rookStartBlack[63 - index]		* gameStagePercent + m_PieceTables.rookEndBlack[63 - index]		* (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.whiteQueens)  value += m_PieceTables.queenStartBlack[63 - index]	* gameStagePercent + m_PieceTables.queenEndBlack[63 - index]	* (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.whiteKing)	  value += m_PieceTables.kingStartBlack[63 - index]		* gameStagePercent + m_PieceTables.kingEndBlack[63 - index]		* (1 - gameStagePercent);


		else if (mask & gameState.bitBoards.blackPawns)   value -= m_PieceTables.pawnStartBlack[index]		* gameStagePercent + m_PieceTables.pawnEndBlack[index]		* (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.blackKnights) value -= m_PieceTables.knightStartBlack[index]	* gameStagePercent + m_PieceTables.knightEndBlack[index]	* (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.blackBishops) value -= m_PieceTables.bishopStartBlack[index]	* gameStagePercent + m_PieceTables.bishopEndBlack[index]	* (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.blackRooks)   value -= m_PieceTables.rookStartBlack[index]		* gameStagePercent + m_PieceTables.rookEndBlack[index]		* (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.blackQueens)  value -= m_PieceTables.queenStartBlack[index]		* gameStagePercent + m_PieceTables.queenEndBlack[index]		* (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.blackKing)    value -= m_PieceTables.kingStartBlack[index]		* gameStagePercent + m_PieceTables.kingEndBlack[index]		* (1 - gameStagePercent);
	}

	return value;
}
float ChessAI_V2_AlphaBeta::MoveBalance(GameState gameState)
{
	float amount{ float(gameState.possibleMoves.size()) - m_MoveAmountOffset };
	return m_MoveAmountValue * (gameState.whiteToMove ? amount : -amount );
}
int ChessAI_V2_AlphaBeta::AmountOfPieces(uint64_t bitBoard)
{
	int amount{};
	for (int index{}; index < 64; ++index)
	{
		if (bitBoard & (static_cast<unsigned long long>(1) << index)) ++amount;
	}
	return amount;
}
#pragma endregion

#pragma region V3
Move ChessAI_V3_AlphaBeta::GetAIMove()
{
	m_StartTimePoint = std::chrono::steady_clock::now();

	auto possibleMoves{ m_pChessBoard->GetPossibleMoves() };

	int W{ AmountOfPieces(m_pChessBoard->GetCurrentGameState().bitBoards.whitePieces) };
	int B{ AmountOfPieces(m_pChessBoard->GetCurrentGameState().bitBoards.blackPieces) };
	int totalPieceAmount{ W + B };

	int depth{ 5 };
	if (totalPieceAmount <= 5)
	{
		depth = 13;
	}
	else if (totalPieceAmount <= 6)
	{
		depth = 11;
	}
	else if (totalPieceAmount <= 7)
	{
		depth = 9;
	}
	else if (totalPieceAmount <= 8)
	{
		depth = 7;
	}

	if (possibleMoves.size() == 1) return possibleMoves.front();

	Move currentBestMove{ possibleMoves.front() };
	float currentBestValue{ FLOAT_MIN };

	float alpha{ FLOAT_MIN };
	float beta{ FLOAT_MAX };

	std::for_each(std::execution::par, possibleMoves.begin(), possibleMoves.end(), [&](Move move)
		{
			ChessBoard copyBoard{ *m_pChessBoard };
			copyBoard.MakeMove(move);

			float moveValue{ DepthSearch(depth - 1, alpha, beta, &copyBoard) };
			if (moveValue > currentBestValue)
			{ 
				currentBestMove = move;
				currentBestValue = moveValue; 
			}
		});
	return currentBestMove;
}
float ChessAI_V3_AlphaBeta::DepthSearch(int depth, float alpha, float beta, ChessBoard* pChessBoard)
{
	m_CurrentTimePoint = std::chrono::steady_clock::now();

	bool isMinimizer{ !bool(depth & 1) };


	if (depth == 0) return BoardValueEvaluation(pChessBoard->GetCurrentGameState());

	auto possibleMoves{ pChessBoard->GetPossibleMoves() };
	float currentMoveValue{ isMinimizer ? FLOAT_MAX : FLOAT_MIN };

	for (const auto& move : possibleMoves)
	{
		pChessBoard->MakeMove(move);
		float moveValue{ DepthSearch(depth - 1, alpha, beta, pChessBoard) };
		pChessBoard->UnMakeLastMove();

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


float ChessAI_V3_AlphaBeta::BoardValueEvaluation(GameState gameState)
{
	switch (gameState.gameProgress) {
	case GameProgress::Draw: return 0;
	case GameProgress::WhiteWon: return m_ControllingWhite ? FLOAT_MAX - 100.f : FLOAT_MIN + 100.f;
	case GameProgress::BlackWon: return m_ControllingWhite ? FLOAT_MIN + 100.f : FLOAT_MAX - 100.f;
	default: break;
	}


	float boardValue{};
	boardValue = m_MaterialBalanceMult			* MaterialBalance(gameState) +
				 m_MaterialConsiderationsMult	* MaterialConsiderations(gameState) +
				 1								* PawnStructure(gameState) +
				 m_DevelopmentMult				* Development(gameState);


	return m_ControllingWhite ? boardValue : -boardValue;
}

float ChessAI_V3_AlphaBeta::MaterialBalance(GameState gameState)
{
	int W{ AmountOfPieces(gameState.bitBoards.whitePieces) };
	int B{ AmountOfPieces(gameState.bitBoards.blackPieces) };
	
	int totalPieceAmount{ W + B };
	float gameStagePercent{ totalPieceAmount / 32.f };

	float value{};
	for (int index{}; index < 64; ++index)
	{
		uint64_t mask{ static_cast<unsigned long long>(1) << index };

		if (mask & gameState.bitBoards.whitePawns)   value += m_PieceTables.pawnStartBlack[63 - index] * gameStagePercent + m_PieceTables.pawnEndBlack[63 - index] * (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.whiteKnights) value += m_PieceTables.knightStartBlack[63 - index] * gameStagePercent + m_PieceTables.knightEndBlack[63 - index] * (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.whiteBishops) value += m_PieceTables.bishopStartBlack[63 - index] * gameStagePercent + m_PieceTables.bishopEndBlack[63 - index] * (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.whiteRooks)	  value += m_PieceTables.rookStartBlack[63 - index] * gameStagePercent + m_PieceTables.rookEndBlack[63 - index] * (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.whiteQueens)  value += m_PieceTables.queenStartBlack[63 - index] * gameStagePercent + m_PieceTables.queenEndBlack[63 - index] * (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.whiteKing)	  value += m_PieceTables.kingStartBlack[63 - index] * gameStagePercent + m_PieceTables.kingEndBlack[63 - index] * (1 - gameStagePercent);


		else if (mask & gameState.bitBoards.blackPawns)   value -= m_PieceTables.pawnStartBlack[index] * gameStagePercent + m_PieceTables.pawnEndBlack[index] * (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.blackKnights) value -= m_PieceTables.knightStartBlack[index] * gameStagePercent + m_PieceTables.knightEndBlack[index] * (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.blackBishops) value -= m_PieceTables.bishopStartBlack[index] * gameStagePercent + m_PieceTables.bishopEndBlack[index] * (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.blackRooks)   value -= m_PieceTables.rookStartBlack[index] * gameStagePercent + m_PieceTables.rookEndBlack[index] * (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.blackQueens)  value -= m_PieceTables.queenStartBlack[index] * gameStagePercent + m_PieceTables.queenEndBlack[index] * (1 - gameStagePercent);
		else if (mask & gameState.bitBoards.blackKing)    value -= m_PieceTables.kingStartBlack[index] * gameStagePercent + m_PieceTables.kingEndBlack[index] * (1 - gameStagePercent);
	}

	return value;
}
float ChessAI_V3_AlphaBeta::MaterialConsiderations(GameState gameState)
{
	float value{};

	constexpr int rookPairBonus{-1}; // Principle of redundancy, 1 rook is enough
	constexpr int bishopPairBonus{1};
	constexpr int knightPairBonus{-1};
	constexpr int noPawnsBonus{-1};


	int R{ AmountOfPieces(gameState.bitBoards.whiteRooks) };
	int B{ AmountOfPieces(gameState.bitBoards.whiteBishops) };
	int N{ AmountOfPieces(gameState.bitBoards.whiteKnights) };
	int P{ AmountOfPieces(gameState.bitBoards.whitePawns) };

	int r{ AmountOfPieces(gameState.bitBoards.blackRooks) };
	int b{ AmountOfPieces(gameState.bitBoards.blackBishops) };
	int n{ AmountOfPieces(gameState.bitBoards.blackKnights) };
	int p{ AmountOfPieces(gameState.bitBoards.blackPawns) };

	if (R >= 2) value += rookPairBonus;
	if (B >= 2) value += bishopPairBonus;
	if (N >= 2) value += knightPairBonus;
	if (P == 0) value += noPawnsBonus;

	if (r >= 2) value -= rookPairBonus;
	if (b >= 2) value -= bishopPairBonus;
	if (n >= 2) value -= knightPairBonus;
	if (p == 0) value -= noPawnsBonus;

	return value;
}


float ChessAI_V3_AlphaBeta::PawnStructure(GameState gameState)
{
	float value =	m_DoubledPawnsMult * DoubledPawns(gameState) +
					m_IsolatedPawnsMult * IsolatedPawns(gameState) +
					m_PassedPawnsMult * PassedPawns(gameState);

	return value;
}

float ChessAI_V3_AlphaBeta::DoubledPawns(GameState gameState)
{
	int whiteDoubledPawns{};
	int blackDoubledPawns{};

	for (int column = 0; column < 8; ++column)
	{
		int whitePawnsOnColumn{ AmountOfPieces(gameState.bitBoards.whitePawns & ColumnMask(column)) };
		int blackPawnsOnColumn{ AmountOfPieces(gameState.bitBoards.blackPawns & ColumnMask(column)) };

		if (whitePawnsOnColumn > 1) whiteDoubledPawns += (whitePawnsOnColumn - 1);
		if (blackPawnsOnColumn > 1) blackDoubledPawns += (blackPawnsOnColumn - 1);
	}

	return float(whiteDoubledPawns - blackDoubledPawns);
}
float ChessAI_V3_AlphaBeta::IsolatedPawns(GameState gameState)
{
	int whiteIsolatedPawns{};
	int blackIsolatedPawns{};

	for (int column = 0; column < 8; ++column)
	{
		bool hasWhitePawnOnColumn{ bool(gameState.bitBoards.whitePawns & ColumnMask(column)) };
		bool hasBlackPawnOnColumn{ bool(gameState.bitBoards.blackPawns & ColumnMask(column)) };

		uint64_t leftColumn{ column >= 1 ? ColumnMask(column - 1) : 0};
		uint64_t rightColumn{ column <= 6 ? ColumnMask(column + 1) : 0 };
		uint64_t whitePawnsOnLeftColumn{ gameState.bitBoards.whitePawns & leftColumn };
		uint64_t whitePawnsOnRightColumn{ gameState.bitBoards.whitePawns & rightColumn };
		uint64_t blackPawnsOnLeftColumn{ gameState.bitBoards.blackPawns & leftColumn };
		uint64_t blackPawnsOnRightColumn{ gameState.bitBoards.blackPawns & rightColumn };


		if (hasWhitePawnOnColumn)
		{
			if (!(whitePawnsOnLeftColumn & whitePawnsOnLeftColumn))
			{
				++whiteIsolatedPawns;
			}
		}

		if (hasBlackPawnOnColumn)
		{
			if (!(blackPawnsOnLeftColumn & blackPawnsOnLeftColumn))
			{
				++blackIsolatedPawns;
			}
		}
		
	}

	return float(blackIsolatedPawns - whiteIsolatedPawns);
}
float ChessAI_V3_AlphaBeta::PassedPawns(GameState gameState)
{
	int whitePassedPawns{};
	int blackPassedPawns{};

	for (int column = 0; column < 8; ++column) 
	{
		uint64_t currentFile{ ColumnMask(column) };
		uint64_t whitePawnsOnColumn{ gameState.bitBoards.whitePawns & currentFile };
		uint64_t blackPawnsOnColumn{ gameState.bitBoards.blackPawns & currentFile };

		// Check if there are no enemy pawns in front of the white pawns
		if (!blackPawnsOnColumn) 
		{
			++whitePassedPawns;
		}
		// Check if there are no enemy pawns in front of the black pawns
		if (!whitePawnsOnColumn) 
		{
			++blackPassedPawns;
		}
	}

	return float(whitePassedPawns - blackPassedPawns);
}

float ChessAI_V3_AlphaBeta::Development(GameState gameState)
{
	float value{};

	int W{ AmountOfPieces(gameState.bitBoards.whitePieces) };
	int B{ AmountOfPieces(gameState.bitBoards.blackPieces) };
	int totalPieceAmount{ W + B };
	float gameStagePercent{ totalPieceAmount / 32.f };


	uint64_t whitePieces = gameState.bitBoards.whitePieces;
	uint64_t blackPieces = gameState.bitBoards.blackPieces;

	int whiteDeveloped = AmountOfPieces(whitePieces & 0x0000FFFFFFFFFFFF);
	int blackDeveloped = AmountOfPieces(blackPieces & 0xFFFFFFFFFFFF0000);


	return (whiteDeveloped - blackDeveloped) * gameStagePercent;
}

int ChessAI_V3_AlphaBeta::AmountOfPieces(uint64_t bitBoard)
{
	int amount{};
	for (int index{}; index < 64; ++index)
	{
		if (bitBoard & (static_cast<unsigned long long>(1) << index)) ++amount;
	}
	return amount;
}

uint64_t ChessAI_V3_AlphaBeta::ColumnMask(int column)
{
	uint64_t bitBoard{0};
	for (int index{ column }; index < 64; index += 8)
	{
		bitBoard |= static_cast<unsigned long long>(1) << index;
	}
	return bitBoard;
}
#pragma endregion

#pragma endregion


#pragma region Monte Carlo Search Tree

Move ChessAI_V1_MCST::GetAIMove()
{
	m_StartTimePoint = std::chrono::steady_clock::now();

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

#pragma endregion
