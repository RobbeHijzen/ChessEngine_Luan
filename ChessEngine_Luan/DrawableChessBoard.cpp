#include "DrawableChessBoard.h"

DrawableChessBoard::DrawableChessBoard()
{
	InitializeBitmaps();

	m_CellSize = m_pTexChessBoard->GetWidth() / 8;
	m_TopLeftPos.x = (GAME_ENGINE->GetWidth() - m_pTexChessBoard->GetWidth()) / 2;
	m_TopLeftPos.y = (GAME_ENGINE->GetHeight() - m_pTexChessBoard->GetHeight()) / 2;
}

void DrawableChessBoard::Draw()
{
	DrawBoard();
	DrawPieces();
}
void DrawableChessBoard::DrawBoard()
{
	GAME_ENGINE->DrawBitmap(m_pTexChessBoard.get(), m_TopLeftPos.x, m_TopLeftPos.y);
}
void DrawableChessBoard::DrawPieces()
{
	DrawPieceType(m_BitBoards.whitePawns, m_pTexWhitePawn.get());
	DrawPieceType(m_BitBoards.whiteKnights, m_pTexWhiteKnight.get());
	DrawPieceType(m_BitBoards.whiteBishops, m_pTexWhiteBishop.get());
	DrawPieceType(m_BitBoards.whiteRooks, m_pTexWhiteRook.get());
	DrawPieceType(m_BitBoards.whiteQueens, m_pTexWhiteQueen.get());
	DrawPieceType(m_BitBoards.whiteKing, m_pTexWhiteKing.get());

	DrawPieceType(m_BitBoards.blackPawns, m_pTexBlackPawn.get());
	DrawPieceType(m_BitBoards.blackKnights, m_pTexBlackKnight.get());
	DrawPieceType(m_BitBoards.blackBishops, m_pTexBlackBishop.get());
	DrawPieceType(m_BitBoards.blackRooks, m_pTexBlackRook.get());
	DrawPieceType(m_BitBoards.blackQueens, m_pTexBlackQueen.get());
	DrawPieceType(m_BitBoards.blackKing, m_pTexBlackKing.get());

}
void DrawableChessBoard::DrawPieceType(uint64_t bitBoard, Bitmap* bitmap)
{
	int xPos{m_TopLeftPos.x};
	int yPos{m_TopLeftPos.y - m_CellSize};

	for (int index{}; index < 64; ++index)
	{
		if (index % 8 == 0)
		{
			xPos = m_TopLeftPos.x;
			yPos += m_CellSize;
		}
		else
		{
			xPos += m_CellSize;
		}

		uint64_t mask{ static_cast<unsigned long long>(1) << index };
		if (bitBoard & mask)
		{
			GAME_ENGINE->DrawBitmap(bitmap, xPos, yPos);
		}
	}
}

void DrawableChessBoard::InitializeBitmaps()
{

	m_pTexChessBoard = std::make_unique<Bitmap>(L"Resources/CheckersBoard.bmp");

	m_pTexWhitePawn = std::make_unique<Bitmap>(L"Resources/Pieces/Pawn_White.bmp");
	m_pTexWhiteKnight = std::make_unique<Bitmap>(L"Resources/Pieces/Knight_White.bmp");
	m_pTexWhiteBishop = std::make_unique<Bitmap>(L"Resources/Pieces/Bishop_White.bmp");
	m_pTexWhiteRook = std::make_unique<Bitmap>(L"Resources/Pieces/Rook_White.bmp");
	m_pTexWhiteQueen = std::make_unique<Bitmap>(L"Resources/Pieces/Queen_White.bmp");
	m_pTexWhiteKing = std::make_unique<Bitmap>(L"Resources/Pieces/King_White.bmp");

	m_pTexBlackPawn = std::make_unique<Bitmap>(L"Resources/Pieces/Pawn_Black.bmp");
	m_pTexBlackKnight = std::make_unique<Bitmap>(L"Resources/Pieces/Knight_Black.bmp");
	m_pTexBlackBishop = std::make_unique<Bitmap>(L"Resources/Pieces/Bishop_Black.bmp");
	m_pTexBlackRook = std::make_unique<Bitmap>(L"Resources/Pieces/Rook_Black.bmp");
	m_pTexBlackQueen = std::make_unique<Bitmap>(L"Resources/Pieces/Queen_Black.bmp");
	m_pTexBlackKing = std::make_unique<Bitmap>(L"Resources/Pieces/King_Black.bmp");

}
