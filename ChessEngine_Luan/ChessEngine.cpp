//-----------------------------------------------------------------
// Main Game File
// C++ Source - ChessEngine.cpp - version v7_02
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ChessEngine.h"
#include <chrono>

//-----------------------------------------------------------------
// ChessEngine methods																				
//-----------------------------------------------------------------

ChessEngine::ChessEngine()
{
	// nothing to create

	
}

ChessEngine::~ChessEngine()																						
{
	// nothing to destroy
}


void ChessEngine::Initialize(HINSTANCE hInstance)			
{
	// Set the required values
	AbstractGame::Initialize(hInstance);
	GAME_ENGINE->SetTitle(_T("ChessEngine_Luan (Robbe Hijzen - 2DAE18)"));					
	GAME_ENGINE->RunGameLoop(true);		
	// Set the optional values
	GAME_ENGINE->SetWidth(1440);
	GAME_ENGINE->SetHeight(810);
    GAME_ENGINE->SetFrameRate(1000);

	// Set the keys that the game needs to listen to
	tstringstream buffer;
	buffer << _T("ZMPR");
	//buffer << _T("KLMO");
	//buffer << (TCHAR) VK_LEFT;
	//buffer << (TCHAR) VK_RIGHT;
	GAME_ENGINE->SetKeyList(buffer.str());

	
	m_pFont = std::make_unique<Font>(L"Arial", true, false, false, 50);

	m_pDrawableChessBoard = std::make_unique<DrawableChessBoard>();
	m_pChessAI_White = std::make_unique<ChessAI_V1_AlphaBeta>(m_pDrawableChessBoard.get(), true);
	m_pChessAI_Black = std::make_unique<ChessAI_V2_AlphaBeta>(m_pDrawableChessBoard.get(), false);
}

void ChessEngine::Start()
{
	// Insert the code that needs to be executed at the start of the game
}

void ChessEngine::End()
{
	// Insert the code that needs to be executed at the closing of the game
}

void ChessEngine::Paint(RECT rect)
{
	// Insert paint code 
	GAME_ENGINE->DrawSolidBackground(RGB(20, 20, 20));

	m_pDrawableChessBoard->Draw(m_CurrentSelectedSquare, m_InMoveGeneration);

	std::wstring s1{ std::to_wstring(m_pDrawableChessBoard->GetTotalAmount()) };
	std::wstring s2{ std::to_wstring(m_pDrawableChessBoard->GetCaptureAmount()) };
	std::wstring s3{ std::to_wstring(m_pDrawableChessBoard->GetEnPassantAmount()) };
	std::wstring s4{ std::to_wstring(m_pDrawableChessBoard->GetCastleAmount()) };
	std::wstring s5{ std::to_wstring(m_pDrawableChessBoard->GetPromotionAmount()) };
	std::wstring s6{ std::to_wstring(m_pDrawableChessBoard->GetCheckAmount()) };
	std::wstring s7{ std::to_wstring(m_pChessAI_Black->GetCurrentMoveTimer()) };
	GAME_ENGINE->SetFont(m_pFont.get());
	GAME_ENGINE->DrawString(s1, 100, 100);
	GAME_ENGINE->DrawString(s2, 100, 150);
	GAME_ENGINE->DrawString(s3, 100, 200);
	GAME_ENGINE->DrawString(s4, 100, 250);
	GAME_ENGINE->DrawString(s5, 100, 300);
	GAME_ENGINE->DrawString(s6, 100, 350);
	GAME_ENGINE->DrawString(s7, 100, 450);

}

void ChessEngine::Tick()
{
	
	
}

void ChessEngine::MouseButtonAction(bool isLeft, bool isDown, int x, int y, WPARAM wParam)
{	
	// Insert the code that needs to be executed when the game registers a mouse button action

	// Example:
	if (isLeft == true && isDown == true && m_FirstFrameMousePress) // is it a left mouse click?
	{	
		
		if (!m_HasASquareSelected)
		{
			m_FirstFrameMousePress = false;

			m_HasASquareSelected = true;
			m_CurrentSelectedSquare = GetIndexFromPosition({ x, y });
		}
		else
		{
			int targetSquare{ GetIndexFromPosition({ x, y }) };
			
			Move move{ m_pDrawableChessBoard->GetMoveFromSquares(m_CurrentSelectedSquare, targetSquare) };
			if (m_pDrawableChessBoard->IsLegalMove(move))
			{
				m_pDrawableChessBoard->MakeMove(move);
				m_HasASquareSelected = false;
				m_CurrentSelectedSquare = -1;
				m_MakeNextMove = true;


			}
			else
			{
				m_CurrentSelectedSquare = GetIndexFromPosition({ x, y });
			}
		}
	}
	else if (!(isLeft && isDown))
	{
		m_FirstFrameMousePress = true;
	}
	
}
void ChessEngine::MouseWheelAction(int x, int y, int distance, WPARAM wParam)
{	
	// Insert the code that needs to be executed when the game registers a mouse wheel action
}

