#include "ndq/core/blob.h"

#include <Windows.h>

#include <dxcapi.h>
#include <wrl/client.h>

#include <atomic>

namespace ndq
{
    class Blob : public IBlob
    {
    public:

        Blob(IDxcBlob* pRawBlob) : pBlob(pRawBlob) ,mRefCount(1) {}

        unsigned long AddRef()
        {
            return ++mRefCount;
        }

        void Release()
        {
            unsigned long result = --mRefCount;
            if (result == 0)
            {
                delete this;
            }
        }

        void* GetBufferPointer() const
        {
            return pBlob->GetBufferPointer();
        }

        size_t GetBufferSize() const
        {
            return pBlob->GetBufferSize();
        }

        Microsoft::WRL::ComPtr<IDxcBlob> pBlob;
        std::atomic<unsigned long> mRefCount;
    };

    IBlob* _CreateBlob(IDxcBlob* pRawBlob)
    {
        return new Blob(pRawBlob);
    }
}