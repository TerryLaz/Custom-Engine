// ELEFTERIOS LAZARIDIS
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <WICTextureLoader.h>
#include <string>
#include "Color.h"

enum class TextureType
{
	NONE,
	AMBIENT,
	DIFFUSE,
	SPECULAR,
	MASK
};

class Texture
{
private:
	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;

	TextureType textureType = TextureType::NONE;
public:
	Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& filePath, TextureType type);
	Texture(ID3D11Device* device, const Color& color, TextureType type);

	TextureType GetType() const;
	ID3D11ShaderResourceView* GetTextureResourceView();
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();
};
