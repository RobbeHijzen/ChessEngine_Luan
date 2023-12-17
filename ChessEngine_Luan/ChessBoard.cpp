#include "ChessBoard.h"
#include "GameEngine.h"
#include <string>
#include <cassert>
#include <cmath>

ChessBoard::ChessBoard()
{
	//m_PossibleMoves.resize(218);
	m_GameStateHistory.resize(50);


	//std::string FEN{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
	//std::string FEN{ "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq 0 0" }; // Position 2	(Depth 3 = 97862)
	//std::string FEN{ "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - 0 0" }; // Position 3
	//std::string FEN{ "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1" }; // Position 4
	//std::string FEN{ "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8" }; // Position 5
	//std::string FEN{ "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10" }; // Position 6
	
	std::string FEN{ "3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1" }; // Illegal ep move #1				Depth : 6 = 1134888		// 
	//std::string FEN{ "8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1" }; // Illegal ep move #2			Depth : 6 = 1015133		// 
	//std::string FEN{ "8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1" }; // EP Capture Checks Opponent	Depth : 6 = 1440467		// 
	//std::string FEN{ "5k2/8/8/8/8/8/8/4K2R w K - 0 1" }; // Short Castling Gives Check		Depth : 6 = 661072		// 
	//std::string FEN{ "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1" }; // Long Castling Gives Check			Depth : 6 = 803711		// 
	//std::string FEN{ "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1" }; // Castle Rights			Depth : 4 = 1274206		//
	//std::string FEN{ "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1" }; // Castling Prevented		Depth : 4 = 1720476		//
	//std::string FEN{ "2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1" }; // Promote out of Check			Depth : 6 = 3821001		// 
	//std::string FEN{ "8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1" }; // Discovered Check				Depth : 5 = 1004658		// 
	//std::string FEN{ "4k3/1P6/8/8/8/8/K7/8 w - - 0 1" }; // Promote to give check				Depth : 6 = 217342		// 
	//std::string FEN{ "8/P1k5/K7/8/8/8/8/8 w - - 0 1" }; // Under Promote to give check		Depth : 6 = 92683		// 
	//std::string FEN{ "K1k5/8/P7/8/8/8/8/8 w - - 0 1" }; // Self Stalemate						Depth : 6 = 2217		// 
	//std::string FEN{ "8/k1P5/8/1K6/8/8/8/8 w - - 0 1" }; // Stalemate & Checkmate 1			Depth : 7 = 567584		// 
	//std::string FEN{ "8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1" }; // Stalemate & Checkmate 2		Depth : 4 = 23527		// 
	SetBitboardsFromFEN(FEN);
	
	UpdateColorBitboards();
	UpdateThreatMap({}, false);
	CalculatePossibleMoves();

	UpdateGameStateHistory();
}
int ChessBoard::StartMoveGenerationTest(int depth)
{
	m_TotalAmount = 0;
	return MoveGenerationTest(depth, depth);
}
int ChessBoard::MoveGenerationTest(int depth, int initialDepth)
{
	if (depth == 0) return 1;

	int positionsCounter{};
	for (int index{}; index < m_PossibleMoves.size(); ++index)
	{

		auto it{ m_PossibleMoves.begin() };
		std::advance(it, index);
		Move move{ *it };

		if (move.moveType == MoveType::Capture || move.moveType == MoveType::BishopPromotionCapture || move.moveType == MoveType::KnightPromotionCapture || move.moveType == MoveType::RookPromotionCapture || move.moveType == MoveType::QueenPromotionCapture || move.moveType == MoveType::EnPassantCaptureLeft || move.moveType == MoveType::EnPassantCaptureRight)
			++m_CaptureAmount;

		if (move.moveType == MoveType::EnPassantCaptureLeft || move.moveType == MoveType::EnPassantCaptureRight)
			++m_EnPassantAmount;

		if (move.moveType == MoveType::KingCastle || move.moveType == MoveType::QueenCastle)
			++m_CastleAmount;

		if (move.moveType == MoveType::BishopPromotion || move.moveType == MoveType::KnightPromotion || move.moveType == MoveType::RookPromotion || move.moveType == MoveType::QueenPromotion || move.moveType == MoveType::BishopPromotionCapture || move.moveType == MoveType::KnightPromotionCapture || move.moveType == MoveType::RookPromotionCapture || move.moveType == MoveType::QueenPromotionCapture)
			++m_PromotionAmount;

		MakeMove(move);
		int amount{ MoveGenerationTest(depth - 1, initialDepth) };
		positionsCounter += amount;
		if(depth == 1)
			m_TotalAmount += amount;
		
		UnMakeLastMove();
	}

	return positionsCounter;
}

