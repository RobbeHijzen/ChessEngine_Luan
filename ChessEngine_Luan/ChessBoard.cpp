#include "ChessBoard.h"
#include <string>

ChessBoard::ChessBoard()
{
	std::string FEN{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
	SetBitboardsFromFEN(FEN);

	UpdateColorBitboards();
	CalculatePossibleMoves();
}

void ChessBoard::MakeMove(Move move)
{
	uint64_t* startBitBoard{GetBitboardFromSquare(move.startSquareIndex)};
	uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };

	if ((*startBitBoard) == 0) return;

	m_EnPassantSquares = 0;
	

	UpdateColorBitboards();
	switch (move.moveType)
	{
		case MoveType::QuietMove:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;
			break;
		}
		case MoveType::DoublePawnPush:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;
			m_EnPassantSquares |= static_cast<unsigned long long>(1) << (move.startSquareIndex + ((move.targetSquareIndex - move.startSquareIndex) / 2));
			break;
		}
		case MoveType::KingCastle:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			uint64_t* rookBitBoard{ GetBitboardFromSquare(move.targetSquareIndex + 1) };
			*rookBitBoard ^= static_cast<unsigned long long>(1) << (move.targetSquareIndex + 1);
			*rookBitBoard |= static_cast<unsigned long long>(1) << (move.targetSquareIndex - 1);
			
			break;
		}
		case MoveType::QueenCastle:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			uint64_t* rookBitBoard{ GetBitboardFromSquare(move.targetSquareIndex - 1) };
			*rookBitBoard ^= static_cast<unsigned long long>(1) << (move.targetSquareIndex - 1);
			*rookBitBoard |= static_cast<unsigned long long>(1) << (move.targetSquareIndex + 1);

			break;
		}
		case MoveType::Capture:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*targetBitBoard ^= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			break;
		}
		case MoveType::EnPassantCapture:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*targetBitBoard ^= static_cast<unsigned long long>(1) << (move.targetSquareIndex + (move.startSquareIndex - move.targetSquareIndex - 1));

			break;
		}
		case MoveType::KnightPromotion:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			
			uint64_t* knightBitBoard{(*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteKnights : &m_BitBoards.blackKnights };
			*knightBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			break;
		}
		case MoveType::BishopPromotion:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			uint64_t* bishopBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteBishops : &m_BitBoards.blackBishops };
			*bishopBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;
			break;
		}
		case MoveType::RookPromotion:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			uint64_t* rookBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteRooks : &m_BitBoards.blackRooks };
			*rookBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;
			break;
		}
		case MoveType::QueenPromotion:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			uint64_t* queenBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteQueens : &m_BitBoards.blackQueens };
			*queenBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;
			break;
		}
		case MoveType::KnightPromotionCapture:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			uint64_t* knightBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteKnights : &m_BitBoards.blackKnights };
			*knightBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*targetBitBoard ^= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			break;
		}
		case MoveType::BishopPromotionCapture:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			uint64_t* bishopBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteBishops : &m_BitBoards.blackBishops };
			*bishopBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*targetBitBoard ^= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			break;
		}
		case MoveType::RookPromotionCapture:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			uint64_t* rookBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteRooks : &m_BitBoards.blackRooks };
			*rookBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*targetBitBoard ^= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			break;
		}
		case MoveType::QueenPromotionCapture:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			uint64_t* queenBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteQueens : &m_BitBoards.blackQueens };
			*queenBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*targetBitBoard ^= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			break;
		}

	}


}

void ChessBoard::CalculatePossibleMoves()
{
	m_PossibleMoves.clear();

	CalculatePawnMoves();
	CalculateKnightMoves();
	CalculateBishopMoves();
	CalculateRookMoves();
	CalculateQueenMoves();
	CalculateKingMoves();

}

