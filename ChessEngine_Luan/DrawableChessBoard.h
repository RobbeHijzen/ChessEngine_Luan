#pragma once

#include <memory>

#include "GameEngine.h"
#include "HelperStructs.h"
#include "ChessStructs.h"

#include "ChessBoard.h"

class DrawableChessBoard : public ChessBoard
{
public:
	DrawableChessBoard();

	~DrawableChessBoard() = default;
	DrawableChessBoard(const DrawableChessBoard& other) = delete;
	DrawableChessBoard(DrawableChessBoard&& other) noexcept = delete;
	DrawableChessBoard& operator=(const DrawableChessBoard& other) = delete;
	DrawableChessBoard& operator=(DrawableChessBoard&& other) noexcept = delete;


	void Draw(int selectedStartSquareIndex);

	Point2i GetTopLeftPos() { return m_TopLeftPos; };
	int GetCellSize() { return m_CellSize; };

private:

	Point2i m_TopLeftPos;
	int m_CellSize;

	std::unique_ptr<Bitmap> m_pTexChessBoard;

	std::unique_ptr<Bitmap> m_pTexWhitePawn_Green;
	std::unique_ptr<Bitmap> m_pTexWhiteKnight_Green;
	std::unique_ptr<Bitmap> m_pTexWhiteBishop_Green;
	std::unique_ptr<Bitmap> m_pTexWhiteRook_Green;
	std::unique_ptr<Bitmap> m_pTexWhiteQueen_Green;
	std::unique_ptr<Bitmap> m_pTexWhiteKing_Green;

	std::unique_ptr<Bitmap> m_pTexWhitePawn_Beige;
	std::unique_ptr<Bitmap> m_pTexWhiteKnight_Beige;
	std::unique_ptr<Bitmap> m_pTexWhiteBishop_Beige;
	std::unique_ptr<Bitmap> m_pTexWhiteRook_Beige;
	std::unique_ptr<Bitmap> m_pTexWhiteQueen_Beige;
	std::unique_ptr<Bitmap> m_pTexWhiteKing_Beige;


	std::unique_ptr<Bitmap> m_pTexBlackPawn_Green;
	std::unique_ptr<Bitmap> m_pTexBlackKnight_Green;
	std::unique_ptr<Bitmap> m_pTexBlackBishop_Green;
	std::unique_ptr<Bitmap> m_pTexBlackRook_Green;
	std::unique_ptr<Bitmap> m_pTexBlackQueen_Green;
	std::unique_ptr<Bitmap> m_pTexBlackKing_Green;

	std::unique_ptr<Bitmap> m_pTexBlackPawn_Beige;
	std::unique_ptr<Bitmap> m_pTexBlackKnight_Beige;
	std::unique_ptr<Bitmap> m_pTexBlackBishop_Beige;
	std::unique_ptr<Bitmap> m_pTexBlackRook_Beige;
	std::unique_ptr<Bitmap> m_pTexBlackQueen_Beige;
	std::unique_ptr<Bitmap> m_pTexBlackKing_Beige;


	void DrawBoard();
	void DrawPieces();
	void DrawPieceType(uint64_t bitBoard, Bitmap* bitmapGreen, Bitmap* bitmapBeige);
	void DrawPossibleMoves(int startSquareIndex);

	void InitializeBitmaps();
};

