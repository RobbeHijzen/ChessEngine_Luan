#pragma once

#include "ChessBoard.h"
#include <chrono>

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
	bool IsControllingWhite() { return m_ControllingWhite; }
	float GetCurrentMoveTimer() { return std::chrono::duration<float>(m_CurrentTimePoint - m_StartTimePoint).count(); }

protected:

	ChessBoard* m_pChessBoard;
	bool m_ControllingWhite;
	std::chrono::steady_clock::time_point m_CurrentTimePoint{std::chrono::steady_clock::now()};
	std::chrono::steady_clock::time_point m_StartTimePoint{ std::chrono::steady_clock::now() };


	virtual float BoardValueEvaluation(GameState gameState) { return 0.f; };

};

