// ELEFTERIOS LAZARIDIS
#include "Model.h"

bool Model::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_Material>& cb_ps_material)
{
    this->device = device;
    this->deviceContext = deviceContext;
    this->cb_vs_vertexshader = &cb_vs_vertexshader;
    this->cb_ps_material = &cb_ps_material;

	if (!LoadModel(filePath))
		return false;

    return true;
}

void Model::Draw(const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewProjectionMatrix)
{	
    deviceContext->PSSetConstantBuffers(2, 1, cb_ps_material->GetAddressOf());

    for (int i = 0; i < meshes.size(); i++)
    {
        //Update vertex constant buffer
        cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewProjectionMatrix;
        cb_vs_vertexshader->data.worldMatrix = worldMatrix;
        cb_vs_vertexshader->ApplyChanges();

        //Update material constant buffer
        cb_ps_material->data.specularExponent = meshes[i].GetMaterial().Ns;
        cb_ps_material->data.hasSpecMap = meshes[i].HasSpecMap();
        cb_ps_material->data.hasMask = meshes[i].HasMaskMap();
        cb_ps_material->data.isReflective = meshes[i].IsReflective();
        cb_ps_material->ApplyChanges();

        meshes[i].Draw();
    }
}

void Model::SetSpecular(float specular)
{
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].SetSpecularExponent(specular);
    }
}

void Model::SetReflective(int reflect)
{
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].SetReflective(reflect);
    }
}

const DirectX::BoundingBox& Model::GetBoundingBox() const
{
    return boundingBox;
}

std::vector<Material> Model::LoadMTL(const std::string& mtlFilePath)
{
    std::vector<Material> materials;
    Material currentMaterial;

    std::ifstream mtlFile(mtlFilePath);
    if (!mtlFile.is_open()) {
        MessageBox(nullptr, L"Failed to open MTL file", L"Error message", MB_OK | MB_ICONINFORMATION);
        return materials;
    }

    std::string mtlLine;
    while (std::getline(mtlFile, mtlLine))
    { 
        std::istringstream mtlIss(mtlLine);
        std::string mtlToken;
        mtlIss >> mtlToken;

        if (mtlToken == "newmtl") // New material
        {
            if (!currentMaterial.name.empty())
                materials.push_back(currentMaterial);

            currentMaterial = Material();
            mtlIss >> currentMaterial.name;
        } 
        else if (mtlToken == "Ns") // Specular exponent
        {
            mtlIss >> currentMaterial.Ns;
        }  
        else if (mtlToken == "Ka") // Ambient color
        {
            mtlIss >> currentMaterial.Ka.x >> currentMaterial.Ka.y >> currentMaterial.Ka.z;
        }
        else if (mtlToken == "Kd") // Diffuse color
        {
            mtlIss >> currentMaterial.Kd.x >> currentMaterial.Kd.y >> currentMaterial.Kd.z;
        }
        else if (mtlToken == "Ks") // Specular color
        {
            mtlIss >> currentMaterial.Ks.x >> currentMaterial.Ks.y >> currentMaterial.Ks.z;
        }   
        else if (mtlToken == "map_Ka") // Ambient texture
        {
            mtlIss >> currentMaterial.map_Ka;
        }
        else if (mtlToken == "map_Kd") // Diffuse texture
        {
            mtlIss >> currentMaterial.map_Kd;
        }
        else if (mtlToken == "map_Ks") // Specular texture
        {
            mtlIss >> currentMaterial.map_Ks;
        }
        else if (mtlToken == "map_d") // Mask
        {
            mtlIss >> currentMaterial.map_d;
        }
    }

    if (!currentMaterial.name.empty())
        materials.push_back(currentMaterial);

    return materials;
}

