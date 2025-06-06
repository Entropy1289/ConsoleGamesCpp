#include <iostream>
#include <thread>
#include <chrono>
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

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// Get index into piece
			int pi = Rotate(px, py, nRotation);

			// Get index into field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					if (wideTetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
						return false;
				}
		}

	return true;
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

	// Game Logic Stuff
	bool bGameOver = false;

	int nCurrentPiece = 1;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	bool bKey[4];

	// Main Game Loop
	while (!bGameOver)
	{
		// Game Timing =====================================================================
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		// Input ===========================================================================
		for (int k = 0; k < 4; k++)
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;


		// Game Logic ======================================================================
		if (bKey[0])
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
				nCurrentX = nCurrentX + 1;
		if (bKey[1])
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
				nCurrentX = nCurrentX - 1;
		if (bKey[2])
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY = nCurrentY + 1;


		// Render Output ===================================================================

		// Draw Game Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 3) * nScreenWidth + (x + 55)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];

		// Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (wideTetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					screen[(nCurrentY + py + 3) * nScreenWidth + (nCurrentX + px + 55)] = nCurrentPiece + 65;

		// Write to console output buffer
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	return 0;
}