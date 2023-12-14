//-----------------------------------------------------------------
// Main Game File
// C++ Source - ChessEngine.cpp - version v7_02
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ChessEngine.h"																				

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
    GAME_ENGINE->SetFrameRate(50);

	// Set the keys that the game needs to listen to
	//tstringstream buffer;
	//buffer << _T("KLMO");
	//buffer << (TCHAR) VK_LEFT;
	//buffer << (TCHAR) VK_RIGHT;
	//GAME_ENGINE->SetKeyList(buffer.str());

	m_pDrawableChessBoard = std::make_unique<DrawableChessBoard>();
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

	m_pDrawableChessBoard->Draw(m_CurrentSelectedSquare);
}

void ChessEngine::Tick()
{
	if (m_pDrawableChessBoard->GetFirstFrameGameEnd())
	{
		switch (m_pDrawableChessBoard->GetGameState())
		{
			case GameState::Draw:
			{
				tstring s{ L"The game has Drawn" };
				GAME_ENGINE->MessageBox(s);

				break;
			}
			case GameState::WhiteWon:
			{
				tstring s{ L"White Has Won!" };
				GAME_ENGINE->MessageBox(s);

				break;
			}
			case GameState::BlackWon:
			{
				tstring s{ L"Black Has Won!" };
				GAME_ENGINE->MessageBox(s);

				break;
			}
			default:
				break;
		}
		m_pDrawableChessBoard->SetFirstFrameGameEnd(false);
	}
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
}

void ChessEngine::CallAction(Caller* callerPtr)
{
	// Insert the code that needs to be executed when a Caller has to perform an action
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




