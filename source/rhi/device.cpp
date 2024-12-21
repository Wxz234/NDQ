#include <d3d12.h>
#include <dxgi1_6.h>

#include <memory>

#include "ndq/rhi/command_list.h"
#include "ndq/rhi/device.h"

#define NDQ_SWAPCHAIN_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define NDQ_SWAPCHAIN_COUNT 3
#define NDQ_NODE_MASK 1

namespace ndq
{
    std::shared_ptr<ICommandList> _CreateCommandList(
        NDQ_COMMAND_LIST_TYPE type,
        ID3D12GraphicsCommandList4* pList,
        ID3D12CommandAllocator* pAllocator
    );
}

namespace ndq
{
    class Device : public IDevice
    {
    public:
        Device() {}

        ~Device()
        {
            Finalize();

            mpDevice->Release();
            mpSwapChain->Release();
            mpGraphicsQueue->Release();
            mpCopyQueue->Release();
            mpComputeQueue->Release();
            mpFence->Release();
            mpCopyFence->Release();
            mpComputeFence->Release();

            CloseHandle(mEvent);
            CloseHandle(mCopyEvent);
            CloseHandle(mComputeEvent);
        }

        void Initialize(HWND hwnd, UINT width, UINT height)
        {
            IDXGIFactory7* Factory = nullptr;
            UINT FactoryFlag = 0;
            ID3D12Debug* DebugController = nullptr;
#ifdef _DEBUG
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
            {
                DebugController->EnableDebugLayer();
            }
            FactoryFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif
            CreateDXGIFactory2(FactoryFlag, IID_PPV_ARGS(&Factory));
            IDXGIAdapter4* Adapter;
            Factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&Adapter));
            D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mpDevice));

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
            IDXGISwapChain1* SwapChain;

            Factory->CreateSwapChainForHwnd(mpGraphicsQueue, hwnd, &ScDesc, &FsSwapChainDesc, nullptr, &SwapChain);
            Factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

            SwapChain->QueryInterface(&mpSwapChain);

            mFrameIndex = mpSwapChain->GetCurrentBackBufferIndex();
            mpDevice->CreateFence(mFenceValue[mFrameIndex]++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpFence));
            mEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

            mpDevice->CreateFence(mCopyFenceValue++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpCopyFence));
            mCopyEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            mpDevice->CreateFence(mComputeFenceValue++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mpComputeFence));
            mComputeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

            Factory->Release();
            if (DebugController)
            {
                DebugController->Release();
            }
            Adapter->Release();
            SwapChain->Release();
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

        ID3D12Device* GetRawDevice() const
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
            ID3D12CommandList* Lists[1] = { reinterpret_cast<ID3D12CommandList*> (TempList) };
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
            ID3D12CommandAllocator* Allocator = nullptr;
            ID3D12GraphicsCommandList4* List = nullptr;
            switch (type)
            {
            case NDQ_COMMAND_LIST_TYPE::GRAPHICS:
                mpDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Allocator));
                mpDevice->CreateCommandList1(NDQ_NODE_MASK, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&List));
                break;
            case NDQ_COMMAND_LIST_TYPE::COPY:
                mpDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&Allocator));
                mpDevice->CreateCommandList1(NDQ_NODE_MASK, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&List));
                break;
            case NDQ_COMMAND_LIST_TYPE::COMPUTE:
                mpDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&Allocator));
                mpDevice->CreateCommandList1(NDQ_NODE_MASK, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&List));
                break;
            }
            return _CreateCommandList(type, List, Allocator);
        }

        void MoveToNextFrame()
        {
            const UINT64 CurrentFenceValue = mFenceValue[mFrameIndex];
            mpGraphicsQueue->Signal(mpFence, CurrentFenceValue);
            mFrameIndex = mpSwapChain->GetCurrentBackBufferIndex();
            if (mpFence->GetCompletedValue() < mFenceValue[mFrameIndex])
            {
                mpFence->SetEventOnCompletion(mFenceValue[mFrameIndex], mEvent);
                WaitForSingleObjectEx(mEvent, INFINITE, FALSE);
            }
            mFenceValue[mFrameIndex] = CurrentFenceValue + 1;
        }

        void _Wait(NDQ_COMMAND_LIST_TYPE type)
        {
            switch (type)
            {
            case NDQ_COMMAND_LIST_TYPE::GRAPHICS:
                mpGraphicsQueue->Signal(mpFence, mFenceValue[mFrameIndex]);
                mpFence->SetEventOnCompletion(mFenceValue[mFrameIndex]++, mEvent);
                WaitForSingleObjectEx(mEvent, INFINITE, FALSE);
                break;
            case NDQ_COMMAND_LIST_TYPE::COPY:
                mpCopyQueue->Signal(mpCopyFence, mCopyFenceValue);
                mpCopyFence->SetEventOnCompletion(mCopyFenceValue++, mCopyEvent);
                WaitForSingleObjectEx(mCopyEvent, INFINITE, FALSE);
                break;
            case NDQ_COMMAND_LIST_TYPE::COMPUTE:
                mpComputeQueue->Signal(mpComputeFence, mComputeFenceValue);
                mpComputeFence->SetEventOnCompletion(mComputeFenceValue++, mComputeEvent);
                WaitForSingleObjectEx(mComputeEvent, INFINITE, FALSE);
                break;
            }
        }

        ID3D12Device4* mpDevice = nullptr;
        IDXGISwapChain4* mpSwapChain = nullptr;

        ID3D12CommandQueue* mpGraphicsQueue = nullptr;
        ID3D12CommandQueue* mpCopyQueue = nullptr;
        ID3D12CommandQueue* mpComputeQueue = nullptr;

        UINT32 mFrameIndex = 0;

        UINT64 mFenceValue[NDQ_SWAPCHAIN_COUNT]{};
        ID3D12Fence1* mpFence = nullptr;
        HANDLE mEvent = INVALID_HANDLE_VALUE;

        UINT64 mCopyFenceValue = 0;
        ID3D12Fence1* mpCopyFence = nullptr;
        HANDLE mCopyEvent = INVALID_HANDLE_VALUE;
        UINT64 mComputeFenceValue = 0;
        ID3D12Fence1* mpComputeFence = nullptr;
        HANDLE mComputeEvent = INVALID_HANDLE_VALUE;
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