// ELEFTERIOS LAZARIDIS
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const VertexBuffer& rhs)
{
	buffer = rhs.buffer;
	vertexCount = rhs.vertexCount;
	stride = rhs.stride;
}

VertexBuffer& VertexBuffer::operator=(const VertexBuffer& a)
{
	buffer = a.buffer;
	vertexCount = a.vertexCount;
	stride = a.stride;
	return *this;
}

bool VertexBuffer::Initialize(ID3D11Device* device, Vertex* data, UINT vertexCount)
{
	if (buffer.Get() != nullptr)
		buffer.Reset();

	this->vertexCount = vertexCount;

	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.ByteWidth = stride * vertexCount;
	D3D11_SUBRESOURCE_DATA vbData = { 0 };
	vbData.pSysMem = data;
	HRESULT hr = device->CreateBuffer(&vbd, &vbData, buffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create vertex buffer", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	return true;
}

UINT VertexBuffer::VertexCount() const
{
	return vertexCount;
}

const UINT VertexBuffer::Stride() const
{
	return stride;
}

const UINT* VertexBuffer::StridePtr() const
{
	return &stride;
}

ID3D11Buffer* VertexBuffer::Get() const
{
	return buffer.Get();
}

ID3D11Buffer* const* VertexBuffer::GetAddressOf() const
{
	return buffer.GetAddressOf();
}