void Model::LoadTextures(std::vector<Texture>& materialTextures, std::vector<Material>& materials, Material& currentMaterial) const
{
    bool materialFound = false;

    // Load textures from material
    for (int i = 0; i < materials.size(); i++)
    {
        if (materials[i].name == currentMaterial.name)
        {
            // Diffuse
            if (!materials[i].map_Kd.empty()){
                std::string kd_texturePath = directory + '\\' + materials[i].map_Kd;
                materialTextures.push_back(Texture(device, deviceContext, std::wstring(kd_texturePath.begin(), kd_texturePath.end()), TextureType::DIFFUSE));
            }
            else{
                materialTextures.push_back(Texture(device, Color(BYTE(materials[i].Kd.x * 255), BYTE(materials[i].Kd.y * 255), BYTE(materials[i].Kd.z * 255)), TextureType::DIFFUSE));
            }

            // Specular
            if (!materials[i].map_Ks.empty()){
                std::string ks_texturePath = directory + '\\' + materials[i].map_Ks;
                materialTextures.push_back(Texture(device, deviceContext, std::wstring(ks_texturePath.begin(), ks_texturePath.end()), TextureType::SPECULAR));
                currentMaterial.map_Ks = materials[i].map_Ks;
            }
            else{
                materialTextures.push_back(Texture(device, Color(BYTE(materials[i].Ks.x * 255), BYTE(materials[i].Ks.y * 255), BYTE(materials[i].Ks.z * 255)), TextureType::SPECULAR));
            }

            // Ambient
            if (!materials[i].map_Ka.empty()){
                std::string ka_texturePath = directory + '\\' + materials[i].map_Ka;
                materialTextures.push_back(Texture(device, deviceContext, std::wstring(ka_texturePath.begin(), ka_texturePath.end()), TextureType::AMBIENT));
            }
            else{
                materialTextures.push_back(Texture(device, Color(BYTE(materials[i].Ka.x * 255), BYTE(materials[i].Ka.y * 255), BYTE(materials[i].Ka.z * 255)), TextureType::AMBIENT));
            }

            // Shininess
            if (materials[i].Ns != 0.0f){
                currentMaterial.Ns = materials[i].Ns;
            }

            // Mask
            if (!materials[i].map_d.empty()){
                std::string d_texturePath = directory + '\\' + materials[i].map_d;
                materialTextures.push_back(Texture(device, deviceContext, std::wstring(d_texturePath.begin(), d_texturePath.end()), TextureType::MASK));
                currentMaterial.map_d = materials[i].map_d;
            }

            materialFound = true;
            break;
        }
    }
    
    // Use default textures if no material is found
    if (!materialFound)
    {
        materialTextures.clear(); // Clear material textures
        // Diffuse
        Texture defaultDiffuse(device, Color(150, 150, 150), TextureType::DIFFUSE);
        materialTextures.push_back(defaultDiffuse); 
        // Specular
        Texture defaultSpecular(device, Color(255, 255, 255), TextureType::SPECULAR);
        materialTextures.push_back(defaultSpecular); 
        // Ambient
        Texture defaultAmbient(device, Color(150, 150, 150), TextureType::AMBIENT);
        materialTextures.push_back(defaultAmbient); 
        // Shininess
        currentMaterial.Ns = 1.0f; 
    }
}

