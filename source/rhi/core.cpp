#include <d3dcompiler.h>

#include "ndq/rhi/core.h"

namespace ndq
{
    ID3DBlob* LoadShader(const wchar_t* path)
    {
        ID3DBlob* pBlob;
        D3DReadFileToBlob(path, &pBlob);
        return pBlob;
    }

    void DeleteShader(ID3DBlob* pBlob) {
        if (pBlob) {
            pBlob->Release();
        }
    }
}