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


	void Draw();

	Point2i GetTopLeftPos() { return m_TopLeftPos; };
	int GetCellSize() { return m_CellSize; };

private:

	Point2i m_TopLeftPos;
	int m_CellSize;

	std::unique_ptr<Bitmap> m_pTexChessBoard;

	std::unique_ptr<Bitmap> m_pTexWhitePawn;
	std::unique_ptr<Bitmap> m_pTexWhiteKnight;
	std::unique_ptr<Bitmap> m_pTexWhiteBishop;
	std::unique_ptr<Bitmap> m_pTexWhiteRook;
	std::unique_ptr<Bitmap> m_pTexWhiteQueen;
	std::unique_ptr<Bitmap> m_pTexWhiteKing;

	std::unique_ptr<Bitmap> m_pTexBlackPawn;
	std::unique_ptr<Bitmap> m_pTexBlackKnight;
	std::unique_ptr<Bitmap> m_pTexBlackBishop;
	std::unique_ptr<Bitmap> m_pTexBlackRook;
	std::unique_ptr<Bitmap> m_pTexBlackQueen;
	std::unique_ptr<Bitmap> m_pTexBlackKing;

	void DrawBoard();
	void DrawPieces();
	void DrawPieceType(uint64_t bitBoard, Bitmap* bitmap);

	void InitializeBitmaps();
};

