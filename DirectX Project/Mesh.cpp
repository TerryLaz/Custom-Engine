// ELEFTERIOS LAZARIDIS
#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, Material& material)
	:deviceContext(deviceContext), textures(textures), material(material)
{
	if (!vertexBuffer.Initialize(device, vertices.data(), (UINT)vertices.size()))
		MessageBox(nullptr, L"Failed to initialize vertex buffer for mesh", L"Error message", MB_OK | MB_ICONINFORMATION);

	if (!indexBuffer.Initialize(device, indices.data(), (UINT)indices.size()))
		MessageBox(nullptr, L"Failed to initialize index buffer for mesh", L"Error message", MB_OK | MB_ICONINFORMATION);

	this->vertices = vertices;
}

Mesh::Mesh(const Mesh& mesh)
{
	deviceContext = mesh.deviceContext;
	indexBuffer = mesh.indexBuffer;
	vertexBuffer = mesh.vertexBuffer;
	textures = mesh.textures;
	material = mesh.material;
	reflective = mesh.reflective;
	vertices = mesh.vertices;
}

void Mesh::Draw()
{
	UINT offset = 0;

	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i].GetType() == TextureType::DIFFUSE){
			deviceContext->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
		}
		else if (textures[i].GetType() == TextureType::SPECULAR) {
			deviceContext->PSSetShaderResources(1, 1, textures[i].GetTextureResourceViewAddress());
		}
		else if (textures[i].GetType() == TextureType::MASK) {
			deviceContext->PSSetShaderResources(2, 1, textures[i].GetTextureResourceViewAddress());
		}
	}

	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(indexBuffer.IndexCount(), 0, 0);
}

Material Mesh::GetMaterial() const
{
	return material;
}

const std::vector<Vertex>& Mesh::GetVertices() const
{
	return vertices;
}

int Mesh::HasSpecMap() const
{
	int result = 0;
	if (!material.map_Ks.empty())
		result = 1;
	return result;
}

int Mesh::HasMaskMap() const
{
	int result = 0;
	if (!material.map_d.empty())
		result = 1;
	return result;
}

int Mesh::IsReflective() const
{
	return reflective;
}

void Mesh::SetReflective(int reflect)
{
	reflective = reflect;
}

float Mesh::GetSpecularExponent() const
{
	return material.Ns;
}

void Mesh::SetSpecularExponent(float specular)
{
	material.Ns = specular;
}
