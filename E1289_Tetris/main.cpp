#include <iostream>
#include <Windows.h>

// Blocks
std::wstring wideTetromino[7];

// Playing Field
int nFieldWidth = 12;
int nFieldHeight = 22;
unsigned char* pField = nullptr;

// Default Command Prompt Screen Size
int nScreenWidth = 120;		// x (Columns)
int nScreenHeight = 40;		// y (Rows)

int Rotate(int gridX, int gridY, int rotation)
{
	switch (rotation % 4)
	{
	case 0: return gridY * 4 + gridX;			// 0 degrees
	case 1: return 12 + gridY - (gridX * 4);	// 90 degrees
	case 2: return 15 - (gridY * 4) - gridX;	// 180 degrees
	case 3: return 3 - gridY + (gridX * 4);		// 270 degrees
	}
	return 0;
}

int main()
{
	// Create assets
	wideTetromino[0].append(L"..X.");
	wideTetromino[0].append(L"..X.");
	wideTetromino[0].append(L"..X.");
	wideTetromino[0].append(L"..X.");

	wideTetromino[1].append(L"..X.");
	wideTetromino[1].append(L".XX.");
	wideTetromino[1].append(L".X..");
	wideTetromino[1].append(L"....");

	wideTetromino[2].append(L".X..");
	wideTetromino[2].append(L".XX.");
	wideTetromino[2].append(L"..X.");
	wideTetromino[2].append(L"....");

	wideTetromino[3].append(L"....");
	wideTetromino[3].append(L".XX.");
	wideTetromino[3].append(L".XX.");
	wideTetromino[3].append(L"....");

	wideTetromino[4].append(L"..X.");
	wideTetromino[4].append(L".XX.");
	wideTetromino[4].append(L"..X.");
	wideTetromino[4].append(L"....");

	wideTetromino[5].append(L"....");
	wideTetromino[5].append(L".XX.");
	wideTetromino[5].append(L"..X.");
	wideTetromino[5].append(L"..X.");

	wideTetromino[6].append(L"....");
	wideTetromino[6].append(L".XX.");
	wideTetromino[6].append(L".X..");
	wideTetromino[6].append(L".X..");

	// Initialize the playing field
	pField = new unsigned char[nFieldWidth * nFieldHeight]; // Create play field buffer
	// Board Boundary
	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

	// Command Line Screen Buffer Effect
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	bool bGameOver = false;

	// Main Game Loop
	while (!bGameOver)
	{
		// Game Timing =====================================================================

		// Input ===========================================================================

		// Game Logic ======================================================================

		// Render Output ===================================================================

		// Draw Game Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 3) * nScreenWidth + (x + 55)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];

		// Write to console output buffer
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	return 0;
}