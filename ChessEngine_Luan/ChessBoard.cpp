#include "ChessBoard.h"
#include "GameEngine.h"
#include <string>

ChessBoard::ChessBoard()
{
	//std::string FEN{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
	//std::string FEN{ "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq 0 0" }; // Position 2
	//std::string FEN{ "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - 0 0" }; // Position 3
	std::string FEN{ "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1" }; // Position 4
	//std::string FEN{ "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8" }; // Position 5
	//std::string FEN{ "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10" }; // Position 6
	SetBitboardsFromFEN(FEN);
	
	UpdateColorBitboards();
	CalculatePossibleMoves(true);

	UpdateGameStateHistory();
}

int ChessBoard::MoveGenerationTest(int depth)
{
	if (depth == 0) return 1;

	int positionsCounter{};
	for (int index{}; index < m_PossibleMoves.size(); ++index)
	{

		auto it{ m_PossibleMoves.begin() };
		std::advance(it, index);
		Move move{ *it };

		if (move.moveType == MoveType::Capture || move.moveType == MoveType::BishopPromotionCapture || move.moveType == MoveType::KnightPromotionCapture || move.moveType == MoveType::RookPromotionCapture || move.moveType == MoveType::QueenPromotionCapture)
			++m_CaptureAmount;

		else if (move.moveType == MoveType::EnPassantCaptureLeft || move.moveType == MoveType::EnPassantCaptureRight)
			++m_EnPassantAmount;

		else if (move.moveType == MoveType::KingCastle || move.moveType == MoveType::QueenCastle)
			++m_CastleAmount;

		else if (move.moveType == MoveType::BishopPromotion || move.moveType == MoveType::KnightPromotion || move.moveType == MoveType::RookPromotion || move.moveType == MoveType::QueenPromotion || move.moveType == MoveType::BishopPromotionCapture || move.moveType == MoveType::KnightPromotionCapture || move.moveType == MoveType::RookPromotionCapture || move.moveType == MoveType::QueenPromotionCapture)
			++m_PromotionAmount;

		MakeMove(move, true, false);
		positionsCounter += MoveGenerationTest(depth - 1);
		UnMakeLastMove();
	}

	return positionsCounter;
}

void ChessBoard::MakeMove(Move move, bool originalBoard, bool canEndGame)
{
	m_WhiteToMove = !m_WhiteToMove;

	if(m_WhiteToMove) ++m_HalfMoveClock;
	++m_FullMoveCounter;
	
	m_EnPassantSquares = 0;

	uint64_t* startBitBoard{GetBitboardFromSquare(move.startSquareIndex)};
	CheckCastleRights(*startBitBoard, move.startSquareIndex);
	
	UpdateBitBoards(move, startBitBoard);
	CalculatePossibleMoves(originalBoard);
	if(canEndGame)
		CheckForGameEnd();

	UpdateGameStateHistory();
}

void ChessBoard::UnMakeLastMove()
{
	if (m_GameStateHistory.size() <= 1) return;

	std::vector<GameState>::iterator it{ m_GameStateHistory.end() };

	//GameState gameState = m_GameStateHistory.back();
	--it;
	--it;
	GameState gameState = *it;

	m_GameStateHistory.pop_back();

	m_BitBoards = gameState.bitBoards;
	m_PossibleMoves = gameState.possibleMoves;

	m_WhiteToMove = gameState.whiteToMove;

	m_WhiteCanCastleKingSide = gameState.whiteCanCastleKingSide;
	m_WhiteCanCastleQueenSide = gameState.whiteCanCastleQueenSide;
	m_BlackCanCastleKingSide = gameState.blackCanCastleKingSide;
	m_BlackCanCastleQueenSide = gameState.blackCanCastleQueenSide;

	m_EnPassantSquares = gameState.enPassantSquares;
	m_HalfMoveClock = gameState.halfMoveClock;
	m_FullMoveCounter = gameState.fullMoveCounter;

}


