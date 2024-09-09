#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/wrappers/corewrappers.h>

#include "ndq/rhi/device.h"

#define NDQ_SWAPCHAIN_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define NDQ_SWAPCHAIN_COUNT 3

namespace ndq
{
    class Device : public IDevice
    {
    public:
        Device() {}

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
            D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&pDevice));

            D3D12_COMMAND_QUEUE_DESC QueueDesc{};
            QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            pDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&pGraphicsQueue));
            QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
            pDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&pCopyQueue));
            QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            pDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&pComputeQueue));

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

            Factory->CreateSwapChainForHwnd(pGraphicsQueue.Get(), hwnd, &ScDesc, &FsSwapChainDesc, nullptr, &SwapChain);
            Factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);

            SwapChain.As(&pSwapChain);

            mFrameIndex = pSwapChain->GetCurrentBackBufferIndex();
            pDevice->CreateFence(mFenceValue[mFrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
            ++mFenceValue[mFrameIndex];
            mEvent.Attach(CreateEventW(nullptr, FALSE, FALSE, nullptr));
        }

        void WaitForQueue(ID3D12CommandQueue* pCommandQueue)
        {
            Microsoft::WRL::ComPtr<ID3D12Fence> pFence;
            HANDLE EventHandle = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            UINT64 FenceValue = 0;
            pDevice->CreateFence(FenceValue++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence));
            pCommandQueue->Signal(pFence.Get(), FenceValue);
            pFence->SetEventOnCompletion(FenceValue, EventHandle);
            WaitForSingleObject(EventHandle, INFINITE);
            CloseHandle(EventHandle);
        }

        void Finalize()
        {
            WaitForQueue(pGraphicsQueue.Get());
            WaitForQueue(pCopyQueue.Get());
            WaitForQueue(pComputeQueue.Get());
        }

        void Present()
        {
            pSwapChain->Present(1, 0);
            MoveToNextFrame();
        }

        Microsoft::WRL::ComPtr<ID3D12Device> GetRawDevice() const
        {
            return pDevice;
        }

        void MoveToNextFrame()
        {
            const UINT64 CurrentFenceValue = mFenceValue[mFrameIndex];
            pGraphicsQueue->Signal(pFence.Get(), CurrentFenceValue);
            mFrameIndex = pSwapChain->GetCurrentBackBufferIndex();
            if (pFence->GetCompletedValue() < mFenceValue[mFrameIndex])
            {
                pFence->SetEventOnCompletion(mFenceValue[mFrameIndex], mEvent.Get());
                WaitForSingleObjectEx(mEvent.Get(), INFINITE, FALSE);
            }
            mFenceValue[mFrameIndex] = CurrentFenceValue + 1;
        }

        Microsoft::WRL::ComPtr<ID3D12Device4> pDevice;
        Microsoft::WRL::ComPtr<IDXGISwapChain4> pSwapChain;

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> pGraphicsQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCopyQueue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> pComputeQueue;

        UINT32 mFrameIndex = 0;

        UINT64 mFenceValue[NDQ_SWAPCHAIN_COUNT]{};
        Microsoft::WRL::ComPtr<ID3D12Fence1> pFence;
        Microsoft::WRL::Wrappers::Event mEvent;
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