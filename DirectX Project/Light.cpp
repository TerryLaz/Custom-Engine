// ELEFTERIOS LAZARIDIS
#include "Light.h"

bool Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,const std::string& fileName, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_Material>& cb_ps_material)
{
    if (!model.Initialize(fileName, device, deviceContext, cb_vs_vertexshader, cb_ps_material))
        return false;

    SetPosition(0.0f, 0.0f, 0.0f);
    SetRotation(0.0f, 0.0f, 0.0f);
    UpdateMatrix();

    return true;
}