void ChessBoard::UpdateBitBoards(Move move, uint64_t* startBitBoard)
{

	switch (move.moveType)
	{
		case MoveType::QuietMove:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			if (*startBitBoard == m_BitBoards.whitePawns || *startBitBoard == m_BitBoards.blackPawns) m_HalfMoveClock = 0;

			break;
		}
		case MoveType::DoublePawnPush:
		{
			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;
			m_EnPassantSquares |= static_cast<unsigned long long>(1) << (move.startSquareIndex + ((move.targetSquareIndex - move.startSquareIndex) / 2));

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::KingCastle:
		{
			uint64_t* rookBitBoard{ GetBitboardFromSquare(move.targetSquareIndex + 1) };
			*rookBitBoard ^= static_cast<unsigned long long>(1) << (move.targetSquareIndex + 1);
			*rookBitBoard |= static_cast<unsigned long long>(1) << (move.targetSquareIndex - 1);

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			break;
		}
		case MoveType::QueenCastle:
		{
			uint64_t* rookBitBoard{ GetBitboardFromSquare(move.targetSquareIndex - 2) };
			*rookBitBoard ^= static_cast<unsigned long long>(1) << (move.targetSquareIndex - 2);
			*rookBitBoard |= static_cast<unsigned long long>(1) << (move.targetSquareIndex + 1);

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			break;
		}
		case MoveType::Capture:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };
			*targetBitBoard ^= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::EnPassantCaptureLeft:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare((move.targetSquareIndex + (move.startSquareIndex - move.targetSquareIndex - 1))) };
			*targetBitBoard ^= static_cast<unsigned long long>(1) << (move.targetSquareIndex + (move.startSquareIndex - move.targetSquareIndex - 1));


			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::EnPassantCaptureRight:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare((move.targetSquareIndex + (move.startSquareIndex - move.targetSquareIndex + 1))) };
			*targetBitBoard ^= static_cast<unsigned long long>(1) << (move.targetSquareIndex + (move.startSquareIndex - move.targetSquareIndex + 1));


			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;
			*startBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::KnightPromotion:
		{
			uint64_t* knightBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteKnights : &m_BitBoards.blackKnights };
			*knightBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::BishopPromotion:
		{
			uint64_t* bishopBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteBishops : &m_BitBoards.blackBishops };
			*bishopBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::RookPromotion:
		{
			uint64_t* rookBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteRooks : &m_BitBoards.blackRooks };
			*rookBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::QueenPromotion:
		{
			uint64_t* queenBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteQueens : &m_BitBoards.blackQueens };
			*queenBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::KnightPromotionCapture:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };
			*targetBitBoard ^= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			uint64_t* knightBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteKnights : &m_BitBoards.blackKnights };
			*knightBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::BishopPromotionCapture:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };
			*targetBitBoard ^= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			uint64_t* bishopBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteBishops : &m_BitBoards.blackBishops };
			*bishopBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::RookPromotionCapture:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };
			*targetBitBoard ^= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			uint64_t* rookBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteRooks : &m_BitBoards.blackRooks };
			*rookBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::QueenPromotionCapture:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };
			*targetBitBoard ^= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			uint64_t* queenBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteQueens : &m_BitBoards.blackQueens };
			*queenBitBoard |= static_cast<unsigned long long>(1) << move.targetSquareIndex;

			*startBitBoard ^= static_cast<unsigned long long>(1) << move.startSquareIndex;

			m_HalfMoveClock = 0;
			break;
		}

	}

	UpdateColorBitboards();
}

