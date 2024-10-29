// ELEFTERIOS LAZARIDIS
#include "IndexBuffer.h"

bool IndexBuffer::Initialize(ID3D11Device* device, DWORD* data, UINT indexCount)
{
	if (indexBuffer.Get() != nullptr)
		indexBuffer.Reset();

	this->indexCount = indexCount;

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.ByteWidth = sizeof(DWORD) * indexCount;
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = data;
	HRESULT hr = device->CreateBuffer(&ibd, &isd, indexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create index buffer", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	return true;
}

UINT IndexBuffer::IndexCount() const
{
	return indexCount;
}

ID3D11Buffer* IndexBuffer::Get() const
{
	return indexBuffer.Get();
}

ID3D11Buffer* const* IndexBuffer::GetAddressOf() const
{
	return indexBuffer.GetAddressOf();
}