void ChessBoard::CalculatePawnMoves()
{
#pragma region BlackPawns
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.blackPawns & static_cast<unsigned long long>(1) << squareIndex)
			{
				// If the square in front has nothing (exluding Promotions)
				if (!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & static_cast<unsigned long long>(1) << (squareIndex + 8)) && squareIndex < 48)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + 8;
					move.moveType = MoveType::QuietMove;

					m_PossibleMoves.push_back(move);



					// Double Pawn Push
					if (!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & static_cast<unsigned long long>(1) << (squareIndex + 16)) && squareIndex < 16)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = squareIndex + 16;
						move.moveType = MoveType::DoublePawnPush;

						m_PossibleMoves.push_back(move);
					}
				}
				//-----------------------------------
				// 
				// Promotion
				if (!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & static_cast<unsigned long long>(1) << (squareIndex + 8)) && squareIndex > 47)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + 8;
					move.moveType = MoveType::QueenPromotion;

					m_PossibleMoves.push_back(move);
				}
				//-----------------------------------
				//
				// Side left Capture (including Promotion Captures)
				if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << (squareIndex + 7) && squareIndex % 8 != 0 && squareIndex < 56)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + 7;
					if (squareIndex > 47)
						move.moveType = MoveType::QueenPromotionCapture;
					else 
						move.moveType = MoveType::Capture;
					

					m_PossibleMoves.push_back(move);
				}
				//-----------------------------------
				//
				// Side right Capture (including Promotion Captures)
				if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << (squareIndex + 9) && squareIndex % 8 != 7 && squareIndex < 56)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + 9;
					if (squareIndex > 47)
						move.moveType = MoveType::QueenPromotionCapture;
					else
						move.moveType = MoveType::Capture;

					m_PossibleMoves.push_back(move);
				}
				//-----------------------------------
				//
				// En Passant Left
				if (m_EnPassantSquares & static_cast<unsigned long long>(1) << (squareIndex + 7) && squareIndex % 8 != 0 && squareIndex < 56)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + 7;
					move.moveType = MoveType::EnPassantCapture;

					m_PossibleMoves.push_back(move);
				}
				//-----------------------------------
				//
				// En Passant Right
				if (m_EnPassantSquares & static_cast<unsigned long long>(1) << (squareIndex + 9) && squareIndex % 8 != 7 && squareIndex < 56)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + 9;
					move.moveType = MoveType::EnPassantCapture;

					m_PossibleMoves.push_back(move);
				}
			}
		}
	}
#pragma endregion

#pragma region WhitePawns
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.whitePawns & static_cast<unsigned long long>(1) << squareIndex)
			{
				// If the square in front has nothing (exluding Promotions)
				if (!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & static_cast<unsigned long long>(1) << (squareIndex - 8)) && squareIndex > 15)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex - 8;
					move.moveType = MoveType::QuietMove;

					m_PossibleMoves.push_back(move);



					// Double Pawn Push
					if (!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & static_cast<unsigned long long>(1) << (squareIndex - 16)) && squareIndex > 47)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = squareIndex - 16;
						move.moveType = MoveType::DoublePawnPush;

						m_PossibleMoves.push_back(move);
					}
				}
				//-----------------------------------
				// 
				// Promotion
				if (!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & static_cast<unsigned long long>(1) << (squareIndex - 8)) && squareIndex < 16)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex - 8;
					move.moveType = MoveType::QueenPromotion;

					m_PossibleMoves.push_back(move);
				}
				//-----------------------------------
				//
				// Side left Capture (includes promotion captures)
				if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << (squareIndex - 9) && squareIndex % 8 != 0 && squareIndex > 7)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex - 9;
					if (squareIndex < 16)
						move.moveType = MoveType::QueenPromotionCapture;
					else
						move.moveType = MoveType::Capture;

					m_PossibleMoves.push_back(move);
				}
				//-----------------------------------
				//
				// Side right Capture (includes promotion captures)
				if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << (squareIndex - 7) && squareIndex % 8 != 7 && squareIndex > 7)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex - 7;
					if (squareIndex < 16)
						move.moveType = MoveType::QueenPromotionCapture;
					else
						move.moveType = MoveType::Capture;

					m_PossibleMoves.push_back(move);
				}
				//-----------------------------------
				//
				// En Passant Left
				if (m_EnPassantSquares & static_cast<unsigned long long>(1) << (squareIndex - 9) && squareIndex % 8 != 0 && squareIndex > 7)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex - 9;
					move.moveType = MoveType::EnPassantCapture;

					m_PossibleMoves.push_back(move);
				}
				//-----------------------------------
				//
				// En Passant Right
				if (m_EnPassantSquares & static_cast<unsigned long long>(1) << (squareIndex - 7) && squareIndex % 8 != 7 && squareIndex > 7)
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex - 7;
					move.moveType = MoveType::EnPassantCapture;

					m_PossibleMoves.push_back(move);
				}
			}
		}
	}
