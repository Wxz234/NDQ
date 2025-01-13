#include <Windows.h>

#include <combaseapi.h>
#include <dxgiformat.h>
#include <wrl/client.h>

#include "d3dx12.h"

#include "ndq/rhi/command_list.h"
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
        const wchar_t* VertexArgs[] =
        {
            L"-T", L"vs_6_6",
            L"-E", L"main"
        };
        const wchar_t* PixelArgs[] =
        {
            L"-T", L"ps_6_6",
            L"-E", L"main"
        };

        auto VertexBlob = LoadShader(L"vertex.hlsl", VertexArgs, 4);
        auto PixelBlob = LoadShader(L"pixel.hlsl", PixelArgs, 4);
       
        auto pRawDevice = GetGraphicsDevice()->GetRawDevice();
        Microsoft::WRL::ComPtr<ID3D12RootSignature> pRootSignature;
        pRawDevice->CreateRootSignature(1, VertexBlob->GetBufferPointer(), VertexBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature));

        D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc{};
        PsoDesc.pRootSignature = pRootSignature.Get();
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
        
        pRawDevice->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&pPipelineSatae));

        GetGraphicsDevice()->CreateCommandList(NDQ_COMMAND_LIST_TYPE::GRAPHICS, &pCmdList);
    }

    void Update(float)
    {
        D3D12_VERTEX_BUFFER_VIEW emptyVertexBuffer{};

        auto pRawCmdList = pCmdList->GetRawCommandList();
        //pCmdList->Open(pPipelineSatae.Get());
        //pCmdList->Close();

        ////commandList->IASetVertexBuffers(0, 1, &emptyVertexBuffer);
        ////commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        ////commandList->DrawInstanced(3, 1, 0, 0);
        //GetGraphicsDevice()->ExecuteCommandList(pCmdList);

        GetGraphicsDevice()->Wait(NDQ_COMMAND_LIST_TYPE::GRAPHICS);
    }

    void Finalize()
    {
        pCmdList->Release();
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pPipelineSatae;
    ICommandList* pCmdList = nullptr;

};

WIN_MAIN_MACRO(Window)