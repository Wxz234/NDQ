#pragma once

#include "ndq/core/releasable.h"

namespace ndq
{
    class IRenderable : public IReleasable
    {
    public:
    };

    void CreateRenderableFromPath(const wchar_t* path, IRenderable** ppRenderable);
}