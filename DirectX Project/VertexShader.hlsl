// ELEFTERIOS LAZARIDIS
#pragma pack_matrix( row_major )

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 lightViewProjection[4];
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLD_POSITION;
    float4 shadowPos[4] : SHADOW_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(float4(input.pos, 1.0f), wvpMatrix);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(float4(input.normal, 0.0f), worldMatrix));
    output.worldPos = mul(float4(input.pos, 1.0f), worldMatrix);
    for (int i = 0; i < 3; i++)
    {
        output.shadowPos[i] = mul(float4(input.pos, 1.0f), mul(worldMatrix, lightViewProjection[i]));
    }
    output.shadowPos[3] = mul(float4(input.pos, 1.0f), lightViewProjection[3]);
    return output;
}