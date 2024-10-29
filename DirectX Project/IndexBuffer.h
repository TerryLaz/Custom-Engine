// ELEFTERIOS LAZARIDIS
#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <iostream>

class IndexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	UINT indexCount = 0;

public:
	bool Initialize(ID3D11Device* device, DWORD * data, UINT indexCount);

	UINT IndexCount()const;
	ID3D11Buffer* Get() const;
	ID3D11Buffer* const* GetAddressOf() const;
};