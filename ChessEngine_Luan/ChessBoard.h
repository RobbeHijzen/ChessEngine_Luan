#pragma once

#include <iostream>
#include <vector>
#include <list>
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


	void MakeMove(Move move, bool originalBoard = true);
	void UnMakeMove(Move move);
	bool IsLegalMove(Move move);
	Move GetMoveFromSquares(int startSquare, int targetSquare);

	GameState GetGameState() { return m_GameState; }
	bool GetFirstFrameGameEnd() { return m_FirstFrameGameEnd; }
	void SetFirstFrameGameEnd(bool isFirstFrameGameEnd) { m_FirstFrameGameEnd = isFirstFrameGameEnd; }

	std::list<Move> GetPossibleMoves() { return m_PossibleMoves; }


protected:

	BitBoards m_BitBoards{};
	std::list<Move> m_PossibleMoves{};

private:

	std::vector<BitBoards> m_BitBoardsHistory{};

	GameState m_GameState{GameState::InProgress};
	bool m_FirstFrameGameEnd{ false };

	bool m_WhiteToMove{ true };

	bool m_WhiteCanCastleQueenSide{ false };
	bool m_WhiteCanCastleKingSide{ false };
	bool m_BlackCanCastleQueenSide{ false };
	bool m_BlackCanCastleKingSide{ false };

	bool m_WhiteKingSideRookHasMoved{false};
	bool m_WhiteQueenSideRookHasMoved{false};
	bool m_BlackKingSideRookHasMoved{false};
	bool m_BlackQueenSideRookHasMoved{false};



	uint64_t m_EnPassantSquares{};

	int m_HalfMoveClock{};
	int m_FullMoveCounter{ 1 };


	void CheckCastleRights(uint64_t startSquareBitBoard, int startSquareIndex);

	void CalculatePossibleMoves();
	void CalculatePawnMoves();
	void CalculateKnightMoves();
	void CalculateBishopMoves();
	void CalculateRookMoves();
	void CalculateQueenMoves();
	void CalculateKingMoves();





	void SetBitboardsFromFEN(std::string FEN);
	void SetPositionFromChar(char c, int& squareIndex);
	void SetSideToMoveFromChar(char c);
	void SetCastlingRulesFromChar(char c);
	void SetEnPassantSquaresFromChars(char file, char rank, int& index);


	uint64_t* GetBitboardFromSquare(int squareIndex);
	void UpdateColorBitboards();

	void CheckForIllegalMoves();
	bool IsOtherKingInCheck();

	void CheckForGameEnd();
	void CheckForCheckmate();
	void CheckForFiftyMoveRule();
	void CheckForInsufficientMaterial();
	int GetAmountOfPiecesFromBitBoard(uint64_t bitBoard);
	void CheckForRepetition();
};

