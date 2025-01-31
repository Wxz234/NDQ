#pragma once

namespace ndq
{
    class IReleasable
    {
    public:
        virtual void Release() = 0;
    };
}
