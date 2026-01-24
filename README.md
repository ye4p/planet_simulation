# planet_simulation
#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <windows.h>
#include <cstdlib>
#include <cstdint>

using namespace std;

static const int W = 800;
static const int H = 800;
static int squareW = 600;
static int squareH = 600;
static uint32_t *gPixels = nullptr;
static BITMAPINFO gBmi = {};
static double maxHm = 40.0;
static double maxWm = 40.0;

const double pi = acos(-1);
double angleDegrees = 30;
double angleRadians = angleDegrees * pi / 180.0;
double ballXm = 0.0;
double ballYm = 0.0;
double velm = 30.0;

double velXm = velm * cos(angleRadians);
double velYm = velm * sin(angleRadians);
const double dt = 0.016;
const int radiusPixels = 10;
const double r = radiusPixels * (maxWm / squareW);
void InitFramebuffer()
{
    gPixels = new uint32_t[W * H];

    gBmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    gBmi.bmiHeader.biWidth = W;
    gBmi.bmiHeader.biHeight = -H;
    gBmi.bmiHeader.biPlanes = 1;
    gBmi.bmiHeader.biBitCount = 32;
    gBmi.bmiHeader.biCompression = BI_RGB;
}
void ShutdownFramebuffer()
{
    delete[] gPixels;
    gPixels = nullptr;
}
void Clear(uint32_t color)
{
    for (int i = 0; i < W * H; i++)
    {
        gPixels[i] = color;
    }
}
void PutPixel(int x, int y, uint32_t color)
{
    if (x < 0 || x >= W || y < 0 || y >= H)
    {
        return;
    }
    gPixels[y * W + x] = color;
}
void drawSquare()
{
    for (int i = 0; i < squareW; i++)
    {
        PutPixel((W - squareW) / 2 + i, (H - squareH) / 2, 0x00FFFFFF);
    }
    for (int i = 0; i < squareW; i++)
    {
        PutPixel((W - squareW) / 2, (H - squareH) / 2 + i, 0x00FFFFFF);
    }
    for (int i = 0; i < squareW; i++)
    {
        PutPixel((W - squareW) / 2 + i, (H - squareH) / 2 + squareH, 0x00FFFFFF);
    }
    for (int i = 0; i < squareW; i++)
    {
        PutPixel((W - squareW) / 2 + squareW, (H - squareH) / 2 + i, 0x00FFFFFF);
    }
}
void PutPixel2(int ind, uint32_t color)
{
    if (ind < 0 || ind >= W * H)
    {
        return;
    }
    gPixels[ind] = color;
}
void drawBall(double xm, double ym, uint32_t color)
{
    int xp = (int)(xm * (squareW / maxWm) + (W - squareW) / 2);
    int yp = (int)((maxHm - ym) * (squareH / maxHm) + (H - squareH) / 2);
    PutPixel(xp, yp, color);
    // PutPixel(xp+1, yp, color);
    // PutPixel(xp+1, yp-1, color);
    // PutPixel(xp+1, yp+1, color);
    // PutPixel(xp, yp+1, color);
    // PutPixel(xp, yp-1, color);
    // PutPixel(xp-1, yp-1, color);
    // PutPixel(xp-1, yp, color);
    // PutPixel(xp-1, yp+1, color);
    for (int layer = 0; layer < radiusPixels * 2 + 1; layer++)
    {
        for (int layerX = 0; layerX < radiusPixels * 2 + 1; layerX++)
        {
            PutPixel(xp - radiusPixels + layerX, yp - radiusPixels + layer, color);
        }
    }
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_TIMER:
    {
        ballXm += velXm * dt;
        ballYm += velYm * dt;
        if (ballXm - r < 0)
        {
            ballXm = r;
            velXm = -velXm;
        }
        if (ballXm + r > maxWm)
        {
            ballXm = maxWm - r;
            velXm = -velXm;
        }
        if (ballYm - r < 0)
        {
            ballYm = r;
            velYm = -velYm;
        }
        if (ballYm + r > maxHm)
        {
            ballYm = maxHm - r;
            velYm = -velYm;
        }

        Clear(0x00000000);
        drawSquare();
        drawBall(ballXm, ballYm, 0x00FFFFFF);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        StretchDIBits(
            hdc,
            0, 0, W, H,
            0, 0, W, H,
            gPixels,
            &gBmi,
            DIB_RGB_COLORS,
            SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main()
{
    InitFramebuffer();
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"MySimWindowClass";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"Simulation",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        W, H,
        nullptr, nullptr,
        wc.hInstance, nullptr);
    ShowWindow(hwnd, SW_SHOW);
    SetTimer(hwnd, 1, 16, nullptr);
    MSG msg;

    // top line: (squareW*(H-squareH)/2 +(W-squareW)/2 < i && squareW*(H-squareH)/2 +(W-squareW)/2 >i)

    // i < W || i % W == 0 || i % W == W - 1 || i >= W * (H - 1)
    drawSquare();
    InvalidateRect(hwnd, nullptr, FALSE);
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    ShutdownFramebuffer();
    return 0;
}
Here is my current c++ project. I want to create new project with simulating planet rotation, but I want it to be with more c++ stuff so that I can learn more of c++ and that's main goal. Tell me what do I need to create in my new project and which c++ stuff I need to use to learn it. and we can also have more complicated physics and math. I want you to clearly and detailed establish goals and objectives so that I know that I need to work towards and specifically say what I need to use