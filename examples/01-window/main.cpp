#include "ndq/window.h"

using namespace ndq;

struct Window : IWindow
{
    Window()
    {
        Title = L"Window";
    }

    void Initialize() {}
    void Update(float) {}
    void Finalize() {}
};

WIN_MAIN_MACRO(Window)