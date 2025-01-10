#include "d3dx12.h"

#include "ndq/rhi/core.h"
#include "ndq/rhi/device.h"
#include "ndq/window.h"

using namespace ndq;

struct Window : IWindow
{
    Window()
    {
        Title = L"Triangle";
    }

    void Initialize()
    {
        auto pDevice = GetGraphicsDevice();

        const wchar_t* VertexArgs[] =
        {
            L"-T", L"vs_6_6",
            L"-E", L"mainVS"
        };
        const wchar_t* PixelArgs[] =
        {
            L"-T", L"ps_6_6",
            L"-E", L"mainPS"
        };

        auto VertexBlob = LoadShader(L"vertex.hlsl", VertexArgs, 4);
        auto PixelBlob = LoadShader(L"pixel.hlsl", PixelArgs, 4);

        auto pRawDevice = pDevice->GetRawDevice();

        D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
        PsoDesc.InputLayout = { 0, 0 };
        //PsoDesc.pRootSignature = m_rootSignature.Get();
        PsoDesc.VS = CD3DX12_SHADER_BYTECODE(VertexBlob->GetBufferPointer(), VertexBlob->GetBufferSize());
        PsoDesc.PS = CD3DX12_SHADER_BYTECODE(PixelBlob->GetBufferPointer(), PixelBlob->GetBufferSize());
        PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        PsoDesc.DepthStencilState.DepthEnable = FALSE;
        PsoDesc.DepthStencilState.StencilEnable = FALSE;
        PsoDesc.SampleMask = 0xffffffff;
        PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        PsoDesc.NumRenderTargets = 1;
        PsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        PsoDesc.SampleDesc.Count = 1;
        //pRawDevice->CreateGraphicsPipelineState()
    }

    void Update(float t) {}
    void Finalize() {}
};

WIN_MAIN_MACRO(Window)