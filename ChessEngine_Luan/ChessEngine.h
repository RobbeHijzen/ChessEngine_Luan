//-----------------------------------------------------------------
// Main Game  File
// C++ Header - ChessEngine.h - version v7_02			
//-----------------------------------------------------------------

#ifndef CHESSENGINE_
#define CHESSENGINE_

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------

#include "Resource.h"	
#include "GameEngine.h"
#include "AbstractGame.h"
#include "HelperStructs.h"

#include "ChessBoard.h"
#include "DrawableChessBoard.h"

//-----------------------------------------------------------------
// ChessEngine Class																
//-----------------------------------------------------------------
class ChessEngine : public AbstractGame, public Callable
{
public:				
	//---------------------------
	// Constructor(s) and Destructor
	//---------------------------
	ChessEngine();

	virtual ~ChessEngine() override;

	//---------------------------
	// Disabling copy/move constructors and assignment operators   
	//---------------------------
	ChessEngine(const ChessEngine& other) = delete;
	ChessEngine(ChessEngine&& other) noexcept = delete;
	ChessEngine& operator=(const ChessEngine& other) = delete;
	ChessEngine& operator=(ChessEngine&& other) noexcept = delete;

	//---------------------------
	// General Methods
	//---------------------------
	void Initialize(HINSTANCE hInstance) override;
	void Start() override;
	void End() override;
	void Paint(RECT rect) override;
	void Tick() override;
	void MouseButtonAction(bool isLeft, bool isDown, int x, int y, WPARAM wParam) override;
	void MouseWheelAction(int x, int y, int distance, WPARAM wParam) override;
	void MouseMove(int x, int y, WPARAM wParam) override;
	void CheckKeyboard() override;
	void KeyPressed(TCHAR cKey) override;
	
	void CallAction(Caller* callerPtr) override;

private:
	// -------------------------
	// Datamembers
	// -------------------------

	std::unique_ptr<DrawableChessBoard> m_pDrawableChessBoard{};

	bool m_FirstFrameMousePress{ true };
	bool m_HasASquareSelected{ false };
	int m_CurrentSelectedSquare{};

	int GetIndexFromPosition(Point2i position);
};

#endif
