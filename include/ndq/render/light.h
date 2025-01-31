#pragma once

#include <DirectXMath.h>

namespace ndq
{
    class Light
    {
    public:
        enum class LightType
        {
            Directional,
        };

        Light(LightType type, float intensity) : mType(type), mIntensity(intensity) {}

        virtual ~Light() {}

        LightType GetType() const { return mType; }
    protected:
        LightType mType;
        float mIntensity;
    };

    class DirectionalLight : public Light
    {
    public:
        DirectionalLight(const DirectX::XMFLOAT3& direction, float intensity) : Light(LightType::Directional, intensity), mDirection(direction) {}
    private:
        DirectX::XMFLOAT3 mDirection;
    };
}