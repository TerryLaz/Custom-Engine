// ELEFTERIOS LAZARIDIS
#include "Shaders.h"

//Vertex Shader
bool VertexShader::Initialize(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT nrElements, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), vertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to read vertex shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	hr = device->CreateVertexShader(vertexBuffer->GetBufferPointer(), vertexBuffer->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create vertex shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	if (layoutDesc != nullptr)
	{
		hr = device->CreateInputLayout(layoutDesc, nrElements, vertexBuffer->GetBufferPointer(), vertexBuffer->GetBufferSize(), inputLayout.GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create input layout", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

    return true;
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return vertexShader.Get();
}

ID3D10Blob* VertexShader::GetBuffer()
{
	return vertexBuffer.Get();
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return inputLayout.Get();
}

//Pixel Shader
bool PixelShader::Initialize(ID3D11Device* device, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), pixelBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to read pixel shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	hr = device->CreatePixelShader(pixelBuffer.Get()->GetBufferPointer(), pixelBuffer.Get()->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create pixel shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	return true;
}

ID3D11PixelShader* PixelShader::GetShader()
{
	return pixelShader.Get();
}

ID3DBlob* PixelShader::GetBuffer()
{
	return pixelBuffer.Get();
}

//Hull Shader
bool HullShader::Initialize(ID3D11Device* device, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), hullBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to read hull shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	hr = device->CreateHullShader(hullBuffer.Get()->GetBufferPointer(), hullBuffer.Get()->GetBufferSize(), nullptr, hullShader.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create hull shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	return true;
}

ID3D11HullShader* HullShader::GetShader()
{
	return hullShader.Get();
}

ID3D10Blob* HullShader::GetBuffer()
{
	return hullBuffer.Get();
}

//Domain Shader
bool DomainShader::Initialize(ID3D11Device* device, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), domainBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to read hull shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	hr = device->CreateDomainShader(domainBuffer.Get()->GetBufferPointer(), domainBuffer.Get()->GetBufferSize(), nullptr, domainShader.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create hull shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	return true;
}

ID3D11DomainShader* DomainShader::GetShader()
{
	return domainShader.Get();
}

ID3D10Blob* DomainShader::GetBuffer()
{
	return domainBuffer.Get();
}

//Compute Shader
bool ComputeShader::Initialize(ID3D11Device* device, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), computeBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to read compute shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	hr = device->CreateComputeShader(computeBuffer.Get()->GetBufferPointer(), computeBuffer.Get()->GetBufferSize(), nullptr, computeShader.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create compute shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	return true;
}

ID3D11ComputeShader* ComputeShader::GetShader()
{
	return computeShader.Get();
}

ID3D10Blob* ComputeShader::GetBuffer()
{
	return computeBuffer.Get();
}

//Geometry Shader
bool GeometryShader::Initialize(ID3D11Device* device, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), geometryBuffer.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to read compute shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	hr = device->CreateGeometryShader(geometryBuffer.Get()->GetBufferPointer(), geometryBuffer.Get()->GetBufferSize(), nullptr, geometryShader.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create compute shader", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	return true;
}

ID3D11GeometryShader* GeometryShader::GetShader()
{
	return geometryShader.Get();
}

ID3D10Blob* GeometryShader::GetBuffer()
{
	return geometryBuffer.Get();
}
