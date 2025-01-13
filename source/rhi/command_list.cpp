#include <d3d12.h>

#include "ndq/rhi/command_list.h"

namespace ndq
{
    class CommandList : public ICommandList
    {
    public:
        CommandList(NDQ_COMMAND_LIST_TYPE type, 
            ID3D12GraphicsCommandList4* pList,
            ID3D12CommandAllocator* pAllocator) :
            mType(type), mpList(pList), mpAllocator(pAllocator) {}

        ~CommandList()
        {
            mpAllocator->Release();
            mpList->Release();
        }

        ID3D12GraphicsCommandList* GetRawCommandList() const
        {
            return mpList;
        }

        void Open(ID3D12PipelineState* pState = nullptr)
        {
            mpAllocator->Reset();
            mpList->Reset(mpAllocator, pState);
        }

        void Close()
        {
            mpList->Close();
        }

        NDQ_COMMAND_LIST_TYPE GetType() const
        {
            return mType;
        }

        void Release()
        {
            delete this;
        }

        NDQ_COMMAND_LIST_TYPE mType;
        ID3D12GraphicsCommandList4* mpList;
        ID3D12CommandAllocator* mpAllocator;
    };

    ICommandList* _CreateCommandList(NDQ_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList4* pList, ID3D12CommandAllocator* pAllocator)
    {
        return new CommandList(type, pList, pAllocator);
    }
}