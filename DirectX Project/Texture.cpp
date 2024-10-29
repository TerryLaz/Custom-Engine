// ELEFTERIOS LAZARIDIS
#include "Texture.h"

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& filePath, TextureType type)
	:textureType(type)
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(
		device,
		deviceContext,
		filePath.c_str(),
		texture.GetAddressOf(),
		textureView.GetAddressOf());

	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create WIC texture", L"Error message", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		deviceContext->GenerateMips(textureView.Get());
	}
}

Texture::Texture(ID3D11Device* device, const Color& color, TextureType type)
	:textureType(type)
{
	// Create a 1x1 texture with the specified color
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1);

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = &color;
	initData.SysMemPitch = sizeof(Color);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	HRESULT hr = device->CreateTexture2D(&textureDesc, &initData, texture2D.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create texture from color", L"Error message", MB_OK | MB_ICONINFORMATION);
	}

	texture = static_cast<ID3D11Texture2D*>(texture2D.Get());

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);

	hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create SRV from color", L"Error message", MB_OK | MB_ICONINFORMATION);
	}
}

TextureType Texture::GetType() const
{
	return textureType;
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
	return textureView.Get();
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
	return textureView.GetAddressOf();
}
