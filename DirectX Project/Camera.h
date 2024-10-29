// ELEFTERIOS LAZARIDIS
#pragma once
#include "GameObject.h"

class Camera : public GameObject
{
private:
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
	
	void UpdateMatrix() override;
public:
	Camera();

	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void SetOrthographicValues(int width, int height, float nearZ, float farZ);

	const DirectX::XMMATRIX& GetViewMatrix() const;
	const DirectX::XMMATRIX& GetProjectionMatrix() const;
};