#pragma endregion
}
void ChessBoard::CalculateKnightMoves()
{
#pragma region BlackKnights
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if(m_BitBoards.blackKnights & static_cast<unsigned long long>(1) << squareIndex)
			{ 
				//West-North-West
				if (squareIndex % 8 >= 2 && squareIndex > 7)
				{
					int newSquareIndex{squareIndex - 10};

					if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//North-North-West
				if (squareIndex % 8 >= 1 && squareIndex > 15)
				{
					int newSquareIndex{ squareIndex - 17 };

					if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//North-North-East
				if (squareIndex % 8 <= 6 && squareIndex > 15)
				{
					int newSquareIndex{ squareIndex - 15 };

					if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//East-North-East
				if (squareIndex % 8 <= 5 && squareIndex > 7)
				{
					int newSquareIndex{ squareIndex - 6 };

					if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//East-South-East
				if (squareIndex % 8 <= 5 && squareIndex < 56)
				{
					int newSquareIndex{ squareIndex + 10 };

					if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//South-South-East
				if (squareIndex % 8 <= 6 && squareIndex < 48)
				{
					int newSquareIndex{ squareIndex + 17 };

					if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//South-South-West
				if (squareIndex % 8 >= 1 && squareIndex < 48)
				{
					int newSquareIndex{ squareIndex + 15 };

					if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//West-South-West
				if (squareIndex % 8 >= 2 && squareIndex < 56)
				{
					int newSquareIndex{ squareIndex + 6 };

					if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
			}
		}
	}
#pragma endregion

#pragma region WhiteKnights
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.whiteKnights & static_cast<unsigned long long>(1) << squareIndex)
			{
				//West-North-West
				if (squareIndex % 8 >= 2 && squareIndex > 7)
				{
					int newSquareIndex{ squareIndex - 10 };

					if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//North-North-West
				if (squareIndex % 8 >= 1 && squareIndex > 15)
				{
					int newSquareIndex{ squareIndex - 17 };

					if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//North-North-East
				if (squareIndex % 8 <= 6 && squareIndex > 15)
				{
					int newSquareIndex{ squareIndex - 15 };

					if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//East-North-East
				if (squareIndex % 8 <= 5 && squareIndex > 7)
				{
					int newSquareIndex{ squareIndex - 6 };

					if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//East-South-East
				if (squareIndex % 8 <= 5 && squareIndex < 56)
				{
					int newSquareIndex{ squareIndex + 10 };

					if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//South-South-East
				if (squareIndex % 8 <= 6 && squareIndex < 48)
				{
					int newSquareIndex{ squareIndex + 17 };

					if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//South-South-West
				if (squareIndex % 8 >= 1 && squareIndex < 48)
				{
					int newSquareIndex{ squareIndex + 15 };

					if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
				//West-South-West
				if (squareIndex % 8 >= 2 && squareIndex < 56)
				{
					int newSquareIndex{ squareIndex + 6 };

					if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.push_back(move);
					}
					else if (~m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.push_back(move);
					}
				}
			}
		}
	}
#pragma endregion
}
void ChessBoard::CalculateBishopMoves()
{
	std::vector<int> directionOffsets{ -9, -7, +9, +7 };
	std::vector<int> directionLengths{ 0, 0, 0, 0 };

#pragma region BlackBishops
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.blackBishops & static_cast<unsigned long long>(1) << squareIndex)
			{ 
				int distanceFromLeft = squareIndex % 8;
				int distanceFromUp = squareIndex / 8;

				directionLengths[0] = std::min(distanceFromLeft, distanceFromUp);
				directionLengths[1] = std::min(7 - distanceFromLeft, distanceFromUp);
				directionLengths[2] = std::min(7 - distanceFromLeft, 7 - distanceFromUp);
				directionLengths[3] = std::min(distanceFromLeft, 7 - distanceFromUp);

				for (int directionIndex{}; directionIndex < directionOffsets.size(); ++directionIndex)
				{
					int directionOffset{ directionOffsets[directionIndex] };

					for(int multipliedIndex{1}; multipliedIndex < directionLengths[directionIndex]; ++multipliedIndex)
					{
						if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << (squareIndex + directionOffset * multipliedIndex))
						{
							break;
						}
						else if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << (squareIndex + directionOffset * multipliedIndex))
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + directionOffset * multipliedIndex;
							move.moveType = MoveType::Capture;

							m_PossibleMoves.push_back(move);
							break;
						}
						else
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + directionOffset * multipliedIndex;
							move.moveType = MoveType::QuietMove;

							m_PossibleMoves.push_back(move);
							continue;
						}
					}
				}
			}
		}
	}
#pragma endregion

#pragma region WhiteBishops
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.whiteBishops & static_cast<unsigned long long>(1) << squareIndex)
			{
				int distanceFromLeft = squareIndex % 8;
				int distanceFromUp = squareIndex / 8;

				directionLengths[0] = std::min(distanceFromLeft, distanceFromUp);
				directionLengths[1] = std::min(7 - distanceFromLeft, distanceFromUp);
				directionLengths[2] = std::min(7 - distanceFromLeft, 7 - distanceFromUp);
				directionLengths[3] = std::min(distanceFromLeft, 7 - distanceFromUp);

				for (int directionIndex{}; directionIndex < directionOffsets.size(); ++directionIndex)
				{
					int directionOffset{ directionOffsets[directionIndex] };

					for (int multipliedIndex{ 1 }; multipliedIndex < directionLengths[directionIndex]; ++multipliedIndex)
					{
						if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << (squareIndex + directionOffset * multipliedIndex))
						{
							break;
						}
						else if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << (squareIndex + directionOffset * multipliedIndex))
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + directionOffset * multipliedIndex;
							move.moveType = MoveType::Capture;

							m_PossibleMoves.push_back(move);
							break;
						}
						else
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + directionOffset * multipliedIndex;
							move.moveType = MoveType::QuietMove;

							m_PossibleMoves.push_back(move);
							continue;
						}
					}
				}
			}
		}
	}
