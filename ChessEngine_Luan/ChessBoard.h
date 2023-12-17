#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <list>
#include <stack>
#include "HelperStructs.h"
#include "ChessStructs.h"

class ChessBoard
{
public:
	ChessBoard();

	~ChessBoard() = default;
	ChessBoard(const ChessBoard& other) = default;
	ChessBoard(ChessBoard&& other) noexcept = default;
	ChessBoard& operator=(const ChessBoard& other) = default;
	ChessBoard& operator=(ChessBoard&& other) noexcept = default;


	
	int StartMoveGenerationTest(int depth);

	
	void MakeMove(Move move);
	void UnMakeLastMove();
	bool IsLegalMove(Move move);
	Move GetMoveFromSquares(int startSquare, int targetSquare);

	GameProgress GetGameProgress() { return m_GameProgress; }

	std::list<Move> GetPossibleMoves() { return m_PossibleMoves; }

	int GetTotalAmount() { return m_TotalAmount; }
	int GetCaptureAmount() { return m_CaptureAmount; }
	int GetEnPassantAmount() { return m_EnPassantAmount; }
	int GetCastleAmount() { return m_CastleAmount; }
	int GetPromotionAmount() { return m_PromotionAmount; }
	int GetCheckAmount() { return m_CheckAmount; }

	bool GetWhiteToMove() { return m_WhiteToMove; }
	GameState GetCurrentGameState() { return m_GameStateHistory[m_GameStateHistoryCounter]; }

protected:

	BitBoards m_BitBoards{};
	std::list<Move> m_PossibleMoves{};

private:

	int m_TotalAmount{};
	int m_CaptureAmount{};
	int m_EnPassantAmount{};
	int m_CastleAmount{};
	int m_PromotionAmount{};
	int m_CheckAmount{};

	std::vector<GameState> m_GameStateHistory{};
	int m_GameStateHistoryCounter{-1};

	GameProgress m_GameProgress{GameProgress::InProgress};

	bool m_WhiteToMove{ true };
	bool m_IsKingInCheck{ false };
	bool m_IsKingInDoubleCheck{ false };


	bool m_WhiteCanCastleQueenSide{ false };
	bool m_WhiteCanCastleKingSide{ false };
	bool m_BlackCanCastleQueenSide{ false };
	bool m_BlackCanCastleKingSide{ false };


	uint64_t m_EnPassantSquares{};

	int m_HalfMoveClock{};
	int m_FullMoveCounter{ 1 };


	const KnightOffsets m_KnightOffsets{};
	const SlidingOffsets m_SlidingOffsets{};

	const BitMasks m_BitMasks{};

	uint64_t m_CurrentPawnsBitBoard{};
	uint64_t m_CurrentKnightsBitBoard{};
	uint64_t m_CurrentBishopsBitBoard{};
	uint64_t m_CurrentRooksBitBoard{};
	uint64_t m_CurrentQueensBitBoard{};
	uint64_t m_CurrentKingBitBoard{};

	uint64_t m_CurrentOwnPiecesBitBoard{};
	uint64_t m_CurrentOpponentPiecesBitBoard{};

	uint64_t* m_pCurrentOwnThreatMap{};
	uint64_t* m_pCurrentOpponentThreatMap{};

	std::vector<uint64_t> m_PinnedBoards{};
	uint64_t m_CurrentPinBoard{};


	int MoveGenerationTest(int depth, int initialDepth);

	void UpdateBitBoards(Move move, uint64_t* startBitBoard);
	void UpdateThreatMap(Move move, bool useMove = true);
	void UpdateRayMap(uint64_t checkingPieceMap, int targetSquare);
	void UpdatePinnedBoards();
	void CheckCastleRights(uint64_t startSquareBitBoard, int startSquareIndex);

	void CalculatePossibleMoves();
	void CalculatePawnMoves(int squareIndex);
	void CalculateKnightMoves(int squareIndex);
	void CalculateSlidingMoves(int squareIndex, int startOffsetIndex, int endOffsetIndex);
	void CalculateKingMoves(int squareIndex);

	void CalculatePawnThreats(int squareIndex, uint64_t* threatMap);
	void CalculateKnightThreats(int squareIndex, uint64_t* threatMap);
	void CalculateSlidingThreats(int squareIndex, uint64_t* threatMap, int startingOffsetIndex, int endOffsetIndex);
	void CalculateKingThreats(int squareIndex, uint64_t* threatMap);

	bool CalculateSlidingPins(int squareIndex, uint64_t& pinBoard, int startingOffsetIndex, int endOffsetIndex);
	void AdjustCurrentPinBoard(int squareIndex);

	void SetBitboardsFromFEN(std::string FEN);
	void SetPositionFromChar(char c, int& squareIndex);
	void SetSideToMoveFromChar(char c);
	void SetCastlingRulesFromChar(char c);
	void SetEnPassantSquaresFromChars(char file, char rank, int& index);


	uint64_t* GetBitboardFromSquare(int squareIndex);
	void UpdateColorBitboards();

	
	bool IsOtherKingInCheck();
	bool IsSquareInCheckByOtherColor(int squareIndex);

	void CheckForGameEnd();
	void CheckForCheckmate();
	void CheckForFiftyMoveRule();
	void CheckForInsufficientMaterial();
	int GetAmountOfPiecesFromBitBoard(uint64_t bitBoard);
	void CheckForRepetition();

	void UpdateGameStateHistory();
};

