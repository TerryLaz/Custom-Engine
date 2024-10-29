// ELEFTERIOS LAZARIDIS
#pragma once
#include <DirectXMath.h>
#include <string>

struct Material 
{
    std::string name = ""; //Material Name
    DirectX::XMFLOAT3 Ka = { 0.0f, 0.0f, 0.0f }; //Ambient Color
    DirectX::XMFLOAT3 Kd = { 0.0f, 0.0f, 0.0f }; //Diffuse Color
    DirectX::XMFLOAT3 Ks = { 0.0f, 0.0f, 0.0f }; //Specular Color
    float Ns = 0.0f; //Specular Exponent
    float Ni = 1.0f; //Optical Density
    float d = 1.0f; //Dissolve
    int illum = 1; //Illumination
	std::string map_Ka = ""; //Ambient Texture Map
	std::string map_Kd = ""; //Diffuse Texture Map
	std::string map_Ks = ""; //Specular Texture Map
	std::string map_Ns = ""; //Specular Hightlight Map
	std::string map_d = ""; //Alpha Texture Map
	std::string map_bump = ""; //Bump Map
};