#pragma endregion
}
void ChessBoard::CalculateRookMoves()
{
}
void ChessBoard::CalculateQueenMoves()
{
}
void ChessBoard::CalculateKingMoves()
{
}

void ChessBoard::SetBitboardsFromFEN(std::string FEN)
{
	FENFields currentFENField{ FENFields::PiecePlacement};
	int currentSquareIndex{};

	for(int index{}; index < FEN.size(); ++index)
	{
		char currentChar{ FEN[index] };
		if (currentChar == ' ')
		{
			currentFENField = FENFields(int(currentFENField) + 1);
			continue;
		}

		switch(currentFENField)
		{ 
			case FENFields::PiecePlacement:
			{
				SetPositionFromChar(currentChar, currentSquareIndex);
				break;
			}
			case FENFields::SideToMove:
			{
				SetSideToMoveFromChar(currentChar);
				break;
			}
			case FENFields::CastlingAbility:
			{
				SetCastlingRulesFromChar(currentChar);
				break;
			}
			case FENFields::EnPassentTargetSquare:
			{
				SetEnPassantSquaresFromChars(currentChar, FEN[index + 1], index);
				break;
			}
			case FENFields::HalfmoveClock:
			{
				m_HalfMoveClock = currentChar - '0';
				break;
			}
			case FENFields::FullmoveCounter:
			{
				m_FullMoveCounter = currentChar - '0';
				break;
			}
			default:
				return;
		}
	}
}

