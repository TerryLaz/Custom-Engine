// ELEFTERIOS LAZARIDIS
#include "Camera.h"

Camera::Camera()
{
	position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	posVector = XMLoadFloat3(&position);
	rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotVector = XMLoadFloat3(&rotation);
	UpdateMatrix();
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * DirectX::XM_2PI;
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

void Camera::SetOrthographicValues(int width, int height, float nearZ, float farZ)
{
	projectionMatrix = DirectX::XMMatrixOrthographicLH(static_cast<float>(width), static_cast<float>(height), nearZ, farZ);
}

const DirectX::XMMATRIX& Camera::GetViewMatrix() const
{
	return viewMatrix;
}

const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}

void Camera::UpdateMatrix()
{
	// Calculate camera rotation matrix
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	// Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
	DirectX::XMVECTOR camTarget = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//Adjust cam target to be offset by the camera's current position
	camTarget = DirectX::XMVectorAdd(camTarget, posVector);
	// Calculate up direction based on current rotation
	DirectX::XMVECTOR upDir = DirectX::XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);
	// Rebuild view matrix
	viewMatrix = DirectX::XMMatrixLookAtLH(posVector, camTarget, upDir);

	UpdateDirectionVectors();
}