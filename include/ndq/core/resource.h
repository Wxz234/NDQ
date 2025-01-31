#pragma once

namespace ndq
{
    class IRefCounted
    {
    public:
        virtual unsigned long AddRef() = 0;
        virtual void Release() = 0;
    };
}