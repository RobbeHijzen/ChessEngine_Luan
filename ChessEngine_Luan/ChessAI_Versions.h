#pragma once
#include "ChessAI.h"
#include "ChessAIHelpers.h"

class ChessAI_V0 final : public ChessAI
{
public:
	ChessAI_V0(ChessBoard* chessBoard, bool controllingWhite) : ChessAI(chessBoard, controllingWhite) {};
	~ChessAI_V0() = default;

	ChessAI_V0(const ChessAI_V0& other) = delete;
	ChessAI_V0(ChessAI_V0&& other) = delete;
	ChessAI_V0& operator=(const ChessAI_V0& other) = delete;
	ChessAI_V0& operator=(ChessAI_V0&& other) noexcept = delete;


	virtual Move GetAIMove() override;


};

#pragma region AlphaBeta

class ChessAI_V1_AlphaBeta final : public ChessAI
{
public:
	ChessAI_V1_AlphaBeta(ChessBoard* chessBoard, bool controllingWhite) : ChessAI(chessBoard, controllingWhite) {};
	~ChessAI_V1_AlphaBeta() = default;

	ChessAI_V1_AlphaBeta(const ChessAI_V1_AlphaBeta& other) = delete;
	ChessAI_V1_AlphaBeta(ChessAI_V1_AlphaBeta&& other) = delete;
	ChessAI_V1_AlphaBeta& operator=(const ChessAI_V1_AlphaBeta& other) = delete;
	ChessAI_V1_AlphaBeta& operator=(ChessAI_V1_AlphaBeta&& other) noexcept = delete;


	virtual Move GetAIMove() override;
	
private:
	float DepthSearch(int depth, float alpha, float beta);
	virtual float BoardValueEvaluation(GameState gameState) override;
};

class ChessAI_V2_AlphaBeta final : public ChessAI
{
public:
	ChessAI_V2_AlphaBeta(ChessBoard* chessBoard, bool controllingWhite) : ChessAI(chessBoard, controllingWhite) {};
	~ChessAI_V2_AlphaBeta() = default;

	ChessAI_V2_AlphaBeta(const ChessAI_V2_AlphaBeta& other) = delete;
	ChessAI_V2_AlphaBeta(ChessAI_V2_AlphaBeta&& other) = delete;
	ChessAI_V2_AlphaBeta& operator=(const ChessAI_V2_AlphaBeta& other) = delete;
	ChessAI_V2_AlphaBeta& operator=(ChessAI_V2_AlphaBeta&& other) noexcept = delete;

	virtual Move GetAIMove() override;

private:

	const float m_MoveAmountValue{ 10.f };
	const int m_MoveAmountOffset{ 20 };
	const PieceSquareTables m_PieceTables{};

	float DepthSearch(int depth, float alpha, float beta, ChessBoard* pChessBoard);
	virtual float BoardValueEvaluation(GameState gameState) override;


	float MaterialBalance(GameState gameState);
	float MoveBalance(GameState gameState);


	int AmountOfPieces(uint64_t bitBoard);
};
class ChessAI_V3_AlphaBeta final : public ChessAI
{
public:
	ChessAI_V3_AlphaBeta(ChessBoard* chessBoard, bool controllingWhite) : ChessAI(chessBoard, controllingWhite) {};
	~ChessAI_V3_AlphaBeta() = default;

	ChessAI_V3_AlphaBeta(const ChessAI_V3_AlphaBeta& other) = delete;
	ChessAI_V3_AlphaBeta(ChessAI_V3_AlphaBeta&& other) = delete;
	ChessAI_V3_AlphaBeta& operator=(const ChessAI_V3_AlphaBeta& other) = delete;
	ChessAI_V3_AlphaBeta& operator=(ChessAI_V3_AlphaBeta&& other) noexcept = delete;

	virtual Move GetAIMove() override;

private:

	const float m_MaterialBalanceMult{2.f};
	const float m_MaterialConsiderationsMult{15.f};
	const float m_DevelopmentMult{15.f};

	const float m_DoubledPawnsMult{35.f};
	const float m_IsolatedPawnsMult{35.f};
	const float m_PassedPawnsMult{35.f};

	const PieceSquareTables m_PieceTables{};

	float DepthSearch(int depth, float alpha, float beta, ChessBoard* pChessBoard);
	virtual float BoardValueEvaluation(GameState gameState) override;


	float MaterialBalance(GameState gameState);
	float MaterialConsiderations(GameState gameState);

	float PawnStructure(GameState gameState);
	float DoubledPawns(GameState gameState);
	float IsolatedPawns(GameState gameState);
	float PassedPawns(GameState gameState);

	float Development(GameState gameState);


	int AmountOfPieces(uint64_t bitBoard);

	uint64_t ColumnMask(int column);
};

#pragma endregion
#pragma region Monte Carlo Search Tree

class ChessAI_V1_MCST final : public ChessAI
{
public:
	ChessAI_V1_MCST(ChessBoard* chessBoard, bool controllingWhite) : ChessAI(chessBoard, controllingWhite) {};
	~ChessAI_V1_MCST() = default;

	ChessAI_V1_MCST(const ChessAI_V1_MCST& other) = delete;
	ChessAI_V1_MCST(ChessAI_V1_MCST&& other) = delete;
	ChessAI_V1_MCST& operator=(const ChessAI_V1_MCST& other) = delete;
	ChessAI_V1_MCST& operator=(ChessAI_V1_MCST&& other) noexcept = delete;


	virtual Move GetAIMove() override;
	
private:

	int m_Iterations{3000};

	virtual float BoardValueEvaluation(GameState gameState) override;
	

	Node* SelectNode(Node* node);
	void ExpandNode(Node* node);
	float Rollout(Node* node);
	void Backpropagate(Node* node, float score);



};

#pragma endregion