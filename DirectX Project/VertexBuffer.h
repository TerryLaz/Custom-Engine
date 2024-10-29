// ELEFTERIOS LAZARIDIS
#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <iostream>
#include "Vertex.h"

class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT stride = sizeof(Vertex);
	UINT vertexCount = 0;

public:
	VertexBuffer() = default;
	VertexBuffer(const VertexBuffer& rhs);
	VertexBuffer& operator=(const VertexBuffer& a);

	bool Initialize(ID3D11Device* device, Vertex* data, UINT vertexCount);

	UINT VertexCount() const;
	const UINT Stride() const;
	const UINT*  StridePtr() const;

	ID3D11Buffer* Get() const;
	ID3D11Buffer* const* GetAddressOf() const;
};
