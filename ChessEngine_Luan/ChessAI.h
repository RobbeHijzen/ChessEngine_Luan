#pragma once

#include "ChessBoard.h"
class ChessAI
{
public:
	ChessAI(ChessBoard* chessBoard, bool controllingWhite) : m_pChessBoard{ chessBoard }, m_ControllingWhite{controllingWhite} {};
	~ChessAI() = default;

	ChessAI(const ChessAI& other) = default;
	ChessAI(ChessAI&& other) = default;
	ChessAI& operator=(const ChessAI& other) = default;
	ChessAI& operator=(ChessAI&& other) noexcept = default;


	virtual Move GetAIMove() = 0;
	virtual int BoardValueEvaluation() { return 0; };
	bool IsControllingWhite() { return m_ControllingWhite; }

protected:

	ChessBoard* m_pChessBoard;
	bool m_ControllingWhite;
};

