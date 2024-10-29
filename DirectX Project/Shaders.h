// ELEFTERIOS LAZARIDIS
#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <wrl.h>

class VertexShader
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
public:
	bool Initialize(ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT nrElements, std::wstring shaderPath);
	ID3D11VertexShader* GetShader();
	ID3DBlob* GetBuffer();
	ID3D11InputLayout* GetInputLayout();
};

class PixelShader
{
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> pixelBuffer;
public:
	bool Initialize(ID3D11Device* device, std::wstring shaderPath);
	ID3D11PixelShader* GetShader();
	ID3D10Blob* GetBuffer();
};

class HullShader
{
private:
	Microsoft::WRL::ComPtr<ID3D11HullShader> hullShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> hullBuffer;
public:
	bool Initialize(ID3D11Device* device, std::wstring shaderPath);
	ID3D11HullShader* GetShader();
	ID3D10Blob* GetBuffer();
};

class DomainShader
{
private:
	Microsoft::WRL::ComPtr<ID3D11DomainShader> domainShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> domainBuffer;
public:
	bool Initialize(ID3D11Device* device, std::wstring shaderPath);
	ID3D11DomainShader* GetShader();
	ID3D10Blob* GetBuffer();
};

class ComputeShader
{
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> computeBuffer;
public:
	bool Initialize(ID3D11Device* device, std::wstring shaderPath);
	ID3D11ComputeShader* GetShader();
	ID3D10Blob* GetBuffer();
};

class GeometryShader
{
private:
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> geometryBuffer;
public:
	bool Initialize(ID3D11Device* device, std::wstring shaderPath);
	ID3D11GeometryShader* GetShader();
	ID3D10Blob* GetBuffer();
};