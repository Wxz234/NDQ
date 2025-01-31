#include "ndq/platform/window.h"
#include "ndq/render/renderer.h"

using namespace ndq;

struct Window : IWindow
{
    Window()
    {
        Title = L"Model";
    }

    void Initialize()
    {
        //CreateRenderer(&pRenderer);
    }

    void Update(float)
    {

    }

    void Finalize()
    {
        //pRenderer->Release();
    }

    //IRenderer* pRenderer = nullptr;
};

WIN_MAIN_MACRO(Window)