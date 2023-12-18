#pragma once
#include "ChessAI.h"

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


struct Node {

	Node(const Move& _move = {}, Node* _parent = nullptr, const GameState& _gameState = {}) : move{ _move }, parent{ _parent }, gameState{ _gameState } {}
	~Node()
	{
		parent = nullptr;
		for (auto node : children)
		{
			delete node;
		}
	}

	Move move;
	int visits{};
	float totalScore{};
	std::vector<Node*> children{};
	Node* parent{};

	GameState gameState{};
};

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