void ChessBoard::MakeMove(Move move, bool isOriginalBoard)
{
	if (m_GameProgress != GameProgress::InProgress) { m_PossibleMoves.clear(); UpdateGameStateHistory(); return; }
	m_WhiteToMove = !m_WhiteToMove;

	if(m_WhiteToMove) ++m_HalfMoveClock;
	++m_FullMoveCounter;
	
	m_EnPassantSquares = 0;

	uint64_t* startBitBoard{GetBitboardFromSquare(move.startSquareIndex)};
	CheckCastleRights(*startBitBoard, move.startSquareIndex);
	
	UpdateBitBoards(move, startBitBoard);
	CalculatePossibleMoves();
	if(isOriginalBoard)
		//CheckForIllegalMoves();
	
	CheckForGameEnd();

	UpdateGameStateHistory();
}
void ChessBoard::UnMakeLastMove()
{

	GameState gameState = m_GameStateHistory[--m_GameStateHistoryCounter];

	m_GameProgress = gameState.gameProgress;

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
	m_BitBoards.whiteThreatMap = 0;
	m_BitBoards.blackThreatMap = 0;
	
	switch (move.moveType)
	{
		case MoveType::QuietMove:
		{
			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex];
			*startBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			if (*startBitBoard == m_BitBoards.whitePawns || *startBitBoard == m_BitBoards.blackPawns) m_HalfMoveClock = 0;

			break;
		}
		case MoveType::DoublePawnPush:
		{
			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;
			*startBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;
			m_EnPassantSquares |= m_BitMasks.bitMasks[move.startSquareIndex + (move.targetSquareIndex - move.startSquareIndex) / 2];

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::KingCastle:
		{
			uint64_t* rookBitBoard{ GetBitboardFromSquare(move.targetSquareIndex + 1) };
			*rookBitBoard ^= m_BitMasks.bitMasks[move.targetSquareIndex + 1];
			*rookBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex - 1];

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;
			*startBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			break;
		}
		case MoveType::QueenCastle:
		{
			uint64_t* rookBitBoard{ GetBitboardFromSquare(move.targetSquareIndex - 2) };
			*rookBitBoard ^= m_BitMasks.bitMasks[move.targetSquareIndex - 2];
			*rookBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex + 1];

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex];
			*startBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			break;
		}
		case MoveType::Capture:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };
			*targetBitBoard ^= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;
			*startBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::EnPassantCaptureLeft:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare((move.targetSquareIndex + (move.startSquareIndex - move.targetSquareIndex - 1))) };
			*targetBitBoard ^= m_BitMasks.bitMasks[(move.targetSquareIndex + (move.startSquareIndex - move.targetSquareIndex - 1))] ;


			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;
			*startBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::EnPassantCaptureRight:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare((move.targetSquareIndex + (move.startSquareIndex - move.targetSquareIndex + 1))) };
			*targetBitBoard ^= m_BitMasks.bitMasks[(move.targetSquareIndex + (move.startSquareIndex - move.targetSquareIndex + 1))] ;


			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;
			*startBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::KnightPromotion:
		{
			uint64_t* knightBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteKnights : &m_BitBoards.blackKnights };
			*knightBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex];

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::BishopPromotion:
		{
			uint64_t* bishopBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteBishops : &m_BitBoards.blackBishops };
			*bishopBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::RookPromotion:
		{
			uint64_t* rookBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteRooks : &m_BitBoards.blackRooks };
			*rookBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::QueenPromotion:
		{
			uint64_t* queenBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteQueens : &m_BitBoards.blackQueens };
			*queenBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::KnightPromotionCapture:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };
			*targetBitBoard ^= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			uint64_t* knightBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteKnights : &m_BitBoards.blackKnights };
			*knightBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::BishopPromotionCapture:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };
			*targetBitBoard ^= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			uint64_t* bishopBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteBishops : &m_BitBoards.blackBishops };
			*bishopBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::RookPromotionCapture:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };
			*targetBitBoard ^= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			uint64_t* rookBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteRooks : &m_BitBoards.blackRooks };
			*rookBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}
		case MoveType::QueenPromotionCapture:
		{
			uint64_t* targetBitBoard{ GetBitboardFromSquare(move.targetSquareIndex) };
			*targetBitBoard ^= m_BitMasks.bitMasks[move.targetSquareIndex] ;

			uint64_t* queenBitBoard{ (*startBitBoard & m_BitBoards.whitePieces) ? &m_BitBoards.whiteQueens : &m_BitBoards.blackQueens };
			*queenBitBoard |= m_BitMasks.bitMasks[move.targetSquareIndex];

			*startBitBoard ^= m_BitMasks.bitMasks[move.startSquareIndex] ;

			m_HalfMoveClock = 0;
			break;
		}

	}

	UpdateColorBitboards();
	UpdateThreatMap(move);

}

