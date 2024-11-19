#pragma once

#include <d3d12.h>

#include <memory>

#include "ndq/rhi/command_list.h"

namespace ndq
{
    class IDevice
    {
    public:
        virtual ID3D12Device* GetRawDevice() const = 0;
        virtual std::shared_ptr<ICommandList> CreateCommandList(NDQ_COMMAND_LIST_TYPE type) = 0;
        virtual void ExecuteCommandList(ICommandList* pList) = 0;
        virtual void Wait(NDQ_COMMAND_LIST_TYPE type) = 0;
    };

    IDevice* GetGraphicsDevice();
}
