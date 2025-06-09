#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <Windows.h>

// Default Command Prompt Screen Size
int nScreenWidth = 120;		// x (Columns)
int nScreenHeight = 40;		// y (Rows)

// Blocks
std::wstring wideTetromino[7];

// Playing Field
int nFieldWidth = 12;
int nFieldHeight = 22;
unsigned char* pField = nullptr;


int Rotate(int gridX, int gridY, int rotation)
{
	int pi = 0;
	switch (rotation % 4)
	{
	case 0: pi = gridY * 4 + gridX; break;			// 0 degrees
	case 1: pi = 12 + gridY - (gridX * 4); break; 	// 90 degrees
	case 2: pi = 15 - (gridY * 4) - gridX; break; 	// 180 degrees
	case 3: pi = 3 - gridY + (gridX * 4); break;		// 270 degrees
	}
	return pi;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	// All field cells >0 are occupied
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// Get index into piece
			int pi = Rotate(px, py, nRotation);

			// Get index into field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					// In Bounds so do collision check
					if (wideTetromino[nTetromino][pi] != L'.' && pField[fi] != 0)
						return false;
				}
			}
		}

	return true;
}

int main()
{
	// Create Screen Buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// Create assets, you can also do it in one continuous line
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

	// Game Logic Stuff
	int nCurrentPiece = 1;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	bool bKey[4];
	bool bRotateHold = false;

	int nSpeed = 20;
	int nSpeedCount = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;
	std::vector<int> vLines;

	bool bGameOver = false;

	// Main Game Loop
	while (!bGameOver)
	{
		// Game Timing =====================================================================
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		nSpeedCount++;
		bForceDown = (nSpeedCount == nSpeed);

		// Input ===========================================================================
		for (int k = 0; k < 4; k++)
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;


		// Game Logic ======================================================================
		nCurrentX += (bKey[0] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))) ? 1 : 0;
		nCurrentX -= (bKey[1] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))) ? 1 : 0;
		nCurrentY += (bKey[2] && (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))) ? 1 : 0;

		if (bKey[3]) {
			nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else {
			bRotateHold = false;
		}

		if (bForceDown) {

			// Update difficulty every 50 pieces
			nSpeedCount = 0;
			nPieceCount++;
			if (nPieceCount % 50 == 0)
				if (nSpeed >= 10) nSpeed--;

			// Test if piece can be moved down
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			// If it can't...
			else {
				// Lock the current piece in the field
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (wideTetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

				// Check have we got any lines base on our last piece
				for (int py = 0; py < 4; py++)
				{
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine)
						{
							// Remove Line, set to =
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							vLines.push_back(nCurrentY + py);
						}
					}
				}

				nScore += 25;
				if (!vLines.empty()) nScore += (1 << vLines.empty()) * 100;

				// Choose next piece
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				// If piece does not fit
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
			nSpeedCount = 0;
		}

		// Render Output ===================================================================

		// Draw Game Field
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y + 3) * nScreenWidth + (x + 55)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];

		// Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (wideTetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
					screen[(nCurrentY + py + 3) * nScreenWidth + (nCurrentX + px + 55)] = nCurrentPiece + 65;

		// Draw Score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 58], 16, L"Score: %8d", nScore);

		// Animate Line Completion
		if (!vLines.empty())
		{
			// Display Frame
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
			std::this_thread::sleep_for(std::chrono::milliseconds(400));

			for (auto &v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}
			vLines.clear();
		}

		// Write to console output buffer
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	// Game Over
	CloseHandle(hConsole);
	std::cout << "Game Over! Score: " << nScore << std::endl;
	system("pause");

	return 0;
}