void ChessBoard::CheckCastleRights(uint64_t startSquareBitBoard, int startSquareIndex)
{
	if (!(m_WhiteCanCastleKingSide || m_WhiteCanCastleQueenSide || m_BlackCanCastleKingSide || m_BlackCanCastleQueenSide)) return;

	if (startSquareBitBoard == m_BitBoards.whiteKing) m_WhiteCanCastleKingSide = m_WhiteCanCastleQueenSide = false;
	if (startSquareBitBoard == m_BitBoards.blackKing) m_BlackCanCastleKingSide = m_BlackCanCastleQueenSide = false;


	
	if (startSquareBitBoard == m_BitBoards.whiteRooks)
	{
		if(startSquareIndex % 8 == 7) m_WhiteCanCastleKingSide = false;
		if(startSquareIndex % 8 == 0) m_WhiteCanCastleQueenSide = false;
	}
	if (startSquareBitBoard == m_BitBoards.blackRooks)
	{
		if (startSquareIndex % 8 == 7) m_BlackCanCastleKingSide = false;
		if (startSquareIndex % 8 == 0) m_BlackCanCastleQueenSide = false;
	}
	

}
void ChessBoard::UpdateColorBitboards()
{
	m_BitBoards.blackPieces = m_BitBoards.blackPawns | m_BitBoards.blackKnights | m_BitBoards.blackBishops | m_BitBoards.blackRooks | m_BitBoards.blackQueens | m_BitBoards.blackKing;
	m_BitBoards.whitePieces = m_BitBoards.whitePawns | m_BitBoards.whiteKnights | m_BitBoards.whiteBishops | m_BitBoards.whiteRooks | m_BitBoards.whiteQueens | m_BitBoards.whiteKing;
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


bool ChessBoard::IsLegalMove(Move _move)
{
	for (auto& move : m_PossibleMoves)
	{
		if (move == _move)
		{
			return true;
		}
	}
	return false;
}
Move ChessBoard::GetMoveFromSquares(int startSquare, int targetSquare)
{
	for (auto& move : m_PossibleMoves)
	{
		if (move.startSquareIndex == startSquare && move.targetSquareIndex == targetSquare)
		{
			return move;
		}
	}

	return Move();
}




void ChessBoard::CalculatePossibleMoves(bool originalBoard)
{
	m_PossibleMoves.clear();

	CalculatePawnMoves();
	CalculateKnightMoves();
	CalculateBishopMoves();
	CalculateRookMoves();
	CalculateQueenMoves();
	CalculateKingMoves(originalBoard);

	if(originalBoard)
		CheckForIllegalMoves();
}

void ChessBoard::CalculatePawnMoves()
{
	uint64_t pawnBitBoard{ m_WhiteToMove ? m_BitBoards.whitePawns : m_BitBoards.blackPawns };
	uint64_t opponentBitBoard{ m_WhiteToMove ? m_BitBoards.blackPieces : m_BitBoards.whitePieces };

	int verticalOffset{m_WhiteToMove ? -8 : 8};

	
	for (int squareIndex{}; squareIndex < 64; ++squareIndex)
	{
		if (pawnBitBoard & static_cast<unsigned long long>(1) << squareIndex)
		{
			// If the square in front has nothing (exluding Promotions)
			{
				bool onValidRow{ m_WhiteToMove ? squareIndex > 15 : squareIndex < 48 };
				if (onValidRow &&
					!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & static_cast<unsigned long long>(1) << (squareIndex + verticalOffset)))
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + verticalOffset;
					move.moveType = MoveType::QuietMove;

					m_PossibleMoves.emplace_back(move);



					// Double Pawn Push
					bool onValidDoublePushRow{ m_WhiteToMove ? squareIndex > 47 : squareIndex < 16 };
					if (onValidDoublePushRow &&
						!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & static_cast<unsigned long long>(1) << (squareIndex + 2 * verticalOffset)))
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = squareIndex + 2 * verticalOffset;
						move.moveType = MoveType::DoublePawnPush;

						m_PossibleMoves.emplace_back(move);
					}
				}
			}
			//-----------------------------------
			// 
			// Promotion
			{
				bool onValidRow{ m_WhiteToMove ? squareIndex < 16 : squareIndex > 47 };
				if (onValidRow &&
					!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & static_cast<unsigned long long>(1) << (squareIndex + verticalOffset)))
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + verticalOffset;

					move.moveType = MoveType::QueenPromotion;
					m_PossibleMoves.emplace_back(move);

					move.moveType = MoveType::RookPromotion;
					m_PossibleMoves.emplace_back(move);

					move.moveType = MoveType::BishopPromotion;
					m_PossibleMoves.emplace_back(move);

					move.moveType = MoveType::KnightPromotion;
					m_PossibleMoves.emplace_back(move);
				}
			}
			//-----------------------------------
			//
			// Side left Capture (including Promotion Captures)
			{
				bool onValidSquare{ squareIndex % 8 != 0 };
				if (onValidSquare && 
					opponentBitBoard & static_cast<unsigned long long>(1) << (squareIndex + verticalOffset - 1))
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + verticalOffset - 1;
					if (m_WhiteToMove ? squareIndex < 16 : squareIndex > 47)
					{
						move.moveType = MoveType::QueenPromotionCapture;
						m_PossibleMoves.emplace_back(move);

						move.moveType = MoveType::RookPromotionCapture;
						m_PossibleMoves.emplace_back(move);

						move.moveType = MoveType::BishopPromotionCapture;
						m_PossibleMoves.emplace_back(move);

						move.moveType = MoveType::KnightPromotionCapture;
						m_PossibleMoves.emplace_back(move);
					}
					else
					{
						move.moveType = MoveType::Capture;
						m_PossibleMoves.emplace_back(move);
					}
				}
			}
			//-----------------------------------
			//
			// Side right Capture (including Promotion Captures)
			{
				bool onValidSquare{ squareIndex % 8 != 7 };
				if (onValidSquare &&
					opponentBitBoard & static_cast<unsigned long long>(1) << (squareIndex + verticalOffset + 1))
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + verticalOffset + 1;
					if (m_WhiteToMove ? squareIndex < 16 : squareIndex > 47)
					{
						move.moveType = MoveType::QueenPromotionCapture;
						m_PossibleMoves.emplace_back(move);

						move.moveType = MoveType::RookPromotionCapture;
						m_PossibleMoves.emplace_back(move);

						move.moveType = MoveType::BishopPromotionCapture;
						m_PossibleMoves.emplace_back(move);

						move.moveType = MoveType::KnightPromotionCapture;
						m_PossibleMoves.emplace_back(move);
					}
					else
					{
						move.moveType = MoveType::Capture;
						m_PossibleMoves.emplace_back(move);
					}
				}
			}
			//-----------------------------------
			//
			// En Passant Left
			{
				bool onValidSquare{squareIndex % 8 != 0};
				if (onValidSquare && 
					m_EnPassantSquares & static_cast<unsigned long long>(1) << (squareIndex + verticalOffset - 1))
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + verticalOffset - 1;
					move.moveType = MoveType::EnPassantCaptureLeft;

					m_PossibleMoves.emplace_back(move);
				}
			}
			//-----------------------------------
			//
			// En Passant Right
			{
				bool onValidSquare{ squareIndex % 8 != 7 };
				if (onValidSquare &&
					m_EnPassantSquares & static_cast<unsigned long long>(1) << (squareIndex + verticalOffset + 1))
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + verticalOffset + 1;
					move.moveType = MoveType::EnPassantCaptureRight;

					m_PossibleMoves.emplace_back(move);
				}
			}
		}
	}
	
}
void ChessBoard::CalculateKnightMoves()
{
	uint64_t knightsBitBoard{ m_WhiteToMove ? m_BitBoards.whiteKnights : m_BitBoards.blackKnights };
	uint64_t ownBitBoard{ m_WhiteToMove ? m_BitBoards.whitePieces : m_BitBoards.blackPieces };
	uint64_t opponentBitBoard{ m_WhiteToMove ? m_BitBoards.blackPieces : m_BitBoards.whitePieces };

	
	
	for (int squareIndex{}; squareIndex < 64; ++squareIndex)
	{
		if(knightsBitBoard & static_cast<unsigned long long>(1) << squareIndex)
		{ 
			for (int index{}; index < m_KnightOffsets.squareOffsets.size(); ++index)
			{
				int newSquareIndex{ squareIndex + m_KnightOffsets.squareOffsets[index] };

				int leftBound{ m_KnightOffsets.leftBounds[index] };
				int northBound{ m_KnightOffsets.northBounds[index] };
				int rightBound{ m_KnightOffsets.rightBounds[index] };
				int southBound{ m_KnightOffsets.southBounds[index] };

				if (squareIndex % 8 >= leftBound && squareIndex / 8 >= northBound && squareIndex % 8 <= 7 - rightBound && squareIndex / 8 <= 7 - southBound)
				{
					if (opponentBitBoard & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.emplace_back(move);
					}
					else if (~ownBitBoard & static_cast<unsigned long long>(1) << newSquareIndex)
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = newSquareIndex;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.emplace_back(move);
					}
				}
			}


			
		}
	}
}
void ChessBoard::CalculateBishopMoves()
{
	std::vector<int> directionOffsets{ -9, -7, +9, +7 };
	std::vector<int> directionLengths{ 0, 0, 0, 0 };

#pragma region BlackBishops
	if (!m_WhiteToMove)
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.blackBishops & static_cast<unsigned long long>(1) << squareIndex)
			{ 
				int distanceFromLeft = squareIndex % 8;
				int distanceFromUp = squareIndex / 8;

				directionLengths[0] = min(distanceFromLeft, distanceFromUp);
				directionLengths[1] = min(7 - distanceFromLeft, distanceFromUp);
				directionLengths[2] = min(7 - distanceFromLeft, 7 - distanceFromUp);
				directionLengths[3] = min(distanceFromLeft, 7 - distanceFromUp);

				for (int directionIndex{}; directionIndex < directionOffsets.size(); ++directionIndex)
				{
					int directionOffset{ directionOffsets[directionIndex] };

					for(int multipliedIndex{1}; multipliedIndex <= directionLengths[directionIndex]; ++multipliedIndex)
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

							m_PossibleMoves.emplace_back(move);
							break;
						}
						else
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + directionOffset * multipliedIndex;
							move.moveType = MoveType::QuietMove;

							m_PossibleMoves.emplace_back(move);
							continue;
						}
					}
				}
			}
		}
	}
