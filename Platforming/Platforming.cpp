#include <Windows.h>
#include <iostream>
#include <chrono>
#include <vector>
using namespace std;

const int nScreenWidth = 120;
const int nScreenHeight = 40;

float fPlayerX = 3.0f;
float fPlayerY = 3.0f;
float fPlayerA = 0.0f;
float fPlayerHeight = 0.0f;
float fPlayerSpeed = 5.0f;

float fLookAngle = (float)3.14159 / 3;
float fLookRange = 15.0f;

int nMapWidth = 16;
int nMapHeight = 16;

float fObjHeight = 4.0f;

int main()
{
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight + 1];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    wstring map;
    
    map += L"################";
    map += L"#..............#";
    map += L"#############..#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#.....##.......#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#.........U....#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"################";

    screen[nScreenWidth * nScreenHeight] = 0;

    auto TimePoint1 = chrono::system_clock::now();
    auto TimePoint2 = chrono::system_clock::now();

    while (true) {
        TimePoint2 = chrono::system_clock::now();
        chrono::duration <float> elapsedTime = TimePoint2 - TimePoint1;
        TimePoint1 = TimePoint2;
        float fElapsedTime = elapsedTime.count();
        bool bSpace = false;

        
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
            fPlayerA -= 1.5f * fElapsedTime;
        }
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
            fPlayerA += 1.5f * fElapsedTime;
        }
        if(GetKeyState(VK_SPACE) & 0x8000) {
            //FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); //clear input buffer
            fPlayerHeight = 5.0f;
        }
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
            fPlayerHeight = -2.0f;
            fPlayerSpeed = 1.0f;
        }
        if (!(GetAsyncKeyState(VK_CONTROL) & 0x8000) && (fPlayerHeight < 0.0f)) {
                fPlayerHeight = 0.0f;
                fPlayerSpeed = 5.0f;
        }
        if (!(GetAsyncKeyState(VK_SPACE) & 0x8000) && (fPlayerHeight > 0.0f)) {
                Sleep(250);
                fPlayerHeight = 0.0f;
                fPlayerSpeed = 5.0f;
                bSpace = false;
        }
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
            fPlayerX += sinf(fPlayerA) * fPlayerSpeed * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * fPlayerSpeed * fElapsedTime;
            if (map[(int)fPlayerX + (int)fPlayerY * nMapWidth] == '#' || map[(int)fPlayerX + (int)fPlayerY * nMapWidth] == 'U') {
                fPlayerX -= sinf(fPlayerA) * fPlayerSpeed * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * fPlayerSpeed * fElapsedTime;
            }
        }
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
            fPlayerX -= sinf(fPlayerA) * fPlayerSpeed * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * fPlayerSpeed * fElapsedTime;
            if (map[(int)fPlayerX + (int)fPlayerY * nMapWidth] == '#' || map[(int)fPlayerX + (int)fPlayerY * nMapWidth] == 'U') {
                fPlayerX += sinf(fPlayerA) * fPlayerSpeed * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * fPlayerSpeed * fElapsedTime;
            }
        }
        
        

        for (int x = 0; x < nScreenWidth; x++) {
            /*______НАХОДИМ РАССТОЯНИЕ ДО СТЕНЫ______*/
            float fVectorDirection = (fPlayerA - fLookAngle / 2.0) + ((float)x / (float)nScreenWidth) * fLookAngle;
            
            float fVectorDirectX = sinf(fVectorDirection);
            float fVectorDirectY = cosf(fVectorDirection);

            float fDistance = 0.0f;
            bool isHitWall = false;
            float fObjDistance = 0.0f;
            bool isHitObj = false;

            while (!isHitWall && fDistance < fLookRange) {
                fDistance += 0.1f;
                int nHitX = (int)(fPlayerX + fVectorDirectX * fDistance);
                int nHitY = (int)(fPlayerY + fVectorDirectY * fDistance);
                if (nHitX < 0 || nHitX >= nMapWidth || nHitY < 0 || nHitY >= nMapHeight) {
                    fDistance = fLookRange;
                    isHitWall = true;
                }
                else if (map[nHitX + nHitY*nMapWidth] == '#') {
                    isHitWall = true;
                }
                else if (map[nHitX + nHitY * nMapWidth] == 'U') {
                    fObjDistance = fDistance;
                    isHitObj = true;
                }
            }
            //fDistance - расснояние до стены для одного луча
            int nCeiling = (float)(nScreenHeight / 2.0f + fPlayerHeight) - nScreenHeight / ((float)fDistance);
            int nFloor = (float)(nScreenHeight / 2.0f + fPlayerHeight) + nScreenHeight / ((float)fDistance);
            int nObjFloor = (float)(nScreenHeight / 2.0f + fPlayerHeight) + nScreenHeight / ((float)fObjDistance);
            int nObjCeiling = (float)(nScreenHeight / 2.0f + fPlayerHeight) - nScreenHeight / ((float)fObjDistance) + (fObjHeight+nScreenHeight)/((float)fObjDistance);
            
            short nPixel, nPixelObj;
            if (fDistance <= fLookRange / 2.5f)        nPixel = 0x2588;  
            else if (fDistance < fLookRange / 2.0f)    nPixel = 0x2593;
            else if (fDistance < fLookRange / 1.5f)    nPixel = 0x2592;
            else if (fDistance < fLookRange)           nPixel = 0x2591;
            else                                       nPixel = ' ';

            

            if (fObjDistance <= fLookRange / 2.5f)        nPixelObj = 0x2587;
            else if (fObjDistance < fLookRange / 2.0f)    nPixelObj = 0x2586;
            else if (fObjDistance < fLookRange / 1.5f)    nPixelObj = 0x2585;
            else if (fObjDistance < fLookRange)           nPixelObj = 0x2584;
            else                                          nPixelObj = ' ';

            for (int y = 0; y < nScreenHeight; y++) {
                
                if (y <= nCeiling) screen[x + y * nScreenWidth] = ' ';
                else if (y > nCeiling && y <= nFloor) screen[x + y * nScreenWidth] = nPixel;
                else {
                    float b = 1.0f - ((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f);
                    if (b < 0.25) nPixel = '#';
                    else if (b < 0.5) nPixel = 'X';
                    else if (b < 0.75) nPixel = '~';
                    else if (b < 0.9) nPixel = '-';
                    else nPixel = ' ';
                    screen[y * nScreenWidth + x] = nPixel;
                }
                if (isHitObj) {
                    if (y > nObjCeiling && y <= nObjFloor) screen[x + y * nScreenWidth] = nPixelObj;
                }

            }

            
        }
        for (int j = 0; j < nMapHeight; j++) {
            for (int i = 0; i < nMapWidth; i++) {
                screen[i + j * nScreenWidth] = map[i + j * nMapWidth];
                if ((int)fPlayerX == i && (int)fPlayerY == j) screen[i + j * nScreenWidth] = 'p';
            }
        }

        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);
    }

   
}

