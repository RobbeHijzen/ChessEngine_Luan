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
	bool IsLegalMove(Move move);
	Move GetMoveFromSquares(int startSquare, int targetSquare);

	std::list<Move> GetPossibleMoves() { return m_PossibleMoves; }


protected:

	BitBoards m_BitBoards{};
	std::list<Move> m_PossibleMoves{};

private:


	bool m_WhiteToMove{ true };

	bool m_WhiteCanCastleQueenSide{ false };
	bool m_WhiteCanCastleKingSide{ false };
	bool m_BlackCanCastleQueenSide{ false };
	bool m_BlackCanCastleKingSide{ false };

	uint64_t m_EnPassantSquares{};

	int m_HalfMoveClock{};
	int m_FullMoveCounter{ 1 };


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
	bool IsKingInCheck();
	void CheckForCheckmate();

};

