#pragma once

#include <d3d12.h>

namespace ndq
{
    enum class NDQ_COMMAND_LIST_TYPE
    {
        GRAPHICS,
        COPY,
        COMPUTE,
    };

    class ICommandList
    {
    public:
        virtual ID3D12GraphicsCommandList* GetRawCommandList() const = 0;
        virtual NDQ_COMMAND_LIST_TYPE GetType() const = 0;
        virtual void Open(ID3D12PipelineState* pState = nullptr) = 0;
        virtual void Close() = 0;
        virtual void Release() = 0;
    };
}
