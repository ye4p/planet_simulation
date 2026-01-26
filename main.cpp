#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <windows.h>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <vector>

static const int W = 1000;
static const int H = 800;
static uint32_t *gPixels = nullptr;
static BITMAPINFO gBmi = {};
static double maxHm = 40.0;
static double maxWm = 40.0;

const double pi = std::acos(-1);
double angleDegrees = 30;
double angleRadians = angleDegrees * pi / 180.0;
double ballXm = 0.0;
double ballYm = 0.0;
double velm = 30.0;

double velXm = velm * std::cos(angleRadians);
double velYm = velm * std::sin(angleRadians);
const double dt = 0.016;
const int radiusPixels = 10;
struct Vector2D
{
    double x;
    double y;
    Vector2D()
        : x(0.0), y(0.0)
    {}
    Vector2D(double x_, double y_)
        : x(x_), y(y_) 
    {}
    Vector2D operator+(const Vector2D& other) {
        Vector2D result;
        result.x= x+ other.x;
        result.y=y+other.y;
        return result;
    }
};
class CelestialBody
{
    double mass;
    double radius;
    Vector2D position;
    Vector2D velocity; 
};
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
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_TIMER:
    {

        Clear(0x00000000);
        // draw
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
    InvalidateRect(hwnd, nullptr, FALSE);
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    ShutdownFramebuffer();
    return 0;
}
