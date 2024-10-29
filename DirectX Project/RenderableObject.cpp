// ELEFTERIOS LAZARIDIS
#include "RenderableObject.h"

bool RenderableObject::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_Material>& cb_ps_material)
{
	if (!model.Initialize(filePath, device, deviceContext, cb_vs_vertexshader, cb_ps_material))
		return false;

	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	SetScale(1.0f, 1.0f, 1.0f);
	UpdateMatrix();

	return true;
}

void RenderableObject::Draw(const DirectX::XMMATRIX& viewProjectionMatrix)
{
	model.Draw(worldMatrix, viewProjectionMatrix);
}

void RenderableObject::SetSpecular(float specular)
{
	model.SetSpecular(specular);
}

void RenderableObject::SetReflective(int reflect)
{
	model.SetReflective(reflect);
}

DirectX::BoundingBox RenderableObject::GetBoundingBox() const
{
	DirectX::BoundingBox transformedBox;
	model.GetBoundingBox().Transform(transformedBox, worldMatrix);
	return transformedBox;
}

void RenderableObject::UpdateMatrix()
{
	worldMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	UpdateDirectionVectors();
}
