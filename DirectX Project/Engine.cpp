// ELEFTERIOS LAZARIDIS
#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, int width, int height)
{
	timer.Start();

    if (!window.Initialize(hInstance, width, height))
        return false;

    if (!graphics.Initialize(window.GetHandle(), width, height))
        return false;

    return true;
}

bool Engine::ProcessMessages()
{
    return window.ProcessMessages();
}

void Engine::Update()
{
	float dt = float(timer.GetMilisecondsElapsed());
	timer.Restart();
	
	float cameraSpeed = 0.2f;
	float rotationSpeed = 0.003f;

	while (!window.keyboard.CharBufferIsEmpty())
	{
		window.keyboard.ReadChar();
	}

	while (!window.keyboard.KeyBufferIsEmpty())
	{
		Keyboard::KeyboardEvent kbe = window.keyboard.ReadKey();
		kbe.GetKeyCode();
	}

	while (!window.mouse.EventBufferIsEmpty())
	{
		Mouse::MouseEvent me = window.mouse.ReadEvent();
		if (window.mouse.IsRightDown())
		{
			if (me.GetType() == Mouse::MouseEvent::RAW_MOVE)
			{
				graphics.camera[graphics.moveCamera].AdjustRotation((float)me.GetPosY() * rotationSpeed, (float)me.GetPosX() * rotationSpeed, 0);
				if (graphics.moveCamera == 0)
				{
					DirectX::XMVECTOR cameraPosition = graphics.camera[0].GetPositionVector();
					DirectX::XMVECTOR cameraForward = graphics.camera[0].GetForwardVector();
					DirectX::XMVECTOR offset = DirectX::XMVectorScale(cameraForward, -50.0f);
					cameraPosition = DirectX::XMVectorAdd(cameraPosition, offset);

					graphics.renderableObjects[0].SetPosition(cameraPosition);
					graphics.renderableObjects[0].SetRotation(graphics.camera[0].GetRotationFloat3());
				}
			}
		}
	}

	// Rotate objects
	graphics.renderableObjects[2].AdjustRotation(DirectX::XMFLOAT3(0.0f, 0.001f * dt, 0.0f));
	graphics.renderableObjects[3].AdjustRotation(DirectX::XMFLOAT3(0.0f, 0.001f * dt, 0.0f));
	graphics.renderableObjects[6].AdjustRotation(DirectX::XMFLOAT3(0.0f, 0.001f * dt, 0.0f));

	// Key press
	if (window.keyboard.KeyIsPressed('W'))
	{
		graphics.camera[graphics.moveCamera].AdjustPosition(DirectX::XMVectorScale(graphics.camera[graphics.moveCamera].GetForwardVector(), cameraSpeed * dt));
		if (graphics.moveCamera == 0)
			graphics.renderableObjects[0].AdjustPosition(DirectX::XMVectorScale(graphics.camera[graphics.moveCamera].GetForwardVector(), cameraSpeed * dt));
	}
	if (window.keyboard.KeyIsPressed('S'))
	{
		graphics.camera[graphics.moveCamera].AdjustPosition(DirectX::XMVectorScale(graphics.camera[graphics.moveCamera].GetBackwardVector(), cameraSpeed * dt));
		if (graphics.moveCamera == 0)
			graphics.renderableObjects[0].AdjustPosition(DirectX::XMVectorScale(graphics.camera[graphics.moveCamera].GetBackwardVector(), cameraSpeed * dt));
	}
	if (window.keyboard.KeyIsPressed('A'))
	{
		graphics.camera[graphics.moveCamera].AdjustPosition(DirectX::XMVectorScale(graphics.camera[graphics.moveCamera].GetLeftVector(), cameraSpeed * dt));
		if (graphics.moveCamera == 0)
			graphics.renderableObjects[0].AdjustPosition(DirectX::XMVectorScale(graphics.camera[graphics.moveCamera].GetLeftVector(), cameraSpeed * dt));
	}
	if (window.keyboard.KeyIsPressed('D'))
	{
		graphics.camera[graphics.moveCamera].AdjustPosition(DirectX::XMVectorScale(graphics.camera[graphics.moveCamera].GetRightVector(), cameraSpeed * dt));
		if (graphics.moveCamera == 0)
			graphics.renderableObjects[0].AdjustPosition(DirectX::XMVectorScale(graphics.camera[graphics.moveCamera].GetRightVector(), cameraSpeed * dt));
	}
	if (window.keyboard.KeyIsPressed(VK_SHIFT))
	{
		graphics.camera[graphics.moveCamera].AdjustPosition(0.0f, cameraSpeed * dt, 0.0f);
		if (graphics.moveCamera == 0)
			graphics.renderableObjects[0].AdjustPosition(0.0f, cameraSpeed * dt, 0.0f);
	}
	if (window.keyboard.KeyIsPressed(VK_CONTROL))
	{
		graphics.camera[graphics.moveCamera].AdjustPosition(0.0f, -cameraSpeed * dt, 0.0f);
		if (graphics.moveCamera == 0)
			graphics.renderableObjects[0].AdjustPosition(0.0f, -cameraSpeed * dt, 0.0f);
	}
	if (window.keyboard.KeyIsPressed(0x31))
	{
		DirectX::XMVECTOR lightPosition = graphics.camera[0].GetPositionVector();
		DirectX::XMVECTOR cameraForward = graphics.camera[0].GetForwardVector();
		DirectX::XMVECTOR offset = DirectX::XMVectorScale(cameraForward, 50.0f);
		lightPosition = DirectX::XMVectorAdd(lightPosition, offset);

		graphics.lights[0].SetPosition(lightPosition);
		graphics.lights[0].SetRotation(graphics.camera[0].GetRotationFloat3());
	}
	if (window.keyboard.KeyIsPressed(0x32))
	{
		DirectX::XMVECTOR lightPosition = graphics.camera[0].GetPositionVector();
		DirectX::XMVECTOR cameraForward = graphics.camera[0].GetForwardVector();
		DirectX::XMVECTOR offset = DirectX::XMVectorScale(cameraForward, 50.0f);
		lightPosition = DirectX::XMVectorAdd(lightPosition, offset);

		graphics.lights[1].SetPosition(lightPosition);
		graphics.lights[1].SetRotation(graphics.camera[0].GetRotationFloat3());
	}
	if (window.keyboard.KeyIsPressed(0x33))
	{
		DirectX::XMVECTOR lightPosition = graphics.camera[0].GetPositionVector();
		DirectX::XMVECTOR cameraForward = graphics.camera[0].GetForwardVector();
		DirectX::XMVECTOR offset = DirectX::XMVectorScale(cameraForward, 50.0f);
		lightPosition = DirectX::XMVectorAdd(lightPosition, offset);

		graphics.lights[2].SetPosition(lightPosition);
		graphics.lights[2].SetRotation(graphics.camera[0].GetRotationFloat3());
	}
	if (window.keyboard.KeyIsPressed(0x34))
	{
		DirectX::XMVECTOR lightPosition = graphics.camera[0].GetPositionVector();
		DirectX::XMVECTOR cameraForward = graphics.camera[0].GetForwardVector();
		DirectX::XMVECTOR offset = DirectX::XMVectorScale(cameraForward, 50.0f);
		lightPosition = DirectX::XMVectorAdd(lightPosition, offset);

		graphics.lights[3].SetPosition(lightPosition);
		graphics.lights[3].SetRotation(graphics.camera[0].GetRotationFloat3());
	}
}

void Engine::Render()
{
    graphics.Render();
}
