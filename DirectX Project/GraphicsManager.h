// ELEFTERIOS LAZARIDIS
#pragma once
#include "Shaders.h"
#include "Camera.h"
#include "Timer.h"
#include "Light.h"
#include "RenderableObject.h"
#include "ParticleBuffer.h"
#include "QuadTree.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>

class GraphicsManager
{
private:
	static const int nrOfLights = 4;
	int windowWidth = 0;
	int windowHeight = 0;
	int cubeMapSize = 900;
	bool renderWireframe = false;
	DirectX::XMFLOAT3 rotations[6] = {
		{0.0f, 90.0f, 0.0f},   // +X
		{0.0f, -90.0f, 0.0f},  // -X
		{-90.0f, 0.0f, 0.0f},  // +Y
		{90.0f, 0.0f, 0.0f},   // -Y
		{0.0f, 0.0f, 0.0f},    // +Z
		{0.0f, 180.0f, 0.0f}   // -Z
	};

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRS;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	D3D11_VIEWPORT viewPort;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSamplerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowMapDSVs[nrOfLights];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowMapSRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> cubeMapSamplerState;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cubeMapRTV[6];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cubeMapDSV[6];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
	D3D11_VIEWPORT cubeVP;

	VertexShader vertexShader;
	VertexShader vertexShader_particle;
	PixelShader pixelShader;
	PixelShader pixelShader_nolight;
	PixelShader pixelShader_particle;
	ComputeShader computeShader_particle;
	GeometryShader geometryShader_particle;
	HullShader hullShader;
	DomainShader domainShader;

	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_Camera> cb_camera;
	ConstantBuffer<CB_PS_Light>  cb_ps_light;
	ConstantBuffer<CB_PS_Material> cb_ps_material;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Timer fpsTimer;
	QuadTree quadTree;

	bool InitializeDirectX(HWND hwnd);
	bool InitializeShadowMapping();
	bool InitializeEnvironmentMapping();
	bool InitializeShaders();
	bool InitializeScene();

	void RenderCubemap();
	void RenderShadowMap();
	void RenderParticles();
	void RenderScene();
	void RenderImGui();
	void RenderText();

	void UpdatePixelCB();
	void ResetLights();
public:
	float shininesSlider = 50.0f;
	int renderCamera = 0;
	int moveCamera = 0;

	Camera camera[2];
	Camera depthCamera;
	Light lights[nrOfLights];
	std::vector<RenderableObject> renderableObjects;
	std::vector<ParticleBuffer> particleBuffers;

	~GraphicsManager();

	bool Initialize(HWND hwnd, int width, int height);
	void Render();
	DirectX::XMFLOAT3 ConvertToRadians(DirectX::XMFLOAT3 rotation);
};
