// ELEFTERIOS LAZARIDIS
#pragma once
#include "GameObject.h"
#include <DirectXCollision.h>

class RenderableObject : public GameObject
{
protected:
	Model model;
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

	void UpdateMatrix() override;
public:
	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_Material>& cb_ps_material);
	void Draw(const DirectX::XMMATRIX& viewProjectionMatrix);

	void SetSpecular(float specular);
	void SetReflective(int reflect);
	DirectX::BoundingBox GetBoundingBox() const;
};