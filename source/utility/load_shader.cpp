#include <d3dcompiler.h>

#include "ndq/utility/load_shader.h"

namespace ndq
{
    Microsoft::WRL::ComPtr<ID3DBlob> LoadShader(const wchar_t* path)
    {
        Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
        D3DReadFileToBlob(path, &pBlob);
        return pBlob;
    }
}