void ChessBoard::UpdateThreatMap(Move move, bool useMove)
{
	m_CurrentPawnsBitBoard = !m_WhiteToMove ? m_BitBoards.whitePawns : m_BitBoards.blackPawns;
	m_CurrentKnightsBitBoard = !m_WhiteToMove ? m_BitBoards.whiteKnights : m_BitBoards.blackKnights;
	m_CurrentBishopsBitBoard = !m_WhiteToMove ? m_BitBoards.whiteBishops : m_BitBoards.blackBishops;
	m_CurrentRooksBitBoard = !m_WhiteToMove ? m_BitBoards.whiteRooks : m_BitBoards.blackRooks;
	m_CurrentQueensBitBoard = !m_WhiteToMove ? m_BitBoards.whiteQueens : m_BitBoards.blackQueens;
	m_CurrentKingBitBoard = !m_WhiteToMove ? m_BitBoards.whiteKing : m_BitBoards.blackKing;

	m_CurrentOwnPiecesBitBoard = !m_WhiteToMove ? m_BitBoards.whitePieces : m_BitBoards.blackPieces;
	m_CurrentOpponentPiecesBitBoard = !m_WhiteToMove ? m_BitBoards.blackPieces : m_BitBoards.whitePieces;

	m_pCurrentOwnThreatMap = !m_WhiteToMove ? &m_BitBoards.whiteThreatMap : &m_BitBoards.blackThreatMap;
	m_pCurrentOpponentThreatMap = !m_WhiteToMove ? &m_BitBoards.blackThreatMap : &m_BitBoards.whiteThreatMap;
	
	auto tempMovedThreatMap1{ std::make_unique<uint64_t>() };
	uint64_t mask1{ m_BitMasks.bitMasks[move.targetSquareIndex] };
	int checkCount{};

	if (m_CurrentPawnsBitBoard & mask1)
		CalculatePawnThreats(move.targetSquareIndex, tempMovedThreatMap1.get());
	else if (m_CurrentKnightsBitBoard & mask1)
		CalculateKnightThreats(move.targetSquareIndex, tempMovedThreatMap1.get());
	else if (m_CurrentBishopsBitBoard & mask1)
		CalculateSlidingThreats(move.targetSquareIndex, tempMovedThreatMap1.get(), 4, 8);
	else if (m_CurrentRooksBitBoard & mask1)
		CalculateSlidingThreats(move.targetSquareIndex, tempMovedThreatMap1.get(),0, 4);
	else if (m_CurrentQueensBitBoard & mask1)
		CalculateSlidingThreats(move.targetSquareIndex, tempMovedThreatMap1.get(), 0, 8);
	else if (m_CurrentKingBitBoard & mask1)
		CalculateKingThreats(move.targetSquareIndex, tempMovedThreatMap1.get());
	
	*m_pCurrentOwnThreatMap |= *tempMovedThreatMap1;
	if ((m_WhiteToMove ? m_BitBoards.whiteKing : m_BitBoards.blackKing) & *tempMovedThreatMap1)
	{
		++checkCount;
		UpdateRayMap(*tempMovedThreatMap1, move.targetSquareIndex);
	}


	for (int squareIndex{}; squareIndex < 64; ++squareIndex)
	{
		if (squareIndex == move.targetSquareIndex) continue;

		auto tempMovedThreatMap2{ std::make_unique<uint64_t>() };
		uint64_t mask{ m_BitMasks.bitMasks[squareIndex] };

		if (m_CurrentPawnsBitBoard & mask)
			CalculatePawnThreats(squareIndex, tempMovedThreatMap2.get());
		else if (m_CurrentKnightsBitBoard & mask)
			CalculateKnightThreats(squareIndex, tempMovedThreatMap2.get());
		else if (m_CurrentBishopsBitBoard & mask)
			CalculateSlidingThreats(squareIndex, tempMovedThreatMap2.get(), 4, 8);
		else if (m_CurrentRooksBitBoard & mask)
			CalculateSlidingThreats(squareIndex, tempMovedThreatMap2.get(), 0, 4);
		else if (m_CurrentQueensBitBoard & mask)
			CalculateSlidingThreats(squareIndex, tempMovedThreatMap2.get(), 0, 8);
		else if (m_CurrentKingBitBoard & mask)
			CalculateKingThreats(squareIndex, tempMovedThreatMap2.get());

		if ((m_WhiteToMove ? m_BitBoards.whiteKing : m_BitBoards.blackKing) & *tempMovedThreatMap2)
		{
			++checkCount;
			UpdateRayMap(*tempMovedThreatMap2, squareIndex);
		}
		*m_pCurrentOwnThreatMap |= *tempMovedThreatMap2;
	}
	

	if (checkCount == 0) { m_IsKingInCheck = false; m_IsKingInDoubleCheck = false; }
	if (checkCount >= 1) m_IsKingInCheck = true;
	if (checkCount == 2) m_IsKingInDoubleCheck = true;

	
}
void ChessBoard::UpdateRayMap(uint64_t checkingPieceMap, int targetSquare)
{
	m_BitBoards.checkRay = 0;
	uint64_t kingBitBoard{ m_WhiteToMove ? m_BitBoards.whiteKing : m_BitBoards.blackKing };

	int kingSquare{ int(std::log2(kingBitBoard) + 0.5) };
	int kingSquareRow{kingSquare / 8};
	int kingSquareCol{kingSquare % 8};

	int targetSquareRow{ targetSquare / 8};
	int targetSquareCol{ targetSquare % 8};

	int rowDif{ targetSquareRow  - kingSquareRow};
	int colDif{ targetSquareCol - kingSquareCol};

	if (rowDif == 0)
	{
		int offset{ colDif < 0 ? 1 : -1 };
		for (int index{}; index < abs(colDif); ++index)
		{
			m_BitBoards.checkRay |= m_BitMasks.bitMasks[targetSquare + offset * index];
		}
	}
	else if (colDif == 0)
	{
		int offset{ rowDif < 0 ? 8 : -8 };
		for (int index{}; index < abs(rowDif); ++index)
		{
			m_BitBoards.checkRay |= m_BitMasks.bitMasks[targetSquare + offset * index];
		}
	}
	else if (abs(rowDif) == abs(colDif))
	{
		int offset{ (rowDif < 0 ? 8 : -8) + (colDif < 0 ? 1 : -1) };
		for (int index{}; index < abs(rowDif); ++index)
		{
			m_BitBoards.checkRay |= m_BitMasks.bitMasks[targetSquare + offset * index];
		}
	}
	else
	{
		m_BitBoards.checkRay |= m_BitMasks.bitMasks[targetSquare];
	}
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
	if (m_BitBoards.whitePawns & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.whitePawns;
	if (m_BitBoards.whiteKnights & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.whiteKnights;
	if (m_BitBoards.whiteBishops & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.whiteBishops;
	if (m_BitBoards.whiteRooks & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.whiteRooks;
	if (m_BitBoards.whiteQueens & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.whiteQueens;
	if (m_BitBoards.whiteKing & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.whiteKing;

	if (m_BitBoards.blackPawns & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.blackPawns;
	if (m_BitBoards.blackKnights & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.blackKnights;
	if (m_BitBoards.blackBishops & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.blackBishops;
	if (m_BitBoards.blackRooks & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.blackRooks;
	if (m_BitBoards.blackQueens & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.blackQueens;
	if (m_BitBoards.blackKing & m_BitMasks.bitMasks[squareIndex] ) return &m_BitBoards.blackKing;

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


void ChessBoard::CalculatePossibleMoves()
{
	m_CurrentPawnsBitBoard = m_WhiteToMove ? m_BitBoards.whitePawns : m_BitBoards.blackPawns;
	m_CurrentKnightsBitBoard = m_WhiteToMove ? m_BitBoards.whiteKnights : m_BitBoards.blackKnights;
	m_CurrentBishopsBitBoard = m_WhiteToMove ? m_BitBoards.whiteBishops : m_BitBoards.blackBishops;
	m_CurrentRooksBitBoard = m_WhiteToMove ? m_BitBoards.whiteRooks : m_BitBoards.blackRooks;
	m_CurrentQueensBitBoard = m_WhiteToMove ? m_BitBoards.whiteQueens : m_BitBoards.blackQueens;
	m_CurrentKingBitBoard = m_WhiteToMove ? m_BitBoards.whiteKing : m_BitBoards.blackKing;

	m_CurrentOwnPiecesBitBoard = m_WhiteToMove ? m_BitBoards.whitePieces : m_BitBoards.blackPieces;
	m_CurrentOpponentPiecesBitBoard = m_WhiteToMove ? m_BitBoards.blackPieces : m_BitBoards.whitePieces;

	m_pCurrentOwnThreatMap = m_WhiteToMove ? &m_BitBoards.whiteThreatMap : &m_BitBoards.blackThreatMap;
	m_pCurrentOpponentThreatMap = m_WhiteToMove ? &m_BitBoards.blackThreatMap : &m_BitBoards.whiteThreatMap;


	m_PossibleMoves.clear();

	for (int squareIndex{}; squareIndex < 64; ++squareIndex)
	{
		if (!m_IsKingInDoubleCheck)
		{
			uint64_t mask{ m_BitMasks.bitMasks[squareIndex] };

			if (m_CurrentPawnsBitBoard & mask)
				CalculatePawnMoves(squareIndex);
			else if (m_CurrentKnightsBitBoard & mask)
				CalculateKnightMoves(squareIndex);
			else if (m_CurrentBishopsBitBoard & mask)
				CalculateSlidingMoves(squareIndex, 4, 8);
			else if (m_CurrentRooksBitBoard & mask)
				CalculateSlidingMoves(squareIndex, 0, 4);
			else if (m_CurrentQueensBitBoard & mask)
				CalculateSlidingMoves(squareIndex, 0, 8);
		}
	}

	for (int squareIndex{}; squareIndex < 64; ++squareIndex)
	{
		if (m_CurrentKingBitBoard & m_BitMasks.bitMasks[squareIndex])
			CalculateKingMoves(squareIndex);
	}
	
	//if(originalBoard)
	//	CheckForIllegalMoves();
}

void ChessBoard::CalculatePawnMoves(int squareIndex)
{
	int verticalOffset{m_WhiteToMove ? -8 : 8};

	// If the square in front has nothing (exluding Promotions)
	{
		bool onValidRow{ m_WhiteToMove ? squareIndex > 15 : squareIndex < 48 };
		if (onValidRow &&
			!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & m_BitMasks.bitMasks[(squareIndex + verticalOffset)] ))
		{
			if (!(m_IsKingInCheck && !(m_BitBoards.checkRay & m_BitMasks.bitMasks[squareIndex + verticalOffset])))
			{
				Move move{};
				move.startSquareIndex = squareIndex;
				move.targetSquareIndex = squareIndex + verticalOffset;
				move.moveType = MoveType::QuietMove;

				m_PossibleMoves.emplace_back(move);
			}


			// Double Pawn Push
			bool onValidDoublePushRow{ m_WhiteToMove ? squareIndex > 47 : squareIndex < 16 };
			if (onValidDoublePushRow &&
				!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & m_BitMasks.bitMasks[(squareIndex + 2 * verticalOffset)] ))
			{
				if (!(m_IsKingInCheck && !(m_BitBoards.checkRay & m_BitMasks.bitMasks[squareIndex + 2 * verticalOffset])))
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + 2 * verticalOffset;
					move.moveType = MoveType::DoublePawnPush;

					m_PossibleMoves.emplace_back(move);
				}
			}

		}
	}
	//-----------------------------------
	// 
	// Promotion
	{
		bool onValidRow{ m_WhiteToMove ? squareIndex < 16 : squareIndex > 47 };
		if (onValidRow &&
			!((m_BitBoards.whitePieces | m_BitBoards.blackPieces) & m_BitMasks.bitMasks[(squareIndex + verticalOffset)] ))
		{
			if (!(m_IsKingInCheck && !(m_BitBoards.checkRay & m_BitMasks.bitMasks[squareIndex + verticalOffset])))
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
	}
	//-----------------------------------
	//
	// Side left Capture (including Promotion Captures)
	{
		bool onValidSquare{ squareIndex % 8 != 0 };
		if (onValidSquare)
		{
			if (m_CurrentOpponentPiecesBitBoard & m_BitMasks.bitMasks[(squareIndex + verticalOffset - 1)])
			{
				if (!(m_IsKingInCheck && !(m_BitBoards.checkRay & m_BitMasks.bitMasks[squareIndex + verticalOffset - 1])))
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
			
		}
	}
	//-----------------------------------
	//
	// Side right Capture (including Promotion Captures)
	{
		bool onValidSquare{ squareIndex % 8 != 7 };
		if (onValidSquare)
		{
			if (m_CurrentOpponentPiecesBitBoard & m_BitMasks.bitMasks[(squareIndex + verticalOffset + 1)])
			{
				if (!(m_IsKingInCheck && !(m_BitBoards.checkRay & m_BitMasks.bitMasks[squareIndex + verticalOffset + 1])))
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
			
		}
	}
	//-----------------------------------
	//
	// En Passant Left
	{
		bool onValidSquare{squareIndex % 8 != 0};
		if (onValidSquare && 
			m_EnPassantSquares & m_BitMasks.bitMasks[(squareIndex + verticalOffset - 1)] )
		{
			if (!(m_IsKingInCheck && !(m_BitBoards.checkRay & m_BitMasks.bitMasks[squareIndex + verticalOffset - 1])))
			{
				Move move{};
				move.startSquareIndex = squareIndex;
				move.targetSquareIndex = squareIndex + verticalOffset - 1;
				move.moveType = MoveType::EnPassantCaptureLeft;

				m_PossibleMoves.emplace_back(move);
			}
		}
	}
	//-----------------------------------
	//
	// En Passant Right
	{
		bool onValidSquare{ squareIndex % 8 != 7 };
		if (onValidSquare &&
			m_EnPassantSquares & m_BitMasks.bitMasks[(squareIndex + verticalOffset + 1)] )
		{
			if (!(m_IsKingInCheck && !(m_BitBoards.checkRay & m_BitMasks.bitMasks[squareIndex + verticalOffset + 1])))
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
void ChessBoard::CalculateKnightMoves(int squareIndex)
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
			if (!(m_IsKingInCheck && !(m_BitBoards.checkRay & m_BitMasks.bitMasks[newSquareIndex])))
				{
				if (m_CurrentOpponentPiecesBitBoard & m_BitMasks.bitMasks[newSquareIndex])
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = newSquareIndex;
					move.moveType = MoveType::Capture;

					m_PossibleMoves.emplace_back(move);
				}
				else if (~m_CurrentOwnPiecesBitBoard & m_BitMasks.bitMasks[newSquareIndex])
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
void ChessBoard::CalculateSlidingMoves(int squareIndex, int startOffsetIndex, int endOffsetIndex)
{
	for (int directionIndex{ startOffsetIndex }; directionIndex < endOffsetIndex; ++directionIndex)
	{
		for (int multipliedIndex{ 1 }; multipliedIndex <= m_SlidingOffsets.distancesFromEdges[directionIndex][squareIndex]; ++multipliedIndex)
		{
			int currentOffset{ m_SlidingOffsets.squareOffsets[directionIndex] };
			
			if (m_CurrentOwnPiecesBitBoard & m_BitMasks.bitMasks[(squareIndex + currentOffset * multipliedIndex)] )
			{
				break;
			}
			else if (m_CurrentOpponentPiecesBitBoard & m_BitMasks.bitMasks[(squareIndex + currentOffset * multipliedIndex)] )
			{
				if (!(m_IsKingInCheck && !(m_BitBoards.checkRay & m_BitMasks.bitMasks[squareIndex + currentOffset * multipliedIndex])))
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + currentOffset * multipliedIndex;
					move.moveType = MoveType::Capture;

					m_PossibleMoves.emplace_back(move);

					break;
				}
			}
			else
			{
				if (!(m_IsKingInCheck && !(m_BitBoards.checkRay & m_BitMasks.bitMasks[squareIndex + currentOffset * multipliedIndex])))
				{
					Move move{};
					move.startSquareIndex = squareIndex;
					move.targetSquareIndex = squareIndex + currentOffset * multipliedIndex;
					move.moveType = MoveType::QuietMove;

					m_PossibleMoves.emplace_back(move);
				}
				continue;
				
			}
		}


	}

	
}
void ChessBoard::CalculateKingMoves(int squareIndex)
{
	bool canCastleKingSide{ m_WhiteToMove ? m_WhiteCanCastleKingSide : m_BlackCanCastleKingSide};
	bool canCastleQueenSide{ m_WhiteToMove ? m_WhiteCanCastleQueenSide : m_BlackCanCastleQueenSide};
	
	
	for (int directionIndex{}; directionIndex < 8; ++directionIndex)
	{
		if (m_SlidingOffsets.distancesFromEdges[directionIndex][squareIndex] == 0) continue;
		int directionOffset{ m_SlidingOffsets.squareOffsets[directionIndex] };

		if (*m_pCurrentOpponentThreatMap & m_BitMasks.bitMasks[(squareIndex + directionOffset)]) continue;
		if (m_CurrentOwnPiecesBitBoard & m_BitMasks.bitMasks[(squareIndex + directionOffset)] ) continue;
		
		else if (m_CurrentOpponentPiecesBitBoard & m_BitMasks.bitMasks[(squareIndex + directionOffset)])
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
	//---------------
	//Castling stuff
	if (canCastleKingSide)
	{
		// If the king is in check, can't castle
		if (IsSquareInCheckByOtherColor(squareIndex)) return;

		if (!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & m_BitMasks.bitMasks[(squareIndex + 2)]) &&
			!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & m_BitMasks.bitMasks[(squareIndex + 1)] ))
		{
			if (!IsSquareInCheckByOtherColor(squareIndex + 1))
			{
				Move move{};
				move.startSquareIndex = squareIndex;
				move.targetSquareIndex = squareIndex + 2;
				move.moveType = MoveType::KingCastle;

				m_PossibleMoves.emplace_back(move);
			}
		}
	}
	if (canCastleQueenSide)
	{
		// If the king is in check, can't castle
		if (IsSquareInCheckByOtherColor(squareIndex)) return;


		if (!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & m_BitMasks.bitMasks[(squareIndex - 1)] ) &&
			!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & m_BitMasks.bitMasks[(squareIndex - 2)] ) &&
			!((m_BitBoards.blackPieces | m_BitBoards.whitePieces) & m_BitMasks.bitMasks[(squareIndex - 3)] ))
		{
			if (!IsSquareInCheckByOtherColor(squareIndex - 1))
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

void ChessBoard::CalculatePawnThreats(int squareIndex, uint64_t* threatMap)
{
	int verticalOffset{ m_WhiteToMove ? -8 : 8 };

	if (squareIndex % 8 != 0)
	{
		*threatMap |= m_BitMasks.bitMasks[(squareIndex + verticalOffset - 1)];
	}
	if (squareIndex % 8 != 7)
	{
		*threatMap |= m_BitMasks.bitMasks[(squareIndex + verticalOffset + 1)];
	}
	
}
void ChessBoard::CalculateKnightThreats(int squareIndex, uint64_t* threatMap)
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
			*threatMap |= m_BitMasks.bitMasks[newSquareIndex];
		}
	}
}
void ChessBoard::CalculateSlidingThreats(int squareIndex, uint64_t* threatMap, int startingOffsetIndex, int endOffsetIndex)
{

	for (int directionIndex{ startingOffsetIndex }; directionIndex < endOffsetIndex; ++directionIndex)
	{
		bool hitKing{ false };

		for (int multipliedIndex{ 1 }; multipliedIndex <= m_SlidingOffsets.distancesFromEdges[directionIndex][squareIndex]; ++multipliedIndex)
		{
			int currentOffset{ m_SlidingOffsets.squareOffsets[directionIndex] };

			*threatMap |= m_BitMasks.bitMasks[squareIndex + currentOffset * multipliedIndex];

			// Own pieces = break
			if (m_CurrentOwnPiecesBitBoard & m_BitMasks.bitMasks[(squareIndex + currentOffset * multipliedIndex)])
			{
				break;
			}
			// Enemy king = continue
			else if ((m_WhiteToMove ? m_BitBoards.whiteKing : m_BitBoards.blackKing) & m_BitMasks.bitMasks[(squareIndex + currentOffset * multipliedIndex)])
			{
				continue;
			}
			// any other Enemy Piece = break
			else if (m_CurrentOpponentPiecesBitBoard & m_BitMasks.bitMasks[(squareIndex + currentOffset * multipliedIndex)])
			{
				break;
			}
			// nothing = continue
			else
			{
				continue;
			}
		}


	}

}
void ChessBoard::CalculateKingThreats(int squareIndex, uint64_t* threatMap)
{
	for (int directionIndex{}; directionIndex < 8; ++directionIndex)
	{
		if (m_SlidingOffsets.distancesFromEdges[directionIndex][squareIndex] == 0) continue;
		int directionOffset{ m_SlidingOffsets.squareOffsets[directionIndex] };

		*threatMap |= m_BitMasks.bitMasks[squareIndex + directionOffset];
	}
}



void ChessBoard::CheckForIllegalMoves()
{
	std::vector<std::list<Move>::iterator> illegalIterators{};
	std::list<Move>::iterator it{};
	it = m_PossibleMoves.begin();
	//for (auto it{m_PossibleMoves.begin()}; it != m_PossibleMoves.end(); ++it)
	while(it != m_PossibleMoves.end())
	{
		MakeMove(*it, false);
	
		if (IsOtherKingInCheck())
		{
			illegalIterators.emplace_back(it);
		}
		UnMakeLastMove();
		std::advance(it, 1);
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
		if (kingBitBoard & m_BitMasks.bitMasks[index] )
		{
			kingSquareIndex = index;
			break;
		}
	}

	return IsSquareInCheckByOtherColor(kingSquareIndex);
}

bool ChessBoard::IsSquareInCheckByOtherColor(int squareIndex)
{
	return *m_pCurrentOpponentThreatMap & m_BitMasks.bitMasks[squareIndex];
}


void ChessBoard::CheckForGameEnd()
{
	//CheckForCheckmate();
	//CheckForFiftyMoveRule();
	//CheckForInsufficientMaterial();
	//CheckForRepetition();
}
void ChessBoard::CheckForCheckmate()
{
	if (m_PossibleMoves.size() == 0)
	{
		m_GameProgress = GameProgress::Draw;
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
		if (bitBoard & m_BitMasks.bitMasks[index] ) ++amount;
	}
	return amount;
}
void ChessBoard::CheckForRepetition()
{
	int amountOfCurrentApearences{0};
	for (int index{}; index < m_GameStateHistoryCounter; ++index)
	{
		if (m_GameStateHistory[index].bitBoards == m_BitBoards)
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

	gameState.gameProgress = m_GameProgress;

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


	m_GameStateHistory[++m_GameStateHistoryCounter] = gameState;
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
				if (index + 1 < FEN.size() && FEN[index + 1] != ' ')
				{
					m_HalfMoveClock = (currentChar - '0') * 10 + (FEN[index + 1] - '0');
					++index;
				}
				else
				{
					m_HalfMoveClock = currentChar - '0';
				}
				break;
			}
			case FENFields::FullmoveCounter:
			{
				if (index + 1 < FEN.size() && FEN[index + 1] != ' ')
				{
					m_FullMoveCounter = (currentChar - '0') * 10 + (FEN[index + 1] - '0');
					++index;
				}
				else
				{
					m_FullMoveCounter = currentChar - '0';
				}
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
			m_BitBoards.whitePawns |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'p':
		{
			m_BitBoards.blackPawns |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'N':
		{
			m_BitBoards.whiteKnights |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'n':
		{
			m_BitBoards.blackKnights |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'B':
		{
			m_BitBoards.whiteBishops |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'b':
		{
			m_BitBoards.blackBishops |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'R':
		{
			m_BitBoards.whiteRooks |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'r':
		{
			m_BitBoards.blackRooks |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'Q':
		{
			m_BitBoards.whiteQueens |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'q':
		{
			m_BitBoards.blackQueens |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'K':
		{
			m_BitBoards.whiteKing |= m_BitMasks.bitMasks[squareIndex] ;
			break;
		}
		case 'k':
		{
			m_BitBoards.blackKing |= m_BitMasks.bitMasks[squareIndex] ;
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

	m_EnPassantSquares |= m_BitMasks.bitMasks[squareIndex] ;
	index += 1; // The char is in notation "e3" which is 2 characters that have to be checked at the same time
}