// ELEFTERIOS LAZARIDIS
#pragma once
#include "RenderableObject.h"

class Light : public RenderableObject
{
public:
	DirectX::XMFLOAT3 lightColor = { 1.0f, 1.0f, 1.0f };
	float lightStrength = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 0.00045f;
	float attenuation_c = 0.000075f;

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& fileName, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_Material>& cb_ps_material);
};