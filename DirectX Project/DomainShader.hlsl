// ELEFTERIOS LAZARIDIS
#pragma pack_matrix( row_major )

cbuffer CB_Camera : register(b0)
{
    float4x4 viewProj;
    float3 cameraPosition;
    float padding;
};

struct HS_CONTROL_POINT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLD_POSITION;
    float4 shadowPos[4] : SHADOW_POSITION;
};

struct DS_OUTPUT
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

[domain("tri")]
DS_OUTPUT main(HS_CONSTANT_DATA input, float3 barycentricCoord : SV_DomainLocation, const OutputPatch<HS_CONTROL_POINT, 3> patch)
{
    DS_OUTPUT output;

    // Calculate the plane equations for each control point
    float4 plane[3];
    float3 projectedPos[3];
    for (int i = 0; i < 3; i++)
    {
        // Plane equation from point and normal: Ax + By + Cz + D = 0, where D = -dot(normal, point)
        plane[i].xyz = patch[i].normal; // A, B, C components are the normal
        plane[i].w = -dot(patch[i].normal, patch[i].worldPos); // D component
        
        // Project the tessellated point onto each plane
        float3 position = barycentricCoord.x * patch[0].worldPos + barycentricCoord.y * patch[1].worldPos + barycentricCoord.z * patch[2].worldPos;
        float d = dot(plane[i].xyz, position) + plane[i].w;
        projectedPos[i] = position - d * plane[i].xyz; // Project point onto the plane
    }

    // Interpolate the projected positions using barycentric coordinates
    output.worldPos = barycentricCoord.x * projectedPos[0] + barycentricCoord.y * projectedPos[1] + barycentricCoord.z * projectedPos[2];
    output.pos = mul(float4(output.worldPos, 1.0), viewProj); // Transform to clip space
    output.texCoord = barycentricCoord.x * patch[0].texCoord + barycentricCoord.y * patch[1].texCoord + barycentricCoord.z * patch[2].texCoord;
    output.normal = normalize(barycentricCoord.x * patch[0].normal + barycentricCoord.y * patch[1].normal + barycentricCoord.z * patch[2].normal);\
    for (int j = 0; j < 4; j++)
    {
        output.shadowPos[j] = barycentricCoord.x * patch[0].shadowPos[j] + barycentricCoord.y * patch[1].shadowPos[j] + barycentricCoord.z * patch[2].shadowPos[j];
    }

    return output;
}

