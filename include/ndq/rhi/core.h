#pragma once

#include <d3d12.h>
#include <d3dcommon.h>

namespace ndq
{
    ID3DBlob* LoadShader(const wchar_t* path);
    void DeleteShader(ID3DBlob* pBlob);

    struct CD3DX12_DEFAULT {};
    extern const DECLSPEC_SELECTANY CD3DX12_DEFAULT D3D12_DEFAULT;

    struct CD3DX12_RASTERIZER_DESC : public D3D12_RASTERIZER_DESC
    {
        CD3DX12_RASTERIZER_DESC() = default;
        explicit CD3DX12_RASTERIZER_DESC(const D3D12_RASTERIZER_DESC& o) noexcept :
            D3D12_RASTERIZER_DESC(o)
        {
        }
        explicit CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT) noexcept
        {
            FillMode = D3D12_FILL_MODE_SOLID;
            CullMode = D3D12_CULL_MODE_BACK;
            FrontCounterClockwise = FALSE;
            DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
            DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
            SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
            DepthClipEnable = TRUE;
            MultisampleEnable = FALSE;
            AntialiasedLineEnable = FALSE;
            ForcedSampleCount = 0;
            ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        }
        explicit CD3DX12_RASTERIZER_DESC(
            D3D12_FILL_MODE fillMode,
            D3D12_CULL_MODE cullMode,
            BOOL frontCounterClockwise,
            INT depthBias,
            FLOAT depthBiasClamp,
            FLOAT slopeScaledDepthBias,
            BOOL depthClipEnable,
            BOOL multisampleEnable,
            BOOL antialiasedLineEnable,
            UINT forcedSampleCount,
            D3D12_CONSERVATIVE_RASTERIZATION_MODE conservativeRaster) noexcept
        {
            FillMode = fillMode;
            CullMode = cullMode;
            FrontCounterClockwise = frontCounterClockwise;
            DepthBias = depthBias;
            DepthBiasClamp = depthBiasClamp;
            SlopeScaledDepthBias = slopeScaledDepthBias;
            DepthClipEnable = depthClipEnable;
            MultisampleEnable = multisampleEnable;
            AntialiasedLineEnable = antialiasedLineEnable;
            ForcedSampleCount = forcedSampleCount;
            ConservativeRaster = conservativeRaster;
        }
    };

    struct CD3DX12_BLEND_DESC : public D3D12_BLEND_DESC
    {
        CD3DX12_BLEND_DESC() = default;
        explicit CD3DX12_BLEND_DESC(const D3D12_BLEND_DESC& o) noexcept :
            D3D12_BLEND_DESC(o)
        {
        }
        explicit CD3DX12_BLEND_DESC(CD3DX12_DEFAULT) noexcept
        {
            AlphaToCoverageEnable = FALSE;
            IndependentBlendEnable = FALSE;
            const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
            {
                FALSE,FALSE,
                D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
                D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
                D3D12_LOGIC_OP_NOOP,
                D3D12_COLOR_WRITE_ENABLE_ALL,
            };
            for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
                RenderTarget[i] = defaultRenderTargetBlendDesc;
        }
    };
}