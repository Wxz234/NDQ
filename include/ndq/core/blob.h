#pragma once

#include "ndq/core/releasable.h"

#include <cstddef>

namespace ndq
{
    class IBlob : public IReleasable
    {
    public:
        virtual void* GetBufferPointer() const = 0;
        virtual size_t GetBufferSize() const = 0;
    };
}
