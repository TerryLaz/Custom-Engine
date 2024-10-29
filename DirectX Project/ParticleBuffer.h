// ELEFTERIOS LAZARIDIS
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <iostream>
#include "Texture.h"

struct Particle
{
    DirectX::XMFLOAT3 startPosition;
    float lifeTime;
    DirectX::XMFLOAT3 position;
    float velocity;
    float randomSeed;
};

class ParticleBuffer
{
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;

    UINT particleSize = sizeof(Particle);
    UINT nrOfParticles = 32;
    Particle particles[32];
public:    
    ParticleBuffer();

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::wstring texturePath, DirectX::XMFLOAT3 position);

    UINT NrOfParticles() const;
    const UINT ParticleSize() const;

    ID3D11Buffer* Get() const;
    ID3D11Buffer* const* GetAddressOf() const;

    ID3D11ShaderResourceView* GetSRV() const;
    ID3D11ShaderResourceView* const* GetSRVAddress() const;

    ID3D11UnorderedAccessView* GetUAV() const;
    ID3D11UnorderedAccessView* const* GetUAVAddress() const;

    ID3D11ShaderResourceView* GetTextureSRV() const;
    ID3D11ShaderResourceView* const* GetTextureSRVAddress() const;
};
