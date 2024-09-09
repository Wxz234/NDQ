#pragma once

#include <d3d12.h>
#include <wrl/client.h>

namespace ndq
{
    class IDevice
    {
    public:
        virtual Microsoft::WRL::ComPtr<ID3D12Device> GetRawDevice() const = 0;
    };

    IDevice* GetGraphicsDevice();
}
