#include <Windows.h>

#include "ndq/window.h"

namespace ndq
{
    void _SetDeviceHwndAndSize(void* hwnd, unsigned width, unsigned height);
    void _DevicePresent();
    void _DeviceFinalize();
}

namespace ndq
{
    LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        LRESULT Result;
        switch (message)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            Result = 0;
            break;
        default:
            Result = DefWindowProcW(hWnd, message, wParam, lParam);
            break;
        }
        return Result;
    }

    float _GetElapsedTime(LARGE_INTEGER& frequency, LARGE_INTEGER& lastTime)
    {
        LARGE_INTEGER CurrentTime;
        QueryPerformanceCounter(&CurrentTime);
        float ElapsedTime = static_cast<float>(CurrentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
        lastTime = CurrentTime;
        return ElapsedTime;
    }

    int IWindow::Run()
    {
        WNDCLASSEXW Wcex{};
        Wcex.cbSize = sizeof(WNDCLASSEXW);
        Wcex.style = CS_HREDRAW | CS_VREDRAW;
        Wcex.lpfnWndProc = _WndProc;
        Wcex.hInstance = GetModuleHandleW(nullptr);
        Wcex.hIcon = LoadIconW(Wcex.hInstance, L"IDI_ICON");
        Wcex.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
        Wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        Wcex.lpszClassName = L"ndq";
        Wcex.hIconSm = LoadIconW(Wcex.hInstance, L"IDI_ICON");
        RegisterClassExW(&Wcex);

        auto Stype = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
        RECT RC = { 0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height) };
        AdjustWindowRect(&RC, Stype, FALSE);
        auto hwnd = CreateWindowExW(0, L"ndq", Title, Stype, CW_USEDEFAULT, CW_USEDEFAULT, RC.right - RC.left, RC.bottom - RC.top, nullptr, nullptr, Wcex.hInstance, nullptr);
        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);

        _SetDeviceHwndAndSize(&hwnd, Width, Height);

        Initialize();

        LARGE_INTEGER Frequency;
        LARGE_INTEGER LastTime;
        QueryPerformanceFrequency(&Frequency);
        QueryPerformanceCounter(&LastTime);

        MSG Msg = {};
        while (WM_QUIT != Msg.message)
        {
            if (PeekMessageW(&Msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&Msg);
                DispatchMessageW(&Msg);
            }
            else
            {
                auto ElapsedTime = _GetElapsedTime(Frequency, LastTime);
                Update(ElapsedTime);

                _DevicePresent();
            }
        }

        Finalize();
        _DeviceFinalize();

        return static_cast<int>(Msg.wParam);
    }
}