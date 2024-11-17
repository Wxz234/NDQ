#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>

#include <memory>

#include "ndq/rhi/command_list.h"
#include "ndq/rhi/device.h"

#define NDQ_SWAPCHAIN_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define NDQ_SWAPCHAIN_COUNT 3
#define NDQ_NODE_MASK 1

namespace ndq
{
    std::shared_ptr<ICommandList> _CreateCommandList(NDQ_COMMAND_LIST_TYPE type, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> pList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pAllocator);
}

namespace ndq
{
    class Device : public IDevice
    {
    public:
        Device() {}

        ~Device()
        {

        }

        void Initialize(HWND hwnd, UINT width, UINT height)
        {
            Microsoft::WRL::ComPtr<IDXGIFactory7> Factory;
            UINT FactoryFlag = 0;
#ifdef _DEBUG
            Microsoft::WRL::ComPtr<ID3D12Debug> DebugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
            {
                DebugController->EnableDebugLayer();
            }
            FactoryFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif
            CreateDXGIFactory2(FactoryFlag, IID_PPV_ARGS(&Factory));
            Microsoft::WRL::ComPtr<IDXGIAdapter4> Adapter;
            Factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&Adapter));
            D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mpDevice));

            D3D12_COMMAND_QUEUE_DESC QueueDesc{};
            QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            mpDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&mpGraphicsQueue));
            QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
            mpDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&mpCopyQueue));
            QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            mpDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&mpComputeQueue));

            DXGI_SWAP_CHAIN_DESC1 ScDesc{};
            ScDesc.BufferCount = NDQ_SWAPCHAIN_COUNT;
            ScDesc.Width = width;
            ScDesc.Height = height;
            ScDesc.Format = NDQ_SWAPCHAIN_FORMAT;
            ScDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            ScDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            ScDesc.SampleDesc.Count = 1;
            ScDesc.SampleDesc.Quality = 0;
            ScDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            ScDesc.Scaling = DXGI_SCALING_STRETCH;

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC FsSwapChainDesc{};
            FsSwapChainDesc.Windowed = TRUE;
            Microsoft::WRL::ComPtr<IDXGISwapChain1> SwapChain;

            Factory->CreateSwapChainForHwnd(mpGraphicsQueue.Get(), hwnd, &ScDesc, &FsSwapChainDesc, nullptr, &SwapChain);
            Factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

            SwapChain.As(&mpSwapChain);

            mFrameIndex = mpSwapChain->GetCurrentBackBufferIndex();
            mpDevice->CreateFence(mFenceValue[mFrameIndex]++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpFence));
            mEvent.Attach(CreateEventW(nullptr, FALSE, FALSE, nullptr));

            mpDevice->CreateFence(mCopyFenceValue++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpCopyFence));
            mCopyEvent.Attach(CreateEventW(nullptr, FALSE, FALSE, nullptr));
            mpDevice->CreateFence(mComputeFenceValue++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpComputeFence));
            mComputeEvent.Attach(CreateEventW(nullptr, FALSE, FALSE, nullptr));
        }

        void Finalize()
        {
            Wait(NDQ_COMMAND_LIST_TYPE::GRAPHICS);
            Wait(NDQ_COMMAND_LIST_TYPE::COPY);
            Wait(NDQ_COMMAND_LIST_TYPE::COMPUTE);
        }

        void Present()
        {
            mpSwapChain->Present(1, 0);
            MoveToNextFrame();
        }

        Microsoft::WRL::ComPtr<ID3D12Device> GetRawDevice() const
        {
            return mpDevice;
        }

        void Wait(NDQ_COMMAND_LIST_TYPE type)
        {
            _Wait(type);
        }

        void ExecuteCommandList(ICommandList* pList)
        {
            auto Type = pList->GetType();
            auto TempList = pList->GetRawCommandList();
            ID3D12CommandList* Lists[1] = { reinterpret_cast<ID3D12CommandList*> (TempList.Get()) };
            switch (Type)
            {
            case NDQ_COMMAND_LIST_TYPE::GRAPHICS:
                mpGraphicsQueue->ExecuteCommandLists(1, Lists);
                break;
            case NDQ_COMMAND_LIST_TYPE::COPY:
                mpCopyQueue->ExecuteCommandLists(1, Lists);
                break;
            case NDQ_COMMAND_LIST_TYPE::COMPUTE:
                mpComputeQueue->ExecuteCommandLists(1, Lists);
                break;
            }
        }

        std::shared_ptr<ICommandList> CreateCommandList(NDQ_COMMAND_LIST_TYPE type)
        {
            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> Allocator;
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> List;
            switch (type)
            {
            case NDQ_COMMAND_LIST_TYPE::GRAPHICS:
                mpDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(Allocator.ReleaseAndGetAddressOf()));
                mpDevice->CreateCommandList1(NDQ_NODE_MASK, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(List.ReleaseAndGetAddressOf()));
                break;
            case NDQ_COMMAND_LIST_TYPE::COPY:
                mpDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(Allocator.ReleaseAndGetAddressOf()));
                mpDevice->CreateCommandList1(NDQ_NODE_MASK, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(List.ReleaseAndGetAddressOf()));
                break;
            case NDQ_COMMAND_LIST_TYPE::COMPUTE:
                mpDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(Allocator.ReleaseAndGetAddressOf()));
                mpDevice->CreateCommandList1(NDQ_NODE_MASK, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(List.ReleaseAndGetAddressOf()));
                break;
            }
            return _CreateCommandList(type, List, Allocator);
        }

        void MoveToNextFrame()
        {
            const UINT64 CurrentFenceValue = mFenceValue[mFrameIndex];
            mpGraphicsQueue->Signal(mpFence.Get(), CurrentFenceValue);
            mFrameIndex = mpSwapChain->GetCurrentBackBufferIndex();
            if (mpFence->GetCompletedValue() < mFenceValue[mFrameIndex])
            {
                mpFence->SetEventOnCompletion(mFenceValue[mFrameIndex], mEvent.Get());
                WaitForSingleObjectEx(mEvent.Get(), INFINITE, FALSE);
            }
            mFenceValue[mFrameIndex] = CurrentFenceValue + 1;
        }

        void _Wait(NDQ_COMMAND_LIST_TYPE type)
        {
            switch (type)
            {
            case NDQ_COMMAND_LIST_TYPE::GRAPHICS:
                mpGraphicsQueue->Signal(mpFence.Get(), mFenceValue[mFrameIndex]);
                mpFence->SetEventOnCompletion(mFenceValue[mFrameIndex]++, mEvent.Get());
                WaitForSingleObjectEx(mEvent.Get(), INFINITE, FALSE);
                break;
            case NDQ_COMMAND_LIST_TYPE::COPY:
                mpCopyQueue->Signal(mpCopyFence.Get(), mCopyFenceValue);
                mpCopyFence->SetEventOnCompletion(mCopyFenceValue++, mCopyEvent.Get());
                WaitForSingleObjectEx(mCopyEvent.Get(), INFINITE, FALSE);
                break;
            case NDQ_COMMAND_LIST_TYPE::COMPUTE:
                mpComputeQueue->Signal(mpComputeFence.Get(), mComputeFenceValue);
                mpComputeFence->SetEventOnCompletion(mComputeFenceValue++, mComputeEvent.Get());
                WaitForSingleObjectEx(mComputeEvent.Get(), INFINITE, FALSE);
                break;
            }
        }

        Microsoft::WRL::ComPtr<ID3D12Device4> mpDevice;
        Microsoft::WRL::ComPtr<IDXGISwapChain4> mpSwapChain;

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> mpGraphicsQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> mpCopyQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> mpComputeQueue;

        UINT32 mFrameIndex = 0;

        UINT64 mFenceValue[NDQ_SWAPCHAIN_COUNT]{};
        Microsoft::WRL::ComPtr<ID3D12Fence1> mpFence;
        Microsoft::WRL::Wrappers::Event mEvent;

        UINT64 mCopyFenceValue = 0;
        Microsoft::WRL::ComPtr<ID3D12Fence1> mpCopyFence;
        Microsoft::WRL::Wrappers::Event mCopyEvent;
        UINT64 mComputeFenceValue = 0;
        Microsoft::WRL::ComPtr<ID3D12Fence1> mpComputeFence;
        Microsoft::WRL::Wrappers::Event mComputeEvent;
    };

    IDevice* GetGraphicsDevice()
    {
        static IDevice* pDevice = new Device;
        return pDevice;
    }

    void _SetDeviceHwndAndSize(void* hwnd, unsigned width, unsigned height)
    {
        auto TempPtr = dynamic_cast<Device*>(GetGraphicsDevice());
        HWND* pHwnd = reinterpret_cast<HWND*>(hwnd);
        TempPtr->Initialize(*pHwnd, width, height);
    }

    void _DevicePresent()
    {
        auto TempPtr = dynamic_cast<Device*>(GetGraphicsDevice());
        TempPtr->Present();
    }

    void _DeviceFinalize()
    {
        auto TempPtr = dynamic_cast<Device*>(GetGraphicsDevice());
        TempPtr->Finalize();
        delete TempPtr;
    }
}