void ChessBoard::SetPositionFromChar(char c, int& squareIndex)
{
	//if (c == '/')
	//{
	//	squareIndex = (int(squareIndex / 8) + 1) * 8;
	//}
	if (c >= '1' && c <= '8')
	{
		squareIndex += c - '0';
	}

	switch (c)
	{
		case 'P':
		{
			m_BitBoards.whitePawns |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'p':
		{
			m_BitBoards.blackPawns |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'N':
		{
			m_BitBoards.whiteKnights |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'n':
		{
			m_BitBoards.blackKnights |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'B':
		{
			m_BitBoards.whiteBishops |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'b':
		{
			m_BitBoards.blackBishops |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'R':
		{
			m_BitBoards.whiteRooks |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'r':
		{
			m_BitBoards.blackRooks |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'Q':
		{
			m_BitBoards.whiteQueens |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'q':
		{
			m_BitBoards.blackQueens |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'K':
		{
			m_BitBoards.whiteKing |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}
		case 'k':
		{
			m_BitBoards.blackKing |= static_cast<unsigned long long>(1) << squareIndex;
			break;
		}

		default:
			return;
	}
	squareIndex += 1;
}
void ChessBoard::SetSideToMoveFromChar(char c)
{
	if (c == 'w')
	{
		m_WhiteToMove = true;
	}
	else
	{
		m_WhiteToMove = false;
	}
}
void ChessBoard::SetCastlingRulesFromChar(char c)
{
	switch (c)
	{
		case 'K':
		{
			m_WhiteCanCastleKingSide = true;
			break;
		}
		case 'Q':
		{
			m_WhiteCanCastleQueenSide = true;
			break;
		}
		case 'k':
		{
			m_BlackCanCastleKingSide = true;
			break;
		}
		case 'q':
		{
			m_BlackCanCastleQueenSide = true;
			break;
		}
		default:
			break;
	}
}
void ChessBoard::SetEnPassantSquaresFromChars(char file, char rank, int& index)
{
	if (file < 'a' || file > 'i') return;
	if (rank < '1' || rank > '8') return;


	int fileIndex{ file - 'a' };
	int rankIndex{ 8 - (rank - '0') };

	int squareIndex{ fileIndex + 8 * rankIndex };

	m_EnPassantSquares |= static_cast<unsigned long long>(1) << squareIndex;
	index += 1; // The char is in notation "e3" which is 2 characters that have to be checked at the same time
}

uint64_t* ChessBoard::GetBitboardFromSquare(int squareIndex)
{
	if (m_BitBoards.whitePawns & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.whitePawns;
	if (m_BitBoards.whiteKnights & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.whiteKnights;
	if (m_BitBoards.whiteBishops & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.whiteBishops;
	if (m_BitBoards.whiteRooks & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.whiteRooks;
	if (m_BitBoards.whiteQueens & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.whiteQueens;
	if (m_BitBoards.whiteKing & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.whiteKing;

	if (m_BitBoards.blackPawns & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.blackPawns;
	if (m_BitBoards.blackKnights & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.blackKnights;
	if (m_BitBoards.blackBishops & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.blackBishops;
	if (m_BitBoards.blackRooks & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.blackRooks;
	if (m_BitBoards.blackQueens & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.blackQueens;
	if (m_BitBoards.blackKing & static_cast<unsigned long long>(1) << squareIndex) return &m_BitBoards.blackKing;

	return &m_BitBoards.nullBitBoard;
}

void ChessBoard::UpdateColorBitboards()
{
	m_BitBoards.blackPieces = m_BitBoards.blackPawns | m_BitBoards.blackKnights | m_BitBoards.blackBishops | m_BitBoards.blackRooks | m_BitBoards.blackQueens | m_BitBoards.blackKing;
	m_BitBoards.whitePieces = m_BitBoards.whitePawns | m_BitBoards.whiteKnights | m_BitBoards.whiteBishops | m_BitBoards.whiteRooks | m_BitBoards.whiteQueens | m_BitBoards.whiteKing;
}

