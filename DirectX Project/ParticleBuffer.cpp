// ELEFTERIOS LAZARIDIS
#include "ParticleBuffer.h"

ParticleBuffer::ParticleBuffer()
{
    for (int i = 0; i < 32; i++)
    {
        particles[i].startPosition = DirectX::XMFLOAT3(0,0,0);
        particles[i].position = DirectX::XMFLOAT3(0, 0, 0);
        particles[i].velocity = 0.0f;
        particles[i].lifeTime = 0.0f;
        particles[i].randomSeed = 0.0f;
    }
}

bool ParticleBuffer::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::wstring texturePath, DirectX::XMFLOAT3 position)
{
    for (int i = 0; i < 32; i++)
    {
        particles[i].startPosition = position;
        particles[i].position = position;
        particles[i].velocity = 1.0f + (rand() % 10) / 10.0f;
        particles[i].lifeTime = 1.0f + (rand() % 5) / 5.0f;
        particles[i].randomSeed = 1.0f + (rand() % 5) / 5.0f;
    }

    Texture texture(device, deviceContext, texturePath, TextureType::DIFFUSE);
    textureSRV = texture.GetTextureResourceView();

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = particleSize * nrOfParticles;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = particleSize;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = particles;
    HRESULT hr = device->CreateBuffer(&desc, &initData, buffer.GetAddressOf());
    if (FAILED(hr)) {
        MessageBox(nullptr, L"Failed to create the particle buffer.", L"Error message", MB_OK | MB_ICONINFORMATION);
        return false;
    }

    // Create SRV and UAV for the buffer
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = nrOfParticles;
    hr = device->CreateShaderResourceView(buffer.Get(), &srvDesc, srv.GetAddressOf());
    if (FAILED(hr)) {
        MessageBox(nullptr, L"Failed to create SRV for the particle buffer.", L"Error message", MB_OK | MB_ICONINFORMATION);
        return false;
    }

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = nrOfParticles;
    uavDesc.Buffer.Flags = 0;
    hr = device->CreateUnorderedAccessView(buffer.Get(), &uavDesc, uav.GetAddressOf());
    if (FAILED(hr)) {
        MessageBox(nullptr, L"Failed to create UAV for the particle buffer.", L"Error message", MB_OK | MB_ICONINFORMATION);
        return false;
    }

    return true;
}

ID3D11Buffer* ParticleBuffer::Get() const
{
    return buffer.Get();
}

ID3D11Buffer* const* ParticleBuffer::GetAddressOf() const
{
    return buffer.GetAddressOf();
}

ID3D11ShaderResourceView* ParticleBuffer::GetSRV() const
{
    return srv.Get();
}

ID3D11ShaderResourceView* const* ParticleBuffer::GetSRVAddress() const
{
    return srv.GetAddressOf();
}

ID3D11UnorderedAccessView* ParticleBuffer::GetUAV() const
{
    return uav.Get();
}

ID3D11UnorderedAccessView* const* ParticleBuffer::GetUAVAddress() const
{
    return uav.GetAddressOf();
}

ID3D11ShaderResourceView* ParticleBuffer::GetTextureSRV() const
{
    return textureSRV.Get();
}

ID3D11ShaderResourceView* const* ParticleBuffer::GetTextureSRVAddress() const
{
    return textureSRV.GetAddressOf();
}

const UINT ParticleBuffer::ParticleSize() const
{
    return particleSize;
}

UINT ParticleBuffer::NrOfParticles() const
{
    return nrOfParticles;
}
