// ELEFTERIOS LAZARIDIS
#include "GameObject.h"

const DirectX::XMVECTOR& GameObject::GetPositionVector() const
{
	return posVector;
}

const DirectX::XMFLOAT3& GameObject::GetPositionFloat3() const
{
	return position;
}

const DirectX::XMVECTOR& GameObject::GetRotationVector() const
{
	return rotVector;
}

const DirectX::XMFLOAT3& GameObject::GetRotationFloat3() const
{
	return rotation;
}

void GameObject::SetPosition(const DirectX::XMVECTOR& pos)
{
	XMStoreFloat3(&position, pos);
	posVector = pos;
	UpdateMatrix();
}

void GameObject::SetPosition(const DirectX::XMFLOAT3& pos)
{
	position = pos;
	posVector = XMLoadFloat3(&position);
	UpdateMatrix();
}

void GameObject::SetPosition(float x, float y, float z)
{
	position = DirectX::XMFLOAT3(x, y, z);
	posVector = XMLoadFloat3(&position);
	UpdateMatrix();
}

void GameObject::AdjustPosition(const DirectX::XMVECTOR& pos)
{
	posVector = DirectX::XMVectorAdd(posVector, pos);
	XMStoreFloat3(&position, posVector);
	UpdateMatrix();
}

void GameObject::AdjustPosition(const DirectX::XMFLOAT3& pos)
{
	position.x += pos.y;
	position.y += pos.y;
	position.z += pos.z;
	posVector = XMLoadFloat3(&position);
	UpdateMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	posVector = XMLoadFloat3(&position);
	UpdateMatrix();
}

void GameObject::SetRotation(const DirectX::XMVECTOR& rot)
{
	rotVector = rot;
	XMStoreFloat3(&rotation, rot);
	UpdateMatrix();
}

void GameObject::SetRotation(const DirectX::XMFLOAT3& rot)
{
	rotation = rot;
	rotVector = XMLoadFloat3(&rotation);
	UpdateMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	rotation = DirectX::XMFLOAT3(x, y, z);
	rotVector = XMLoadFloat3(&rotation);
	UpdateMatrix();
}

void GameObject::AdjustRotation(const DirectX::XMVECTOR& rot)
{
	rotVector = DirectX::XMVectorAdd(rotVector, rot);
	XMStoreFloat3(&rotation, rotVector);
	UpdateMatrix();
}

void GameObject::AdjustRotation(const DirectX::XMFLOAT3& rot)
{
	rotation.x += rot.x;
	rotation.y += rot.y;
	rotation.z += rot.z;
	rotVector = XMLoadFloat3(&rotation);
	UpdateMatrix();
}

void GameObject::AdjustRotation(float x, float y, float z)
{
	rotation.x += x;
	rotation.y += y;
	rotation.z += z;
	rotVector = XMLoadFloat3(&rotation);
	UpdateMatrix();
}

void GameObject::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == position.x && lookAtPos.y == position.y && lookAtPos.z == position.z)
		return;

	lookAtPos.x = position.x - lookAtPos.x;
	lookAtPos.y = position.y - lookAtPos.y;
	lookAtPos.z = position.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += DirectX::XM_PI;

	SetRotation(pitch, yaw, 0.0f);
}

void GameObject::SetScale(float xScale, float yScale, float zScale)
{
	scale.x = xScale;
	scale.y = yScale;
	scale.z = zScale;
	UpdateMatrix();
}

const DirectX::XMVECTOR& GameObject::GetForwardVector()
{
	return vec_forward;
}

const DirectX::XMVECTOR& GameObject::GetRightVector()
{
	return vec_right;
}

const DirectX::XMVECTOR& GameObject::GetBackwardVector()
{
	return vec_backward;
}

const DirectX::XMVECTOR& GameObject::GetLeftVector()
{
	return vec_left;
}

void GameObject::UpdateMatrix()
{
	assert("UpdateMatrix must be overridden." && 0);
}

void GameObject::UpdateDirectionVectors()
{
	DirectX::XMMATRIX vecRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, 0.0f);
	vec_forward = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	vec_backward = DirectX::XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	vec_left = DirectX::XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	vec_right = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}