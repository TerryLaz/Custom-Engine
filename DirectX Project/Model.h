// ELEFTERIOS LAZARIDIS
#pragma once
#define NOMINMAX
#include "Mesh.h"
#include <DirectXCollision.h>
#include <fstream>
#include <sstream>
#include <filesystem>

class Model
{
private:
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader = nullptr;
	ConstantBuffer<CB_PS_Material>* cb_ps_material = nullptr;

	std::string directory = "";
	std::vector<Mesh> meshes;
	DirectX::BoundingBox boundingBox;
	
	bool LoadModel(const std::string& filePath);
	std::vector<Material> LoadMTL(const std::string& mtlFilePath);
	void LoadTextures(std::vector<Texture>& materialTextures, std::vector<Material>& materials, Material& currentMaterial) const;
	std::string GetDirectoryFromPath(const std::string& filePath);
public:
	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_Material>& cb_ps_material);
	void Draw(const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewProjectionMatrix);

	void SetSpecular(float specular);
	void SetReflective(int reflect);
	const DirectX::BoundingBox& GetBoundingBox() const;
};