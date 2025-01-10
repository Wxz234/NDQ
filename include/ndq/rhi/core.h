#pragma once

#include <dxcapi.h>
#include <wrl/client.h>

namespace ndq
{
    Microsoft::WRL::ComPtr<IDxcBlob> LoadShader(const wchar_t* path, const wchar_t** pArguments, unsigned argCount);
}