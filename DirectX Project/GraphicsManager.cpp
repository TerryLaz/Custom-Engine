// ELEFTERIOS LAZARIDIS
#include "GraphicsManager.h"

GraphicsManager::~GraphicsManager()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool GraphicsManager::Initialize(HWND hwnd, int width, int height)
{
	fpsTimer.Start();
	windowWidth = width;
	windowHeight = height;

	if (!InitializeDirectX(hwnd))
		return false;
	
	if (!InitializeShadowMapping())
		return false;
	
	if (!InitializeEnvironmentMapping())
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene())
		return false;

	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

void GraphicsManager::Render()
{
	RenderCubemap();
	RenderShadowMap();
	RenderScene();
	RenderParticles();
	RenderText();
	RenderImGui();

	swapChain->Present(0, 0);
}

DirectX::XMFLOAT3 GraphicsManager::ConvertToRadians(DirectX::XMFLOAT3 rotation)
{
	DirectX::XMFLOAT3 rotationInRadians = {};
	rotationInRadians.x = DirectX::XMConvertToRadians(rotation.x);
	rotationInRadians.y = DirectX::XMConvertToRadians(rotation.y);
	rotationInRadians.z = DirectX::XMConvertToRadians(rotation.z);
	return rotationInRadians;
}

bool GraphicsManager::InitializeDirectX(HWND hwnd)
{
	HRESULT hr;
	// Create device and swapchain
	{
		DXGI_SWAP_CHAIN_DESC scd = { 0 };
		scd.BufferDesc.Width = windowWidth;
		scd.BufferDesc.Height = windowHeight;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.RefreshRate.Numerator = 0;
		scd.BufferDesc.RefreshRate.Denominator = 0;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr, //IDXGI Adapter
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr, //For software driver type
			0, //Flags for runtime layers
			nullptr, //Feature levels array
			0, //# of feature levels in array
			D3D11_SDK_VERSION,
			&scd, //Swapchain description
			&swapChain, //Swapchain Address
			&device, //Device Address
			nullptr, //Supported feature level
			&deviceContext //Device Context Address
		);
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create device and swap chain", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

	// Create render target view
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create backBuffer", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
		hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create RTV", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

	// Create depth stencil view
	{
		CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, windowWidth, windowHeight);
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
		hr = device->CreateTexture2D(&depthStencilTextureDesc, nullptr, depthTexture.GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create depth stencil buffer", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
		hr = device->CreateDepthStencilView(depthTexture.Get(), nullptr, depthStencilView.GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create depth stencil view", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

	// Create depth stencil state
	{
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depthstencildesc, depthStencilState.GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create depth stencil state", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

	// Create sampler state
	{
		D3D11_SAMPLER_DESC sd = {};
		sd.Filter = D3D11_FILTER_ANISOTROPIC;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd.MipLODBias = 0.0f;
		sd.MinLOD = 0.0f;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sd, samplerState.GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create sampler state", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

	// Create Rasterizer States
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.FrontCounterClockwise = false;
		hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create rasterizer state", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}

		D3D11_RASTERIZER_DESC rasterizerDescWF = {};
		rasterizerDescWF.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDescWF.CullMode = D3D11_CULL_NONE;
		rasterizerDescWF.DepthClipEnable = true;
		hr = device->CreateRasterizerState(&rasterizerDescWF, wireframeRS.GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create rasterizer state", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

	// Create blend state
	{
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = {};
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		D3D11_BLEND_DESC bsd = {};
		bsd.RenderTarget[0] = rtbd;
		hr = device->CreateBlendState(&bsd, blendState.GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create blend state", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

	viewPort.Width = static_cast<float>(windowWidth);
	viewPort.Height = static_cast<float>(windowHeight);
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(deviceContext.Get());
	spriteFont = std::make_unique<DirectX::SpriteFont>(device.Get(), L"Fonts\\comic_sans_ms_16.spritefont");

	return true;
}

bool GraphicsManager::InitializeShadowMapping()
{
	// Create shadow map texture
	D3D11_TEXTURE2D_DESC shadowMapDesc = {};
	shadowMapDesc.Width = windowWidth;
	shadowMapDesc.Height = windowHeight;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = nrOfLights;
	shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.SampleDesc.Quality = 0;
	shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.CPUAccessFlags = 0;
	shadowMapDesc.MiscFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowMap;
	HRESULT hr = device->CreateTexture2D(&shadowMapDesc, nullptr, shadowMap.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create texture for shadow map", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	// Create depth/stencil view for shadow map
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.MipSlice = 0;
	dsvDesc.Texture2DArray.ArraySize = 1;
	for (int i = 0; i < nrOfLights; i++)
	{
		dsvDesc.Texture2DArray.FirstArraySlice = i;
		hr = device->CreateDepthStencilView(shadowMap.Get(), &dsvDesc, shadowMapDSVs[i].GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create DSV for shadow map", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}
	// Create shader resource view for shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = nrOfLights;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	hr = device->CreateShaderResourceView(shadowMap.Get(), &srvDesc, shadowMapSRV.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create SRV for shadow map", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	// Create shadowMap sampler state
	D3D11_SAMPLER_DESC shadowSamplerDesc = {};
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.BorderColor[0] = 1.0f;
	shadowSamplerDesc.BorderColor[1] = 1.0f;
	shadowSamplerDesc.BorderColor[2] = 1.0f;
	shadowSamplerDesc.BorderColor[3] = 1.0f;
	shadowSamplerDesc.MaxAnisotropy = 1;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	hr = device->CreateSamplerState(&shadowSamplerDesc, shadowSamplerState.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create sampler state", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	// Create Shadow Rasterizer State
	D3D11_RASTERIZER_DESC shadowRasterizerDesc = {};
	shadowRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	shadowRasterizerDesc.CullMode = D3D11_CULL_FRONT;
	shadowRasterizerDesc.DepthClipEnable = true;
	shadowRasterizerDesc.FrontCounterClockwise = false;
	shadowRasterizerDesc.DepthBias = 1000;
	shadowRasterizerDesc.SlopeScaledDepthBias = 1.0f;
	hr = device->CreateRasterizerState(&shadowRasterizerDesc, shadowRasterizerState.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create shadow rasterizer state", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	return true;
}

bool GraphicsManager::InitializeEnvironmentMapping()
{
	// Texture description for a cube map
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = cubeMapSize;
	texDesc.Height = cubeMapSize;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6; // 6 faces for the cube
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeMapTexture;
	HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, cubeMapTexture.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create texture for cube map", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;
	for (UINT face = 0; face < 6; ++face)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = face;
		HRESULT hr = device->CreateRenderTargetView(cubeMapTexture.Get(), &rtvDesc, cubeMapRTV[face].GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(nullptr, L"Failed to create RTV for cube map", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

	// Create the shader resource view for the cube map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;
	hr = device->CreateShaderResourceView(cubeMapTexture.Get(), &srvDesc, cubeMapSRV.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create SRV for cubemap", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	// Create the depth buffer
	D3D11_TEXTURE2D_DESC dsDesc = texDesc;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsDesc.MiscFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture;
	hr = device->CreateTexture2D(&dsDesc, nullptr, depthStencilTexture.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create texture for cubemap DSV", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = dsDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.ArraySize = 1;
	dsvDesc.Texture2DArray.MipSlice = 0;
	for (UINT face = 0; face < 6; ++face) {
		dsvDesc.Texture2DArray.FirstArraySlice = face;
		hr = device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvDesc, cubeMapDSV[face].GetAddressOf());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to create DSV for cubemap", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

	// Define the sampler state description for the cubemap
	D3D11_SAMPLER_DESC cubeSampDesc = {};
	cubeSampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	cubeSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	cubeSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	cubeSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	cubeSampDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	cubeSampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	cubeSampDesc.MipLODBias = 0.0f;
	cubeSampDesc.MinLOD = 0.0f;
	cubeSampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&cubeSampDesc, cubeMapSamplerState.GetAddressOf());
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Failed to create sampler state for cubemap", L"Error message", MB_OK | MB_ICONINFORMATION);
		return false;
	}

	// setup cube view port
	cubeVP.Width = static_cast<float>(cubeMapSize);
	cubeVP.Height = static_cast<float>(cubeMapSize);
	cubeVP.TopLeftX = 0.0f;
	cubeVP.TopLeftY = 0.0f;
	cubeVP.MinDepth = 0.0f;
	cubeVP.MaxDepth = 1.0f;

	return true;
}

bool GraphicsManager::InitializeShaders()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	if (!vertexShader.Initialize(device.Get(), layout, (UINT)std::size(layout), L"VertexShader.cso"))
		return false;

	if (!vertexShader_particle.Initialize(device.Get(), nullptr, 0, L"VertexShader_particle.cso"))
		return false;

	if (!pixelShader.Initialize(device.Get(), L"PixelShader.cso"))
		return false;

	if (!pixelShader_nolight.Initialize(device.Get(), L"PixelShader_nolight.cso"))
		return false;

	if (!pixelShader_particle.Initialize(device.Get(), L"PixelShader_particle.cso"))
		return false;

	if (!computeShader_particle.Initialize(device.Get(), L"ComputeShader_particle.cso"))
		return false;

	if (!geometryShader_particle.Initialize(device.Get(), L"GeometryShader_particle.cso"))
		return false;

	if (!hullShader.Initialize(device.Get(), L"HullShader.cso"))
		return false;

	if (!domainShader.Initialize(device.Get(), L"DomainShader.cso"))
		return false;

	return true;
}

bool GraphicsManager::InitializeScene()
{
	//Initialize constant buffers
	{
		HRESULT hr = cb_vs_vertexshader.Initialize(device.Get(), deviceContext.Get());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to initialize cb_vs_vertexshader constant buffer", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
		hr = cb_ps_light.Initialize(device.Get(), deviceContext.Get());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to initialize cb_ps_light constant buffer", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
		hr = cb_camera.Initialize(device.Get(), deviceContext.Get());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to initialize cb_camera constant buffer", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
		hr = cb_ps_material.Initialize(device.Get(), deviceContext.Get());
		if (FAILED(hr)) {
			MessageBox(nullptr, L"Failed to initialize cb_ps_material constant buffer", L"Error message", MB_OK | MB_ICONINFORMATION);
			return false;
		}
	}

	//Initialize Models
	{
		RenderableObject camera; //#0
		if (!camera.Initialize("Objects\\camera", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
			return false;
		camera.SetScale(5.0f, 5.0f, 5.0f);
		camera.SetPosition(0.0f, 100.0f, -150.0f);
		renderableObjects.push_back(camera);

		RenderableObject sponza; //#1
		if (!sponza.Initialize("Objects\\Sponza\\sponza", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
			return false;
		renderableObjects.push_back(sponza);

		RenderableObject nanosuit; //#2
		if (!nanosuit.Initialize("Objects\\nanosuit\\nanosuit", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
			return false;
		nanosuit.SetScale(10.0f, 10.0f, 10.0f);
		renderableObjects.push_back(nanosuit);

		RenderableObject cubeNoMat; //#3
		if (!cubeNoMat.Initialize("Objects\\Cubes\\cubeNoMat", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
			return false;
		cubeNoMat.SetScale(100.0f, 100.0f, 100.0f);
		cubeNoMat.SetPosition(300.0f, 100.0f, 0.0f);
		renderableObjects.push_back(cubeNoMat);

		RenderableObject shinyCube; //#4
		if (!shinyCube.Initialize("Objects\\Cubes\\ShinyCube\\shinyCube", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
			return false;
		shinyCube.SetScale(100.0f, 100.0f, 100.0f);
		shinyCube.SetPosition(-300.0f, 100.0f, 0.0f);
		renderableObjects.push_back(shinyCube);

		RenderableObject reflectiveCube; //#5
		if (!reflectiveCube.Initialize("Objects\\Cubes\\cubeNoMat", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
			return false;
		reflectiveCube.SetReflective(1);
		reflectiveCube.SetScale(100.0f, 100.0f, 100.0f);
		reflectiveCube.SetPosition(0.0f, 500.0f, 0.0f);
		renderableObjects.push_back(reflectiveCube);

		RenderableObject diamonds; //#6
		if (!diamonds.Initialize("Objects\\Diamonds\\Diamonds", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
			return false;
		diamonds.SetPosition(0.0f, 500.0f, 0.0f);
		diamonds.SetScale(1.5f, 1.5f, 1.5f);
		renderableObjects.push_back(diamonds);

		RenderableObject sphere; //#7
		if (!sphere.Initialize("Objects\\sphere", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
			return false;
		sphere.SetScale(100.0f, 100.0f, 100.0f);
		sphere.SetPosition(500.0f, 500.0f, 0.0f);
		renderableObjects.push_back(sphere);

		RenderableObject suzanne; //#8
		if (!suzanne.Initialize("Objects\\suzanne", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
			return false;
		suzanne.SetScale(100.0f, 100.0f, 100.0f);
		suzanne.SetPosition(-500.0f, 500.0f, 0.0f);
		renderableObjects.push_back(suzanne);

		RenderableObject reflectiveSuzanne; //#9
		if (!reflectiveSuzanne.Initialize("Objects\\suzanne", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
			return false;
		reflectiveSuzanne.SetReflective(1);
		reflectiveSuzanne.SetScale(50.0f, 50.0f, 50.0f);
		reflectiveSuzanne.SetPosition(0.0f, 600.0f, 0.0f);
		renderableObjects.push_back(reflectiveSuzanne);

		int cubesPerSide = 5;
		float cubeSpacing = 100.0f;
		for (int x = 0; x < cubesPerSide; x++)
		{
			for (int y = 0; y < cubesPerSide; y++)
			{
				for (int z = 0; z < cubesPerSide; z++)
				{
					RenderableObject cube; //#10-135
					if (!cube.Initialize("Objects\\Cubes\\cubeNoMat", device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_material))
						return false;
					cube.SetPosition(x * cubeSpacing, y * cubeSpacing - 1000.0f, z * cubeSpacing);
					cube.SetScale(50.0f, 50.0f, 50.0f);
					renderableObjects.push_back(cube);
				}
			}
		}
	}

	// Initialize lights
	{
		if (!lights[0].Initialize(device.Get(), deviceContext.Get(), "Objects\\Lights\\Cone", cb_vs_vertexshader, cb_ps_material))
			return false;
		lights[0].SetPosition(0.0f, 300.0f, 0.0f);
		lights[0].SetRotation(ConvertToRadians(DirectX::XMFLOAT3(90.0f,0.0f,0.0f)));
		lights[0].SetScale(10.0f, 10.0f, 10.0f);

		if (!lights[1].Initialize(device.Get(), deviceContext.Get(), "Objects\\Lights\\Cone", cb_vs_vertexshader, cb_ps_material))
			return false;
		lights[1].SetPosition(350.0f, 300.0f, 0.0f);
		lights[1].SetRotation(ConvertToRadians(DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f)));
		lights[1].SetScale(10.0f, 10.0f, 10.0f);

		if (!lights[2].Initialize(device.Get(), deviceContext.Get(), "Objects\\Lights\\Cone", cb_vs_vertexshader, cb_ps_material))
			return false;
		lights[2].SetPosition(-350.0f, 300.0f, 0.0f);
		lights[2].SetRotation(ConvertToRadians(DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f)));
		lights[2].SetScale(10.0f, 10.0f, 10.0f);

		if (!lights[3].Initialize(device.Get(), deviceContext.Get(), "Objects\\Lights\\Light", cb_vs_vertexshader, cb_ps_material))
			return false;
		lights[3].SetPosition(0.0f, 2000.0f, 100.0f);
		lights[3].SetRotation(ConvertToRadians(DirectX::XMFLOAT3(85.0f, 0.0f, 0.0f)));
		lights[3].SetScale(2000.0f, 2000.0f, 2000.0f);

		// Setup lights to default values
		ResetLights();
	}

	// Initialize particles
	{
		ParticleBuffer fireParticles;
		fireParticles.Initialize(device.Get(), deviceContext.Get(), L"Particles\\fire.png", DirectX::XMFLOAT3(620, 125, 220));
		particleBuffers.push_back(fireParticles);

		ParticleBuffer fireParticles2;
		fireParticles2.Initialize(device.Get(), deviceContext.Get(), L"Particles\\fire.png", DirectX::XMFLOAT3(620, 125, -135));
		particleBuffers.push_back(fireParticles2);

		ParticleBuffer fireParticles3;
		fireParticles3.Initialize(device.Get(), deviceContext.Get(), L"Particles\\fire.png", DirectX::XMFLOAT3(-485, 125, 220));
		particleBuffers.push_back(fireParticles3);

		ParticleBuffer fireParticles4;
		fireParticles4.Initialize(device.Get(), deviceContext.Get(), L"Particles\\fire.png", DirectX::XMFLOAT3(-485, 125, -135));
		particleBuffers.push_back(fireParticles4);
	}

	// Setup cameras
	{
		camera[0].SetPosition(0.0f, 100.0f, -100.0f);
		camera[0].SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);

		camera[1].SetPosition(-500.0f, 300.0f, -100.0f);
		camera[1].SetProjectionValues(90.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);
	}

	// Populate the quadtree
	{
		for (auto& object : renderableObjects) {
			quadTree.insertObject(&object);
		}
	}

	return true;
}

void GraphicsManager::RenderCubemap()
{
	float clearColor[] = { 0.20f, 0.20f, 0.20f, 1.0f };
	deviceContext->RSSetViewports(1, &cubeVP); // Set cube view port
	deviceContext->RSSetState(rasterizerState.Get()); // Set rasterizer
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0); // Set depth stencil state
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // Set primitive topology
	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf()); // Set sampler state
	deviceContext->PSSetSamplers(1, 1, shadowSamplerState.GetAddressOf()); // Set shadowmap sampler state
	deviceContext->PSSetSamplers(2, 1, cubeMapSamplerState.GetAddressOf()); // Set cubemap sampler state
	deviceContext->IASetInputLayout(vertexShader.GetInputLayout()); // Set input layout
	deviceContext->VSSetShader(vertexShader.GetShader(), nullptr, 0); // Set vertex shader
	deviceContext->PSSetShader(pixelShader.GetShader(), nullptr, 0); // Set pixel shader

	deviceContext->PSSetShaderResources(3, 1, shadowMapSRV.GetAddressOf());
	deviceContext->PSSetShaderResources(4, 1, cubeMapSRV.GetAddressOf());
	
	deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.GetAddressOf()); // Set constant buffer

	depthCamera.SetProjectionValues(90.0f, 1.0f, 0.1f, 3000.0f);
	depthCamera.SetPosition(renderableObjects[5].GetPositionFloat3());
	for (int i = 0; i < 6; ++i)
	{
		deviceContext->ClearRenderTargetView(cubeMapRTV[i].Get(), clearColor); // Clear rtv color
		deviceContext->ClearDepthStencilView(cubeMapDSV[i].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); // Clear DSV cube map
		deviceContext->OMSetRenderTargets(1, cubeMapRTV[i].GetAddressOf(), cubeMapDSV[i].Get()); // set render target

		// Adjust camera rotation to face each cubemap direction
		depthCamera.SetRotation(ConvertToRadians(rotations[i]));

		// Render objects from reflective surface view
		for (int j = 0; j < renderableObjects.size(); j++)
		{
			renderableObjects[j].Draw(depthCamera.GetViewMatrix() * depthCamera.GetProjectionMatrix());
		}
	}
}

void GraphicsManager::RenderShadowMap()
{
	deviceContext->RSSetViewports(1, &viewPort); // Set view port
	deviceContext->RSSetState(shadowRasterizerState.Get()); // Set rasterizer state shadow map
	deviceContext->PSSetShader(nullptr, nullptr, 0); // Unbind pixel shader

	for (int i = 0; i < nrOfLights; i++)
	{
		deviceContext->ClearDepthStencilView(shadowMapDSVs[i].Get(), D3D11_CLEAR_DEPTH, 1.0f, 0); // Clear DSV shadow map
		deviceContext->OMSetRenderTargets(0, nullptr, shadowMapDSVs[i].Get()); // Set render target shadow map

		// Setup camera for shadow mapping
		depthCamera.SetPosition(lights[i].GetPositionFloat3());
		depthCamera.SetRotation(lights[i].GetRotationFloat3());
		if (i < 3) {
			depthCamera.SetProjectionValues(cb_ps_light.data.spotFalloff * 360, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);
		}
		else {
			depthCamera.SetOrthographicValues(windowWidth, windowHeight, 0.1f, 5000.0f);
		}
		cb_vs_vertexshader.data.lightViewProjection[i] = depthCamera.GetViewMatrix() * depthCamera.GetProjectionMatrix();
		cb_vs_vertexshader.ApplyChanges();

		// Render objects from light view
		for (int j = 0; j < renderableObjects.size(); j++)
		{
			renderableObjects[j].Draw(depthCamera.GetViewMatrix() * depthCamera.GetProjectionMatrix());
		}
	}
}

void GraphicsManager::RenderParticles()
{	
	deviceContext->IASetInputLayout(nullptr); // Set input layout
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->VSSetShader(vertexShader_particle.GetShader(), nullptr, 0); // Set vertex shader for particles
	deviceContext->GSSetShader(geometryShader_particle.GetShader(), nullptr, 0); // Set geometry shader
	deviceContext->PSSetShader(pixelShader_particle.GetShader(), nullptr, 0); // Set pixel shader for particles

	// Set and update constant buffers
	deviceContext->GSSetConstantBuffers(0, 1, cb_camera.GetAddressOf());
	cb_camera.data.cameraPosition = camera[renderCamera].GetPositionFloat3();
	cb_camera.data.viewProj = camera[renderCamera].GetViewMatrix() * camera[renderCamera].GetProjectionMatrix();
	cb_camera.ApplyChanges();

	for (int i = 0; i < particleBuffers.size(); i++)
	{
		// Update Particles
		deviceContext->CSSetShader(computeShader_particle.GetShader(), nullptr, 0); // Set the compute shader
		deviceContext->CSSetUnorderedAccessViews(0, 1, particleBuffers[i].GetUAVAddress(), nullptr); // Bind the UAV of particle buffer
		deviceContext->Dispatch(UINT(std::ceil(particleBuffers[i].NrOfParticles() / 32.0f)), 1u, 1u);// Dispatch the compute shader
		ID3D11UnorderedAccessView* nullUAVs[] = { nullptr };
		deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);

		// Set the particle buffer as a shader resource for vertex pulling
		deviceContext->VSSetShaderResources(0, 1, particleBuffers[i].GetSRVAddress());
		deviceContext->PSSetShaderResources(0, 1, particleBuffers[i].GetTextureSRVAddress());

		// Draw the particles
		deviceContext->Draw(particleBuffers[i].NrOfParticles(), 0u);
	}

	deviceContext->GSSetShader(nullptr, nullptr, 0);
	ID3D11ShaderResourceView* nullSRV = nullptr;
	deviceContext->VSSetShaderResources(0, 1, &nullSRV);
}

void GraphicsManager::RenderScene()
{
	float clearColor[] = { 0.20f, 0.20f, 0.20f, 1.0f };
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), clearColor); // Clear rtv color
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); // Clear DSV
	if (renderWireframe) {
		deviceContext->RSSetState(wireframeRS.Get()); // Set wireframe RS
	}
	else {
		deviceContext->RSSetState(rasterizerState.Get()); // Set rasterizer state
	}
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST); // Set primitive topology
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get()); // Set render target
	deviceContext->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF); // Set blend state
	deviceContext->PSSetShader(pixelShader.GetShader(), nullptr, 0); // Set pixel shader
	deviceContext->HSSetShader(hullShader.GetShader(), nullptr, 0); // Set hull shader
	deviceContext->DSSetShader(domainShader.GetShader(), nullptr, 0); // Set domain shader

	//Pass the SRVs to pixel shader
	deviceContext->PSSetShaderResources(3, 1, shadowMapSRV.GetAddressOf());
	deviceContext->PSSetShaderResources(4, 1, cubeMapSRV.GetAddressOf());

	// Set and update constnat buffers
	UpdatePixelCB();
	
	// Draw visible objects
	DirectX::BoundingFrustum viewFrustum(camera[0].GetProjectionMatrix());
	DirectX::XMVECTOR detView = DirectX::XMMatrixDeterminant(camera[0].GetViewMatrix());
	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(&detView, camera[0].GetViewMatrix());
	viewFrustum.Transform(viewFrustum, invView);
	std::vector<RenderableObject*> visibleObjects = quadTree.cullWithFrustum(viewFrustum);
	for (RenderableObject* object : visibleObjects) {
		object->Draw(camera[renderCamera].GetViewMatrix() * camera[renderCamera].GetProjectionMatrix());
	}
	// Draw camera
	renderableObjects[0].Draw(camera[renderCamera].GetViewMatrix() * camera[renderCamera].GetProjectionMatrix());
	// Draw Lights
	deviceContext->PSSetShader(pixelShader_nolight.GetShader(), nullptr, 0); // Set pixelShader for light objects
	for (int k = 0; k < nrOfLights; k++)
	{
		lights[k].Draw(camera[renderCamera].GetViewMatrix() * camera[renderCamera].GetProjectionMatrix());
	}

	//deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); // Unbind blend state
	deviceContext->HSSetShader(nullptr, nullptr, 0); // Unbind hull shader
	deviceContext->DSSetShader(nullptr, nullptr, 0); // Unbind domain shader
}

void GraphicsManager::RenderImGui()
{
	//Start ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	ImGui::Begin("Light Controls");
	if (ImGui::Button("Reset")) { ResetLights(); }
	ImGui::Checkbox("Wireframe", &renderWireframe);
	ImGui::SliderInt("Render camera", &renderCamera, 0, 1);
	ImGui::SliderInt("Move camera", &moveCamera, 0, 1);
	ImGui::DragFloat3("Ambient Light Color", &cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strength", &cb_ps_light.data.ambientLightStrength, 0.01f, 0.0f, 1.0f);
	ImGui::NewLine();
	ImGui::DragFloat3("Directional Light Color", &cb_ps_light.data.directionalLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Directional Light Strength", &cb_ps_light.data.directionalLightStrength, 0.01f, 0.0f, 1.0f);
	ImGui::NewLine();
	ImGui::DragFloat3("Spotlights Color", &cb_ps_light.data.spotlightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Spotlights Strength", &cb_ps_light.data.spotlightStrength, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Spotlights Falloff", &cb_ps_light.data.spotFalloff, 0.01f, 0.10f, 0.99f);
	ImGui::NewLine();
	ImGui::DragFloat("Attenuation A", &cb_ps_light.data.attenuation_a, 0.1f, 0.1f, 10.0f);
	ImGui::DragFloat("Attenuation B", &cb_ps_light.data.attenuation_b, 0.0001f, 0.0f, 10.0f, "%.5f");
	ImGui::DragFloat("Attenuation C", &cb_ps_light.data.attenuation_c, 0.00001f, 0.0f, 10.0f, "%.6f");
	ImGui::NewLine();
	ImGui::DragFloat("Cube Specular", &shininesSlider, 1.0f, 1.0f, 1000.0f);
	renderableObjects[4].SetSpecular(shininesSlider);
	ImGui::End();

	ImGui::Begin("Navigation Controls");
	ImGui::Text("W - Move forward");
	ImGui::Text("S - Move back");
	ImGui::Text("A - Move left");
	ImGui::Text("D - Move right");
	ImGui::Text("Hold RM - Rotate camera");
	ImGui::Text("1-4 - Move light to camera");
	ImGui::Text("Shift - Move camera up");
	ImGui::Text("Ctrl - Move camera down");
	ImGui::End();

	//Assemble together draw data
	ImGui::Render();
	//Render draw data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void GraphicsManager::RenderText()
{
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), fpsString.c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();
}

void GraphicsManager::UpdatePixelCB()
{
	deviceContext->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf()); // Set PS light constant buffer
	deviceContext->PSSetConstantBuffers(1, 1, cb_camera.GetAddressOf()); // Set PS camera constant buffer
	deviceContext->HSSetConstantBuffers(0, 1, cb_camera.GetAddressOf()); // Set HS camera constant buffer
	deviceContext->DSSetConstantBuffers(0, 1, cb_camera.GetAddressOf()); // Set DS camera constant buffer
	for (int i = 0; i < nrOfLights; i++)
	{
		cb_ps_light.data.lightsPosition[i] = { lights[i].GetPositionFloat3().x, lights[i].GetPositionFloat3().y, lights[i].GetPositionFloat3().z, 1.0f };
		DirectX::XMStoreFloat4(&cb_ps_light.data.lightsDir[i], lights[i].GetForwardVector());
	}
	cb_camera.data.cameraPosition = camera[renderCamera].GetPositionFloat3();
	cb_camera.data.viewProj = camera[renderCamera].GetViewMatrix() * camera[renderCamera].GetProjectionMatrix();
	cb_ps_light.ApplyChanges();
	cb_camera.ApplyChanges();
}

void GraphicsManager::ResetLights()
{
	cb_ps_light.data.ambientLightColor = { 1.0f, 1.0f, 1.0f};
	cb_ps_light.data.ambientLightStrength = 1.0f;
	cb_ps_light.data.directionalLightColor = { 1.0f, 1.0f, 1.0f };
	cb_ps_light.data.directionalLightStrength = 1.0f;
	cb_ps_light.data.spotFalloff = 0.8f;
	cb_ps_light.data.spotlightColor = { 1.0f, 1.0f, 1.0f };
	cb_ps_light.data.spotlightStrength = 1.0f;
	cb_ps_light.data.attenuation_a = 1.0f;
	cb_ps_light.data.attenuation_b = 0.00045f;
	cb_ps_light.data.attenuation_c = 0.000075f;
}