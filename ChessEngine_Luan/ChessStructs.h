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

struct Move
{


};