// ELEFTERIOS LAZARIDIS
cbuffer CB_Camera : register(b0)
{
    float4x4 viewProj;
    float3 cameraPosition;
    float padding;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLD_POSITION;
    float4 shadowPos[4] : SHADOW_POSITION;
};

struct HS_CONTROL_POINT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLD_POSITION;
    float4 shadowPos[4] : SHADOW_POSITION;
};

struct HS_CONSTANT_DATA
{
    float EdgeTess[3] : SV_TessFactor;
    float InsideTess : SV_InsideTessFactor;
};

HS_CONSTANT_DATA PatchConstantFunction(InputPatch<VS_OUTPUT, 3> patch)
{
    HS_CONSTANT_DATA output;
    float maxDistance = 500.0f;
    float maxTessellationFactor = 16.0f;
    float3 patchCenter = (patch[0].worldPos + patch[1].worldPos + patch[2].worldPos) / 3.0;
    
    float distance = length(cameraPosition - patchCenter);
    float normalizedDistance = clamp(distance / maxDistance, 0, 1);
    float tessellationAmount = lerp(maxTessellationFactor, 1, normalizedDistance);
    
    output.EdgeTess[0] = tessellationAmount;
    output.EdgeTess[1] = tessellationAmount;
    output.EdgeTess[2] = tessellationAmount;
    output.InsideTess = tessellationAmount;
    return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
HS_CONTROL_POINT main(InputPatch<VS_OUTPUT, 3> patch, uint id : SV_OutputControlPointID)
{
    HS_CONTROL_POINT output;
    output.pos = patch[id].pos;
    output.texCoord = patch[id].texCoord;
    output.normal = patch[id].normal;
    output.worldPos = patch[id].worldPos;
    output.shadowPos = patch[id].shadowPos;
    return output;
}
