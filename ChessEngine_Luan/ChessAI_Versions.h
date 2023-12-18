#pragma once
#include "ChessAI.h"

class ChessAI_V1 final : public ChessAI
{
public:
	ChessAI_V1(ChessBoard* chessBoard, bool controllingWhite) : ChessAI(chessBoard, controllingWhite) {};
	~ChessAI_V1() = default;

	ChessAI_V1(const ChessAI_V1& other) = delete;
	ChessAI_V1(ChessAI_V1&& other) = delete;
	ChessAI_V1& operator=(const ChessAI_V1& other) = delete;
	ChessAI_V1& operator=(ChessAI_V1&& other) noexcept = delete;


	virtual Move GetAIMove() override;


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
	virtual int BoardValueEvaluation() override;

	int DepthSearch(int depth, int alpha, int beta);
};

class ChessAI_V2_MCST final : public ChessAI
{
public:
	ChessAI_V2_MCST(ChessBoard* chessBoard, bool controllingWhite) : ChessAI(chessBoard, controllingWhite) {};
	~ChessAI_V2_MCST() = default;

	ChessAI_V2_MCST(const ChessAI_V2_MCST& other) = delete;
	ChessAI_V2_MCST(ChessAI_V2_MCST&& other) = delete;
	ChessAI_V2_MCST& operator=(const ChessAI_V2_MCST& other) = delete;
	ChessAI_V2_MCST& operator=(ChessAI_V2_MCST&& other) noexcept = delete;


	virtual Move GetAIMove() override;
	virtual int BoardValueEvaluation() override;

	int DepthSearch();
};