#pragma endregion

#pragma region WhiteBishops
	if (m_WhiteToMove)
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.whiteBishops & static_cast<unsigned long long>(1) << squareIndex)
			{
				int distanceFromLeft = squareIndex % 8;
				int distanceFromUp = squareIndex / 8;

				directionLengths[0] = min(distanceFromLeft, distanceFromUp);
				directionLengths[1] = min(7 - distanceFromLeft, distanceFromUp);
				directionLengths[2] = min(7 - distanceFromLeft, 7 - distanceFromUp);
				directionLengths[3] = min(distanceFromLeft, 7 - distanceFromUp);

				for (int directionIndex{}; directionIndex < directionOffsets.size(); ++directionIndex)
				{
					int directionOffset{ directionOffsets[directionIndex] };

					for (int multipliedIndex{ 1 }; multipliedIndex <= directionLengths[directionIndex]; ++multipliedIndex)
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

							m_PossibleMoves.emplace_back(move);
							break;
						}
						else
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + directionOffset * multipliedIndex;
							move.moveType = MoveType::QuietMove;

							m_PossibleMoves.emplace_back(move);
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
	std::vector<int> directionOffsets{-1, -8, +1, +8};
	std::vector<int> directionLengths{0, 0, 0, 0};

#pragma region BlackRooks
	if (!m_WhiteToMove)
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.blackRooks & static_cast<unsigned long long>(1) << squareIndex)
			{
				int distanceFromLeft = squareIndex % 8;
				int distanceFromUp = squareIndex / 8;

				directionLengths[0] = distanceFromLeft;
				directionLengths[1] = distanceFromUp;
				directionLengths[2] = 7 - distanceFromLeft;
				directionLengths[3] = 7 - distanceFromUp;

				for (int directionIndex{}; directionIndex < directionOffsets.size(); ++directionIndex)
				{
					int directionOffset{ directionOffsets[directionIndex] };

					for (int multipliedIndex{ 1 }; multipliedIndex <= directionLengths[directionIndex]; ++multipliedIndex)
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

							m_PossibleMoves.emplace_back(move);
							break;
						}
						else
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + directionOffset * multipliedIndex;
							move.moveType = MoveType::QuietMove;

							m_PossibleMoves.emplace_back(move);
							continue;
						}
					}
				}
			}
		}
	}