void ChessEngine::MouseMove(int x, int y, WPARAM wParam)
{	
	// Insert the code that needs to be executed when the mouse pointer moves across the game window

	/* Example:
	if ( x > 261 && x < 261 + 117 ) // check if mouse position is within x coordinates of choice
	{
		if ( y > 182 && y < 182 + 33 ) // check if mouse position also is within y coordinates of choice
		{
			GAME_ENGINE->MessageBox(_T("Da mouse wuz here."));
		}
	}
	*/
}
void ChessEngine::CheckKeyboard()
{	
	// Here you can check if a key of choice is held down
	// Is executed once per frame if the Game Loop is running 

	/* Example:
	if (GAME_ENGINE->IsKeyDown(_T('K'))) xIcon -= xSpeed;
	if (GAME_ENGINE->IsKeyDown(_T('L'))) yIcon += xSpeed;
	if (GAME_ENGINE->IsKeyDown(_T('M'))) xIcon += xSpeed;
	if (GAME_ENGINE->IsKeyDown(_T('O'))) yIcon -= ySpeed;
	*/
}
void ChessEngine::KeyPressed(TCHAR cKey)
{	
	// DO NOT FORGET to use SetKeyList() !!

	// Insert the code that needs to be executed when a key of choice is pressed
	// Is executed as soon as the key is released
	// You first need to specify the keys that the game engine needs to watch by using the SetKeyList() method

	/* Example:
	switch (cKey)
	{
	case _T('K'): case VK_LEFT:
		GAME_ENGINE->MessageBox(_T("Moving left."));
		break;
	case _T('L'): case VK_DOWN:
		GAME_ENGINE->MessageBox(_T("Moving down."));
		break;
	case _T('M'): case VK_RIGHT:
		GAME_ENGINE->MessageBox(_T("Moving right."));
		break;
	case _T('O'): case VK_UP:
		GAME_ENGINE->MessageBox(_T("Moving up."));
		break;
	case VK_ESCAPE:
		GAME_ENGINE->MessageBox(_T("Escape menu."));
	}
	*/

	switch (cKey)
	{
		case _T('P'):
		{
			m_GameIsPaused = !m_GameIsPaused;
			break;
		}
		case _T('R'):
		{
			m_MakeNextMove = true;
			while (!m_GameHasEnded)
			{
				HandleGameEnd();

				if (!m_GameHasEnded)
				{
					if (!m_GameIsPaused && m_MakeNextMove)
					{
						m_MakeNextMove = false;

						if (m_pDrawableChessBoard->GetWhiteToMove() == m_pChessAI_Black->IsControllingWhite())
						{
							m_pDrawableChessBoard->MakeMove(m_pChessAI_Black->GetAIMove());
							m_MakeNextMove = true;
						}
						//else if (m_pDrawableChessBoard->GetWhiteToMove() == m_pChessAI_White->IsControllingWhite())
						//{
						//	m_pDrawableChessBoard->MakeMove(m_pChessAI_White->GetAIMove());
						//	m_MakeNextMove = true;
						//}
					}
				}
			}
			break;
		}
		case _T('Z'):
		{
			m_pDrawableChessBoard->UnMakeLastMove();
			break;
		}
		case _T('M'):
		{
			auto lastUpdate{ std::chrono::steady_clock::now() };

			
			m_InMoveGeneration = true;
			m_MoveGenerationTestAmount = m_pDrawableChessBoard->StartMoveGenerationTest(3);

			auto now = std::chrono::steady_clock::now();
			m_MoveGenerationTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
			lastUpdate = now;
			break;
		}
	}
}
void ChessEngine::CallAction(Caller* callerPtr)
{
	// Insert the code that needs to be executed when a Caller has to perform an action
}

void ChessEngine::HandleGameEnd()
{
	switch (m_pDrawableChessBoard->GetGameProgress())
	{
	case GameProgress::Draw:
	{
		m_GameHasEnded = true;

		tstring s{ L"The game has Drawn" };
		GAME_ENGINE->MessageBox(s);

		break;
	}
	case GameProgress::WhiteWon:
	{
		m_GameHasEnded = true;

		tstring s{ L"White Has Won!" };
		GAME_ENGINE->MessageBox(s);

		break;
	}
	case GameProgress::BlackWon:
	{
		m_GameHasEnded = true;

		tstring s{ L"Black Has Won!" };
		GAME_ENGINE->MessageBox(s);

		break;
	}
	default:
		break;
	}	
}

int ChessEngine::GetIndexFromPosition(Point2i position)
{
	Point2i relativeBoardPos{ position - m_pDrawableChessBoard->GetTopLeftPos() };
	
	// Check if in bounds
	if (relativeBoardPos.x < 0 || relativeBoardPos.y < 0) return -1;
	if (relativeBoardPos.x > m_pDrawableChessBoard->GetCellSize() * 8 || relativeBoardPos.y > m_pDrawableChessBoard->GetCellSize() * 8) return -1;
	//-------------

	int row{ relativeBoardPos.y / m_pDrawableChessBoard->GetCellSize() };
	int column{ relativeBoardPos.x / m_pDrawableChessBoard->GetCellSize() };

	return row * 8 + column;

}




