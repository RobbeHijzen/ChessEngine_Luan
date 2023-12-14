#pragma once

#include "stdint.h"

struct BitBoards
{
	uint64_t whitePawns{};
	uint64_t whiteKnights{};
	uint64_t whiteBishops{};
	uint64_t whiteRooks{};
	uint64_t whiteQueens{};
	uint64_t whiteKing{};

	uint64_t blackPawns{};
	uint64_t blackKnights{};
	uint64_t blackBishops{};
	uint64_t blackRooks{};
	uint64_t blackQueens{};
	uint64_t blackKing{};

	uint64_t whitePieces{};
	uint64_t blackPieces{};

	uint64_t nullBitBoard{0};

	bool operator==(BitBoards other)
	{
		return whitePawns == other.whitePawns &&
			   whiteKnights == other.whiteKnights &&
			   whiteBishops == other.whiteBishops &&
			   whiteRooks == other.whiteRooks &&
			   whiteQueens == other.whiteQueens &&
			   whiteKing == other.whiteKing &&
			   blackPawns == other.blackPawns &&
			   blackKnights == other.blackKnights &&
			   blackBishops == other.blackBishops &&
			   blackRooks == other.blackRooks &&
			   blackQueens == other.blackQueens &&
			   blackKing == other.blackKing;
	}
};

enum class FENFields
{
	PiecePlacement,
	SideToMove,
	CastlingAbility,
	EnPassentTargetSquare,
	HalfmoveClock,
	FullmoveCounter
};

enum class MoveType
{
	QuietMove,
	DoublePawnPush,
	KingCastle,
	QueenCastle,
	Capture,
	EnPassantCaptureLeft,
	EnPassantCaptureRight,

	KnightPromotion,
	BishopPromotion,
	RookPromotion,
	QueenPromotion,

	KnightPromotionCapture,
	BishopPromotionCapture,
	RookPromotionCapture,
	QueenPromotionCapture

};

struct Move
{
	int startSquareIndex{};
	int targetSquareIndex{};

	MoveType moveType{MoveType::QuietMove};

	bool operator==(Move other)
	{
		return (startSquareIndex == other.startSquareIndex && targetSquareIndex == other.targetSquareIndex && moveType == other.moveType);
	}

};

enum class GameState
{
	InProgress,
	Draw,
	WhiteWon,
	BlackWon
};