#pragma endregion

#pragma region WhiteRooks
	if (m_WhiteToMove)
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.whiteRooks & static_cast<unsigned long long>(1) << squareIndex)
			{
				int distanceFromLeft = squareIndex % 8;
				int distanceFromUp = squareIndex / 8;

				directionLengths[0] = distanceFromLeft;
				directionLengths[1] = distanceFromUp;
				directionLengths[2] = 7 - distanceFromLeft;
				directionLengths[3] = 7 - distanceFromUp;

				for (int directionIndex{}; directionIndex < directionOffsets.size(); ++directionIndex)
				{
					int directionOffset{ directionOffsets[directionIndex] };

					for (int multipliedIndex{ 1 }; multipliedIndex <= directionLengths[directionIndex]; ++multipliedIndex)
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

							m_PossibleMoves.emplace_back(move);
							break;
						}
						else
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + directionOffset * multipliedIndex;
							move.moveType = MoveType::QuietMove;

							m_PossibleMoves.emplace_back(move);
							continue;
						}
					}
				}
			}
		}
	}
#pragma endregion
}
void ChessBoard::CalculateQueenMoves()
{
	std::vector<int> directionOffsets{ -1, -8, +1, +8,  -9, -7, +9, +7 };
	std::vector<int> directionLengths{ 0, 0, 0, 0,  0, 0, 0, 0 };

#pragma region BlackQueens
	if (!m_WhiteToMove)
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.blackQueens & static_cast<unsigned long long>(1) << squareIndex)
			{
				int distanceFromLeft = squareIndex % 8;
				int distanceFromUp = squareIndex / 8;

				directionLengths[0] = distanceFromLeft;
				directionLengths[1] = distanceFromUp;
				directionLengths[2] = 7 - distanceFromLeft;
				directionLengths[3] = 7 - distanceFromUp;
				directionLengths[4] = min(distanceFromLeft, distanceFromUp);
				directionLengths[5] = min(7 - distanceFromLeft, distanceFromUp);
				directionLengths[6] = min(7 - distanceFromLeft, 7 - distanceFromUp);
				directionLengths[7] = min(distanceFromLeft, 7 - distanceFromUp);

				for (int directionIndex{}; directionIndex < directionOffsets.size(); ++directionIndex)
				{
					int directionOffset{ directionOffsets[directionIndex] };

					for (int multipliedIndex{ 1 }; multipliedIndex <= directionLengths[directionIndex]; ++multipliedIndex)
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

							m_PossibleMoves.emplace_back(move);
							break;
						}
						else
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + directionOffset * multipliedIndex;
							move.moveType = MoveType::QuietMove;

							m_PossibleMoves.emplace_back(move);
							continue;
						}
					}
				}
			}
		}
	}
#pragma endregion

#pragma region WhiteQueens
	if (m_WhiteToMove)
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.whiteQueens & static_cast<unsigned long long>(1) << squareIndex)
			{
				int distanceFromLeft = squareIndex % 8;
				int distanceFromUp = squareIndex / 8;

				directionLengths[0] = distanceFromLeft;
				directionLengths[1] = distanceFromUp;
				directionLengths[2] = 7 - distanceFromLeft;
				directionLengths[3] = 7 - distanceFromUp;
				directionLengths[4] = min(distanceFromLeft, distanceFromUp);
				directionLengths[5] = min(7 - distanceFromLeft, distanceFromUp);
				directionLengths[6] = min(7 - distanceFromLeft, 7 - distanceFromUp);
				directionLengths[7] = min(distanceFromLeft, 7 - distanceFromUp);

				for (int directionIndex{}; directionIndex < directionOffsets.size(); ++directionIndex)
				{
					int directionOffset{ directionOffsets[directionIndex] };

					for (int multipliedIndex{ 1 }; multipliedIndex <= directionLengths[directionIndex]; ++multipliedIndex)
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

							m_PossibleMoves.emplace_back(move);
							break;
						}
						else
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + directionOffset * multipliedIndex;
							move.moveType = MoveType::QuietMove;

							m_PossibleMoves.emplace_back(move);
							continue;
						}
					}
				}
			}
		}
	}
