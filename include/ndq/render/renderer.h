#pragma once

#include "ndq/core/releasable.h"
#include "ndq/render/light.h"
#include "ndq/render/renderable.h"

namespace ndq
{
    class IRenderer : public IReleasable
    {
    public:
        virtual void AddRenderable(IRenderable* pRenderable) = 0;
    };

    void CreateRenderer(IRenderer** ppRenderer);
}