#include <Windows.h>

#include "ndq/window.h"

struct Window : public ndq::IWindow
{
    Window()
    {
        Title = L"Window";
    }

    void Initialize() {}
    void Update(float t) {}
    void Finalize() {}
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    Window MyWindow;
    return MyWindow.Run();
    return 0;
}