#pragma endregion
}
void ChessBoard::CalculateSlidingMoves()
{


}
void ChessBoard::CalculateKingMoves(bool originalBoard)
{
	std::vector<int> directionOffsets{ -1, -8, +1, +8,  -9, -7, +9, +7 };
	std::vector<int> directionLengths{ 0, 0, 0, 0,  0, 0, 0, 0 };

#pragma region BlackKing
	if (!m_WhiteToMove)
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.blackKing & static_cast<unsigned long long>(1) << squareIndex)
			{
				int distanceFromLeft = squareIndex % 8;
				int distanceFromUp = squareIndex / 8;

				directionLengths[0] = distanceFromLeft;
				directionLengths[1] = distanceFromUp;
				directionLengths[2] = 7 - distanceFromLeft;
				directionLengths[3] = 7 - distanceFromUp;
				directionLengths[4] = min(distanceFromLeft, distanceFromUp);
				directionLengths[5] = min(7 - distanceFromLeft, distanceFromUp);
				directionLengths[6] = min(7 - distanceFromLeft, 7 - distanceFromUp);
				directionLengths[7] = min(distanceFromLeft, 7 - distanceFromUp);

				for (int directionIndex{}; directionIndex < directionOffsets.size(); ++directionIndex)
				{
					if (directionLengths[directionIndex] == 0) continue;
					int directionOffset{ directionOffsets[directionIndex] };

					if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << (squareIndex + directionOffset))
					{
						continue;
					}
					else if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << (squareIndex + directionOffset))
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = squareIndex + directionOffset;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.emplace_back(move);
						continue;
					}
					else
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = squareIndex + directionOffset;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.emplace_back(move);
						continue;
					}
					
				}

				if (m_BlackCanCastleKingSide)
				{
					if (!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & static_cast<unsigned long long>(1) << (squareIndex + 2)) &&
						!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & static_cast<unsigned long long>(1) << (squareIndex + 1)))
					{
						if (originalBoard && !IsSquareInCheckByOtherColor(squareIndex + 1))
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + 2;
							move.moveType = MoveType::KingCastle;

							m_PossibleMoves.emplace_back(move);
						}
					}
				}
				if (m_BlackCanCastleQueenSide)
				{
					if (!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & static_cast<unsigned long long>(1) << (squareIndex - 1)) &&
						!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & static_cast<unsigned long long>(1) << (squareIndex - 2)) &&
						!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & static_cast<unsigned long long>(1) << (squareIndex - 3)))
					{
						if (originalBoard && !IsSquareInCheckByOtherColor(squareIndex - 1))
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex - 2;
							move.moveType = MoveType::QueenCastle;

							m_PossibleMoves.emplace_back(move);
						}
					}
				}
			}
		}

		
	}
#pragma endregion

#pragma region WhiteKing
	if (m_WhiteToMove)
	{
		for (int squareIndex{}; squareIndex < 64; ++squareIndex)
		{
			if (m_BitBoards.whiteKing & static_cast<unsigned long long>(1) << squareIndex)
			{
				int distanceFromLeft = squareIndex % 8;
				int distanceFromUp = squareIndex / 8;

				directionLengths[0] = distanceFromLeft;
				directionLengths[1] = distanceFromUp;
				directionLengths[2] = 7 - distanceFromLeft;
				directionLengths[3] = 7 - distanceFromUp;
				directionLengths[4] = min(distanceFromLeft, distanceFromUp);
				directionLengths[5] = min(7 - distanceFromLeft, distanceFromUp);
				directionLengths[6] = min(7 - distanceFromLeft, 7 - distanceFromUp);
				directionLengths[7] = min(distanceFromLeft, 7 - distanceFromUp);

				for (int directionIndex{}; directionIndex < directionOffsets.size(); ++directionIndex)
				{
					if (directionLengths[directionIndex] == 0) continue;
					int directionOffset{ directionOffsets[directionIndex] };

					if (m_BitBoards.whitePieces & static_cast<unsigned long long>(1) << (squareIndex + directionOffset))
					{
						continue;
					}
					else if (m_BitBoards.blackPieces & static_cast<unsigned long long>(1) << (squareIndex + directionOffset))
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = squareIndex + directionOffset;
						move.moveType = MoveType::Capture;

						m_PossibleMoves.emplace_back(move);
						continue;
					}
					else
					{
						Move move{};
						move.startSquareIndex = squareIndex;
						move.targetSquareIndex = squareIndex + directionOffset;
						move.moveType = MoveType::QuietMove;

						m_PossibleMoves.emplace_back(move);
						continue;
					}

				}

				if (m_WhiteCanCastleKingSide)
				{
					if (!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & static_cast<unsigned long long>(1) << (squareIndex + 2)) &&
						!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & static_cast<unsigned long long>(1) << (squareIndex + 1)))
					{
						if (originalBoard && !IsSquareInCheckByOtherColor(squareIndex + 1))
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex + 2;
							move.moveType = MoveType::KingCastle;

							m_PossibleMoves.emplace_back(move);
						}
					}
				}
				if (m_WhiteCanCastleQueenSide)
				{
					if (!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & static_cast<unsigned long long>(1) << (squareIndex - 1)) &&
						!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & static_cast<unsigned long long>(1) << (squareIndex - 2)) &&
						!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & static_cast<unsigned long long>(1) << (squareIndex - 3)))
					{
						if (originalBoard && !IsSquareInCheckByOtherColor(squareIndex - 1))
						{
							Move move{};
							move.startSquareIndex = squareIndex;
							move.targetSquareIndex = squareIndex - 2;
							move.moveType = MoveType::QueenCastle;

							m_PossibleMoves.emplace_back(move);
						}
					}
				}
			}
		}
	}
