#pragma once

#include "ndq/core/blob.h"

namespace ndq
{
    void LoadShaderFromPath(const wchar_t* path, const wchar_t** pArguments, unsigned argCount, IBlob** ppBlob);
}