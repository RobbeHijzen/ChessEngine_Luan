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

class ChessAI_V2 final : public ChessAI
{
public:
	ChessAI_V2(ChessBoard* chessBoard, bool controllingWhite) : ChessAI(chessBoard, controllingWhite) {};
	~ChessAI_V2() = default;

	ChessAI_V2(const ChessAI_V2& other) = delete;
	ChessAI_V2(ChessAI_V2&& other) = delete;
	ChessAI_V2& operator=(const ChessAI_V2& other) = delete;
	ChessAI_V2& operator=(ChessAI_V2&& other) noexcept = delete;


	virtual Move GetAIMove() override;
	virtual int BoardValueEvaluation() override;

	int DepthSearch(int depth);
};


