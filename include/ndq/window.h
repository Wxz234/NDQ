#pragma once

namespace ndq
{
    class IWindow
    {
    public:
        int Run();
        virtual void Initialize() = 0;
        virtual void Finalize() = 0;
        virtual void Update(float t) = 0;

        unsigned Width = 800;
        unsigned Height = 600;
        const wchar_t* Title = L"ndq";
    };
}