#pragma endregion
}


void ChessBoard::CheckForIllegalMoves()
{
	std::vector<std::list<Move>::iterator> illegalIterators{};
	for (auto it{m_PossibleMoves.begin()}; it != m_PossibleMoves.end(); ++it)
	{
		ChessBoard simulatedChessBoard{ *this };
		simulatedChessBoard.MakeMove(*it, false, false);

		if (simulatedChessBoard.IsOtherKingInCheck())
		{
			illegalIterators.emplace_back(it);
		}
	}

	for (auto& it : illegalIterators)
	{
		m_PossibleMoves.erase(it);
	}
}
bool ChessBoard::IsOtherKingInCheck()
{
	uint64_t kingBitBoard{m_WhiteToMove ? m_BitBoards.blackKing : m_BitBoards.whiteKing };

	int kingSquareIndex{};
	for (int index{}; index < 64; ++index)
	{
		if (kingBitBoard & static_cast<unsigned long long>(1) << index)
		{
			kingSquareIndex = index;
			break;
		}
	}

	for (auto& move : m_PossibleMoves)
	{
		if (move.targetSquareIndex == kingSquareIndex)
		{
			return true;
		}
	}
	return false;
}

bool ChessBoard::IsSquareInCheckByOtherColor(int squareIndex)
{
	ChessBoard copyBoard{ *this };
	copyBoard.m_WhiteToMove = !m_WhiteToMove;
	copyBoard.CalculatePossibleMoves(false);

	for (auto& move : copyBoard.m_PossibleMoves)
	{
		if (move.targetSquareIndex == squareIndex)
		{
			return true;
		}
	}
	return false;
}


