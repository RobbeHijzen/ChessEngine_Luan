#include "ChessBoard.h"
#include <string>

ChessBoard::ChessBoard()
{
	std::string FEN{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" };
	SetBitboardsFromFEN(FEN);
}

void ChessBoard::MakeMove(int startSquareIndex, int targetSquareIndex)
{
	uint64_t* startBitBoard{GetBitboardFromSquare(startSquareIndex)};
	if ((*startBitBoard) == 0) return;


	uint64_t* targetBitBoard{ GetBitboardFromSquare(targetSquareIndex) };
	if ((*targetBitBoard) != 0)
	{
		*targetBitBoard &= ~(static_cast<unsigned long long>(1) << targetSquareIndex);
	}


	*startBitBoard &= ~(static_cast<unsigned long long>(1) << startSquareIndex);
	*startBitBoard |= static_cast<unsigned long long>(1) << targetSquareIndex;

	

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

