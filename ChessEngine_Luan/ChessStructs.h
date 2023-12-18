#pragma once

#include "stdint.h"
#include "GameEngine.h"
#include <list>
#include <vector>

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

	uint64_t checkRay{};

	uint64_t whiteThreatMap{};
	uint64_t blackThreatMap{};

	uint64_t nullBitBoard{};


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
			   blackKing == other.blackKing &&
			   whiteThreatMap == other.whiteThreatMap &&
			   blackThreatMap == other.blackThreatMap &&
			   checkRay == other.checkRay;
				
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
	NullMove,

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

	MoveType moveType{MoveType::NullMove};

	bool operator==(Move other)
	{
		return (startSquareIndex == other.startSquareIndex && targetSquareIndex == other.targetSquareIndex && moveType == other.moveType);
	}

};

enum class GameProgress
{
	InProgress,
	Draw,
	WhiteWon,
	BlackWon
};

struct GameState
{
	GameProgress gameProgress;

	BitBoards bitBoards;
	std::list<Move> possibleMoves;

	bool whiteToMove;

	bool whiteCanCastleQueenSide;
	bool whiteCanCastleKingSide;
	bool blackCanCastleQueenSide;
	bool blackCanCastleKingSide;

	uint64_t enPassantSquares;

	int halfMoveClock;
	int fullMoveCounter;
};

struct KnightOffsets
{
	const std::vector<int> squareOffsets{ -10, -17, -15, -6, +10, +17, +15, +6 };
	const std::vector<int> leftBounds{ 2, 1, 0, 0, 0, 0, 1, 2 };
	const std::vector<int> northBounds{ 1, 2, 2, 1, 0, 0, 0, 0 };
	const std::vector<int> rightBounds{ 0, 0, 1, 2, 2, 1, 0, 0 };
	const std::vector<int> southBounds{ 0, 0, 0, 0, 1, 2, 2, 1 };
};

struct SlidingOffsets
{
	SlidingOffsets()
	{
		distancesFromEdges.resize(8);
		for (int index{}; index < 8; ++index)
		{
			distancesFromEdges[index].resize(64);
		}


		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			distancesFromEdges[0][squareIndex] = squareIndex % 8;
		}

		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			distancesFromEdges[1][squareIndex] = squareIndex / 8;
		}

		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			distancesFromEdges[2][squareIndex] = 7 - distancesFromEdges[0][squareIndex];
		}

		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			distancesFromEdges[3][squareIndex] = 7 - distancesFromEdges[1][squareIndex];
		}

		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			distancesFromEdges[4][squareIndex] = min(distancesFromEdges[0][squareIndex], distancesFromEdges[1][squareIndex]);
		}

		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			distancesFromEdges[5][squareIndex] = min(distancesFromEdges[2][squareIndex], distancesFromEdges[1][squareIndex]);
		}

		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			distancesFromEdges[6][squareIndex] = min(distancesFromEdges[2][squareIndex], distancesFromEdges[3][squareIndex]);
		}

		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			distancesFromEdges[7][squareIndex] = min(distancesFromEdges[0][squareIndex], distancesFromEdges[3][squareIndex]);
		}
	}


	const std::vector<int> squareOffsets{-1, -8, +1, +8, -9, -7, +9, +7};

	std::vector<std::vector<int>> distancesFromEdges;
};

struct BitMasks
{
	BitMasks()
	{
		bitMasks.resize(64);
		for (int index{}; index < 64; ++index)
		{
			bitMasks[index] = static_cast<uint64_t>(1) << index;
		}
	}

	std::vector<uint64_t> bitMasks;

};