void ChessBoard::CheckForGameEnd()
{
	CheckForCheckmate();
	CheckForFiftyMoveRule();
	CheckForInsufficientMaterial();
	CheckForRepetition();
}
void ChessBoard::CheckForCheckmate()
{
	if (m_PossibleMoves.size() == 0)
	{
		ChessBoard copiedChessBoard{ *this };
		copiedChessBoard.m_WhiteToMove = !copiedChessBoard.m_WhiteToMove;
		copiedChessBoard.CalculatePossibleMoves(false);

		if (copiedChessBoard.IsOtherKingInCheck())
		{
			if (m_WhiteToMove)
				m_GameProgress = GameProgress::BlackWon;
			else
				m_GameProgress = GameProgress::WhiteWon;
		}
		else
		{
			m_GameProgress = GameProgress::Draw;
		}
		m_FirstFrameGameEnd = true;
	}
}
void ChessBoard::CheckForFiftyMoveRule()
{
	if (m_HalfMoveClock >= 50)
	{
		m_GameProgress = GameProgress::Draw;

		m_FirstFrameGameEnd = true;
	}

}
void ChessBoard::CheckForInsufficientMaterial()
{
	bool insufficientMaterial{ false };

	int amountOfWhitePawns{ GetAmountOfPiecesFromBitBoard(m_BitBoards.whitePawns)};
	int amountOfWhiteKnights{ GetAmountOfPiecesFromBitBoard(m_BitBoards.whiteKnights)};
	int amountOfWhiteBishops{ GetAmountOfPiecesFromBitBoard(m_BitBoards.whiteBishops)};
	int amountOfWhiteRooks{ GetAmountOfPiecesFromBitBoard(m_BitBoards.whiteRooks)};
	int amountOfWhiteQueens{ GetAmountOfPiecesFromBitBoard(m_BitBoards.whiteQueens)};

	int amountOfBlackPawns{ GetAmountOfPiecesFromBitBoard(m_BitBoards.blackPawns)};
	int amountOfBlackKnights{ GetAmountOfPiecesFromBitBoard(m_BitBoards.blackKnights)};
	int amountOfBlackBishops{ GetAmountOfPiecesFromBitBoard(m_BitBoards.blackBishops)};
	int amountOfBlackRooks{ GetAmountOfPiecesFromBitBoard(m_BitBoards.blackRooks)};
	int amountOfBlackQueens{ GetAmountOfPiecesFromBitBoard(m_BitBoards.blackQueens)};

	// King vs King
	if (amountOfWhitePawns + amountOfWhiteKnights + amountOfWhiteBishops + amountOfWhiteRooks + amountOfWhiteQueens +
		amountOfBlackPawns + amountOfBlackKnights + amountOfBlackBishops + amountOfBlackRooks + amountOfBlackQueens == 0)
	{
		insufficientMaterial = true;
	}

	// King + minor piece vs King (White)
	else if (amountOfWhitePawns + amountOfWhiteRooks + amountOfWhiteQueens +
		amountOfBlackPawns + amountOfBlackKnights + amountOfBlackBishops + amountOfBlackRooks + amountOfBlackQueens == 0
		&& amountOfWhiteBishops + amountOfWhiteKnights == 1)
	{
		insufficientMaterial = true;
	}
	// King + minor piece vs King (Black)
	else if (amountOfBlackPawns + amountOfBlackRooks + amountOfBlackQueens +
		amountOfWhitePawns + amountOfWhiteKnights + amountOfWhiteBishops + amountOfWhiteRooks + amountOfWhiteQueens == 0
		&& amountOfBlackBishops + amountOfBlackKnights == 1)
	{
		insufficientMaterial = true;
	}

	// King + 2 knights vs King (White)
	else if (amountOfWhitePawns + amountOfWhiteRooks + amountOfWhiteQueens + amountOfWhiteBishops +
		amountOfBlackPawns + amountOfBlackKnights + amountOfBlackBishops + amountOfBlackRooks + amountOfBlackQueens == 0
		&&  amountOfWhiteKnights == 2)
	{
		insufficientMaterial = true;
	}
	// King + 2 knights vs King (Black)
	else if (amountOfBlackPawns + amountOfBlackRooks + amountOfBlackQueens + amountOfBlackBishops +
		amountOfWhitePawns + amountOfWhiteKnights + amountOfWhiteBishops + amountOfWhiteRooks + amountOfWhiteQueens == 0
		&& amountOfBlackKnights == 2)
	{
		insufficientMaterial = true;
	}

	// King + minor piece vs King + minor piece
	else if (amountOfWhitePawns + amountOfWhiteRooks + amountOfWhiteQueens +
		amountOfBlackPawns + amountOfBlackRooks + amountOfBlackQueens == 0
		&& amountOfWhiteKnights + amountOfWhiteBishops == 1
		&& amountOfBlackKnights + amountOfBlackBishops == 1)
	{
		insufficientMaterial = true;
	}

	if (insufficientMaterial)
	{
		m_GameProgress = GameProgress::Draw;
		m_FirstFrameGameEnd = true;
	}
}
int ChessBoard::GetAmountOfPiecesFromBitBoard(uint64_t bitBoard)
{
	int amount{};
	for (int index{}; index < 64; ++index)
	{
		if (bitBoard & static_cast<unsigned long long>(1) << index) ++amount;
	}
	return amount;
}
void ChessBoard::CheckForRepetition()
{
	int amountOfCurrentApearences{0};
	for (auto& gameState : m_GameStateHistory)
	{
		if (gameState.bitBoards == m_BitBoards)
		{
			++amountOfCurrentApearences;
		}
	}

	if (amountOfCurrentApearences >= 2)
	{
		m_GameProgress = GameProgress::Draw;
		m_FirstFrameGameEnd = true;
	}

}

void ChessBoard::UpdateGameStateHistory()
{
	GameState gameState{};

	gameState.bitBoards = m_BitBoards;
	gameState.possibleMoves = m_PossibleMoves;

	gameState.whiteToMove = m_WhiteToMove;

	gameState.whiteCanCastleKingSide = m_WhiteCanCastleKingSide;
	gameState.whiteCanCastleQueenSide = m_WhiteCanCastleQueenSide;
	gameState.blackCanCastleKingSide = m_BlackCanCastleKingSide;
	gameState.blackCanCastleQueenSide = m_BlackCanCastleQueenSide;

	gameState.enPassantSquares = m_EnPassantSquares;
	gameState.halfMoveClock = m_HalfMoveClock;
	gameState.fullMoveCounter = m_FullMoveCounter;

	m_GameStateHistory.emplace_back(gameState);
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