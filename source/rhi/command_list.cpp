#include <d3d12.h>
#include <wrl/client.h>

#include <memory>

#include "ndq/rhi/command_list.h"

namespace ndq
{
    class CommandList : public ICommandList
    {
    public:
        CommandList(NDQ_COMMAND_LIST_TYPE type, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> pList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pAllocator) : 
            mType(type), mpList(pList), mpAllocator(pAllocator) {}

        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetRawCommandList() const
        {
            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> tempList;
            mpList.As(&tempList);
            return tempList;
        }

        void Open(ID3D12PipelineState* pState = nullptr)
        {
            mpAllocator->Reset();
            mpList->Reset(mpAllocator.Get(), pState);
        }

        void Close()
        {
            mpList->Close();
        }

        NDQ_COMMAND_LIST_TYPE GetType() const
        {
            return mType;
        }

        NDQ_COMMAND_LIST_TYPE mType;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> mpList;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mpAllocator;
    };

    std::shared_ptr<ICommandList> _CreateCommandList(NDQ_COMMAND_LIST_TYPE type, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> pList, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> pAllocator)
    {
        return std::shared_ptr<ICommandList>(new CommandList(type, pList, pAllocator));
    }
}