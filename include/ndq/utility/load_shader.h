#pragma once

#include <d3dcommon.h>
#include <wrl/client.h>

namespace ndq
{
    Microsoft::WRL::ComPtr<ID3DBlob> LoadShader(const wchar_t* path);
}