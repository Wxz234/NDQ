#pragma once

#include "ndq/core/resource.h"

#include <cstddef>

namespace ndq
{
    class IBlob : public IRefCounted
    {
    public:
        virtual void* GetBufferPointer() const = 0;
        virtual size_t GetBufferSize() const = 0;
    };
}