bool Model::LoadModel(const std::string& filePath)
{
    directory = GetDirectoryFromPath(filePath);
    std::string objFilePath = filePath + ".obj";
    std::string mtlFilePath = filePath + ".mtl";

    // Data to pass in the mesh constructor
    std::vector<Vertex> vertices;
    std::vector<DWORD> indices;
    std::vector<Material> materials;
    std::vector<Texture> materialTextures;

    // Temp data to create vertices
    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT2> texCoords;
    std::vector<DirectX::XMFLOAT3> normals;

    std::string currentObjectName = "";  // Keep track of the current object name
    Material currentMaterial;  // Keep track of the current material

    // Initialize extremes for bounding box calculation
    DirectX::XMVECTOR minPoint = DirectX::XMVectorSet(FLT_MAX, FLT_MAX, FLT_MAX, 0);
    DirectX::XMVECTOR maxPoint = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0);

    // OBJ
    std::ifstream file(objFilePath);
    if (!file.is_open())
    {
        MessageBox(nullptr, L"Failed to open OBJ file", L"Error message", MB_OK | MB_ICONINFORMATION);
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "mtllib") // MTL file reference
        {
            std::string mtlFileName;
            iss >> mtlFileName;
            materials = LoadMTL(directory + '\\' + mtlFileName); // Store all the materials from mtl file
        }
        else if (token == "o" || token == "g") // object/group
        {
            // Create a new mesh for each object
            if (!currentObjectName.empty() && !vertices.empty())
            {
                // Load textures of the current material
                LoadTextures(materialTextures, materials, currentMaterial);

                // Create mesh
                meshes.push_back(Mesh(device, deviceContext, vertices, indices, materialTextures, currentMaterial));

                // Clear vectors for the new object
                vertices.clear();
                indices.clear();
            }

            // Set the new object name
            iss >> currentObjectName;
        }
        else if (token == "v") // vertex
        {
            DirectX::XMFLOAT3 position = {};
            iss >> position.x >> position.y >> position.z;
            // Apply rotation to the vertex position
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(DirectX::XM_PI); // Rotate 90 degrees around Y axis
            DirectX::XMFLOAT3 rotatedPosition;
            DirectX::XMStoreFloat3(&rotatedPosition, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&position), rotationMatrix));
            positions.push_back(rotatedPosition);

            // Update bounding box extremes
            DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&rotatedPosition);
            minPoint = DirectX::XMVectorMin(minPoint, posVec);
            maxPoint = DirectX::XMVectorMax(maxPoint, posVec);
        }
        else if (token == "vt") // texture coordinates
        {
            DirectX::XMFLOAT2 texCoord = {};
            iss >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        }
        else if (token == "vn") // vertex normal
        {
            DirectX::XMFLOAT3 normal = {};
            iss >> normal.x >> normal.y >> normal.z;
            // Apply rotation to the normal
            DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationY(DirectX::XM_PI); // Rotate 90 degrees around Y axis
            DirectX::XMFLOAT3 rotatedNormal;
            DirectX::XMStoreFloat3(&rotatedNormal, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&normal), rotationMatrix));
            normals.push_back(rotatedNormal);
        }
        else if (token == "usemtl") // material
        {
            materialTextures.clear();
            currentMaterial = Material();
            iss >> currentMaterial.name;
        }
        else if (token == "f") // face
        {
            for (int i = 0; i < 3; i++)
            {
                DWORD positionIndex, texCoordIndex, normalIndex;
                char slash; // Variable to store the slashes between indices
                iss >> positionIndex;

                // Check for texture coordinate and normal indices
                if (iss.peek() == '/')
                {
                    iss.ignore(); // Skip '/'
                    if (iss.peek() != '/')
                    {
                        // Texture coordinate index is present
                        iss >> texCoordIndex;
                    }
                    else
                    {
                        // Texture coordinate is missing
                        texCoordIndex = 0; // Set a default value or handle as needed
                    }
                    iss.ignore(); // Skip the second '/'
                    iss >> normalIndex;
                }
                else
                {
                    // Texture coordinate is missing
                    texCoordIndex = 0; // Set a default value or handle as needed
                    // No texture coordinate, directly read normal index
                    iss >> slash >> slash >> normalIndex;
                }

                // Create a vertex using the extracted indices
                Vertex vertex = {};
                vertex.position = positions[positionIndex - 1];
                if (texCoordIndex != 0)
                {
                    // Handle texture coordinates if available
                    vertex.texCoord.x = texCoords[texCoordIndex - 1].x;
                    vertex.texCoord.y = -texCoords[texCoordIndex - 1].y;
                }
                vertex.normal = normals[normalIndex - 1];

                vertices.push_back(vertex);
                indices.push_back(DWORD(vertices.size() - 1));
            }
        }
    }

    if (!currentObjectName.empty() && !vertices.empty())
    {
        // Load textures of the current material
        LoadTextures(materialTextures, materials, currentMaterial);

        // Create Mesh for the last object
        meshes.push_back(Mesh(device, deviceContext, vertices, indices, materialTextures, currentMaterial));
    }

    // Finalize and store the bounding box for the model
    DirectX::BoundingBox boundingBox;
    DirectX::XMStoreFloat3(&boundingBox.Center, DirectX::XMVectorScale(DirectX::XMVectorAdd(minPoint, maxPoint), 0.5f));
    DirectX::XMVECTOR extents = DirectX::XMVectorScale(DirectX::XMVectorSubtract(maxPoint, minPoint), 0.5f);
    DirectX::XMStoreFloat3(&boundingBox.Extents, extents);

    // Store this boundingBox in your Model class instance for later retrieval
    this->boundingBox = boundingBox;

    return true;
}

std::string Model::GetDirectoryFromPath(const std::string& filePath)
{
    std::filesystem::path path(filePath);

    std::string directory = path.parent_path().string();

    return directory;
}
