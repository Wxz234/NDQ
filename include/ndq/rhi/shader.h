#pragma once

#include <d3dcommon.h>

namespace ndq
{
    ID3DBlob* LoadShader(const wchar_t* path);
    void DeleteShader(ID3DBlob* pBlob);
}