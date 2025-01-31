#include "ndq/core/blob.h"

#include <Windows.h>

#include <dxcapi.h>
#include <wrl/client.h>

namespace ndq
{
    class Blob : public IBlob
    {
    public:

        Blob(IDxcBlob* pRawBlob) : pBlob(pRawBlob) {}

        void Release()
        {
            delete this;
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
    };

    IBlob* _CreateBlob(IDxcBlob* pRawBlob)
    {
        return new Blob(pRawBlob);
    }
}