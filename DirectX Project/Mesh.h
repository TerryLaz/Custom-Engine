// ELEFTERIOS LAZARIDIS
#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include "Material.h"
#include <vector>

class Mesh
{
private:
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
	ID3D11DeviceContext* deviceContext;

	std::vector<Texture> textures;
	std::vector<Vertex> vertices;
	Material material;
	int reflective = 0;
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, Material& material);
	Mesh(const Mesh& mesh);

	void Draw();

	Material GetMaterial() const;
	const std::vector<Vertex>& GetVertices() const;
	int HasSpecMap() const;
	int HasMaskMap() const;
	int IsReflective() const;
	void SetReflective(int reflect);
	float GetSpecularExponent() const;
	void SetSpecularExponent(float specular);
};