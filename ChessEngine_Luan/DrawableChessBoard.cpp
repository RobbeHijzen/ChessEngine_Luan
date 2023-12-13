#include "DrawableChessBoard.h"

DrawableChessBoard::DrawableChessBoard()
{
	InitializeBitmaps();

	m_CellSize = m_pTexChessBoard->GetWidth() / 8;
	m_TopLeftPos.x = (GAME_ENGINE->GetWidth() - m_pTexChessBoard->GetWidth()) / 2;
	m_TopLeftPos.y = (GAME_ENGINE->GetHeight() - m_pTexChessBoard->GetHeight()) / 2;
}

void DrawableChessBoard::Draw(int selectedStartSquareIndex)
{
	DrawBoard();
	DrawPieces();

	DrawPossibleMoves(selectedStartSquareIndex);
}
void DrawableChessBoard::DrawBoard()
{
	GAME_ENGINE->DrawBitmap(m_pTexChessBoard.get(), m_TopLeftPos.x, m_TopLeftPos.y);
}
void DrawableChessBoard::DrawPieces()
{
	DrawPieceType(m_BitBoards.whitePawns	, m_pTexWhitePawn_Green.get()	, m_pTexWhitePawn_Beige.get()	);
	DrawPieceType(m_BitBoards.whiteKnights	, m_pTexWhiteKnight_Green.get()	, m_pTexWhiteKnight_Beige.get()	);
	DrawPieceType(m_BitBoards.whiteBishops	, m_pTexWhiteBishop_Green.get()	, m_pTexWhiteBishop_Beige.get()	);
	DrawPieceType(m_BitBoards.whiteRooks	, m_pTexWhiteRook_Green.get()	, m_pTexWhiteRook_Beige.get()	);
	DrawPieceType(m_BitBoards.whiteQueens	, m_pTexWhiteQueen_Green.get()	, m_pTexWhiteQueen_Beige.get()	);
	DrawPieceType(m_BitBoards.whiteKing		, m_pTexWhiteKing_Green.get()	, m_pTexWhiteKing_Beige.get()	);

	DrawPieceType(m_BitBoards.blackPawns	, m_pTexBlackPawn_Green.get()	, m_pTexBlackPawn_Beige.get()	);
	DrawPieceType(m_BitBoards.blackKnights	, m_pTexBlackKnight_Green.get()	, m_pTexBlackKnight_Beige.get()	);
	DrawPieceType(m_BitBoards.blackBishops	, m_pTexBlackBishop_Green.get()	, m_pTexBlackBishop_Beige.get()	);
	DrawPieceType(m_BitBoards.blackRooks	, m_pTexBlackRook_Green.get()	, m_pTexBlackRook_Beige.get()	);
	DrawPieceType(m_BitBoards.blackQueens	, m_pTexBlackQueen_Green.get()	, m_pTexBlackQueen_Beige.get()	);
	DrawPieceType(m_BitBoards.blackKing		, m_pTexBlackKing_Green.get()	, m_pTexBlackKing_Beige.get()	);

}
void DrawableChessBoard::DrawPieceType(uint64_t bitBoard, Bitmap* bitmapGreen, Bitmap* bitmapBeige)
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
			if ((index % 8 + index / 8) & 1)
			{
				GAME_ENGINE->DrawBitmap(bitmapGreen, xPos, yPos);

			}
			else
			{
				GAME_ENGINE->DrawBitmap(bitmapBeige, xPos, yPos);

			}
		}
	}
}

void DrawableChessBoard::DrawPossibleMoves(int startSquareIndex)
{
	for (auto& move : m_PossibleMoves)
	{
		if (move.startSquareIndex == startSquareIndex)
		{
			int x = m_TopLeftPos.x + move.targetSquareIndex % 8 * m_CellSize + m_CellSize / 2 - 15;
			int y = m_TopLeftPos.y + move.targetSquareIndex / 8 * m_CellSize + m_CellSize / 2 - 15;

			GAME_ENGINE->FillOval(x, y, 30, 30, 50);
		}
	}
}

void DrawableChessBoard::InitializeBitmaps()
{

	m_pTexChessBoard = std::make_unique<Bitmap>(L"Resources/CheckersBoard.bmp");

	m_pTexWhitePawn_Green	= std::make_unique<Bitmap>(L"Resources/Pieces/Pawn_White_Green.bmp");
	m_pTexWhiteKnight_Green = std::make_unique<Bitmap>(L"Resources/Pieces/Knight_White_Green.bmp");
	m_pTexWhiteBishop_Green = std::make_unique<Bitmap>(L"Resources/Pieces/Bishop_White_Green.bmp");
	m_pTexWhiteRook_Green	= std::make_unique<Bitmap>(L"Resources/Pieces/Rook_White_Green.bmp");
	m_pTexWhiteQueen_Green	= std::make_unique<Bitmap>(L"Resources/Pieces/Queen_White_Green.bmp");
	m_pTexWhiteKing_Green	= std::make_unique<Bitmap>(L"Resources/Pieces/King_White_Green.bmp");

	m_pTexWhitePawn_Beige	= std::make_unique<Bitmap>(L"Resources/Pieces/Pawn_White_Beige.bmp");
	m_pTexWhiteKnight_Beige = std::make_unique<Bitmap>(L"Resources/Pieces/Knight_White_Beige.bmp");
	m_pTexWhiteBishop_Beige = std::make_unique<Bitmap>(L"Resources/Pieces/Bishop_White_Beige.bmp");
	m_pTexWhiteRook_Beige	= std::make_unique<Bitmap>(L"Resources/Pieces/Rook_White_Beige.bmp");
	m_pTexWhiteQueen_Beige	= std::make_unique<Bitmap>(L"Resources/Pieces/Queen_White_Beige.bmp");
	m_pTexWhiteKing_Beige	= std::make_unique<Bitmap>(L"Resources/Pieces/King_White_Beige.bmp");



	m_pTexBlackPawn_Green	= std::make_unique<Bitmap>(L"Resources/Pieces/Pawn_Black_Green.bmp");
	m_pTexBlackKnight_Green = std::make_unique<Bitmap>(L"Resources/Pieces/Knight_Black_Green.bmp");
	m_pTexBlackBishop_Green = std::make_unique<Bitmap>(L"Resources/Pieces/Bishop_Black_Green.bmp");
	m_pTexBlackRook_Green	= std::make_unique<Bitmap>(L"Resources/Pieces/Rook_Black_Green.bmp");
	m_pTexBlackQueen_Green	= std::make_unique<Bitmap>(L"Resources/Pieces/Queen_Black_Green.bmp");
	m_pTexBlackKing_Green	= std::make_unique<Bitmap>(L"Resources/Pieces/King_Black_Green.bmp");

	m_pTexBlackPawn_Beige	= std::make_unique<Bitmap>(L"Resources/Pieces/Pawn_Black_Beige.bmp");
	m_pTexBlackKnight_Beige = std::make_unique<Bitmap>(L"Resources/Pieces/Knight_Black_Beige.bmp");
	m_pTexBlackBishop_Beige = std::make_unique<Bitmap>(L"Resources/Pieces/Bishop_Black_Beige.bmp");
	m_pTexBlackRook_Beige	= std::make_unique<Bitmap>(L"Resources/Pieces/Rook_Black_Beige.bmp");
	m_pTexBlackQueen_Beige	= std::make_unique<Bitmap>(L"Resources/Pieces/Queen_Black_Beige.bmp");
	m_pTexBlackKing_Beige	= std::make_unique<Bitmap>(L"Resources/Pieces/King_Black_Beige.bmp");

}
