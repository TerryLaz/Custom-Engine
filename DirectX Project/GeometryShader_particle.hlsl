// ELEFTERIOS LAZARIDIS
#pragma pack_matrix( row_major )

cbuffer Camera : register(b0)
{
    float4x4 viewProj;
    float3 cameraPosition;
    float padding;
};

struct GeometryShaderOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

[maxvertexcount(6)]
void main(point float3 input[1] : POSITION, inout TriangleStream<GeometryShaderOutput> output)
{
    float3 frontVec = normalize(input[0] - cameraPosition);
    float3 defaultUpVec = float3(0, 1, 0);
    float3 rightVec = normalize(cross(defaultUpVec, frontVec));
    float3 actualUpVec = cross(frontVec, rightVec);

    float scale = 7.0f;
    rightVec *= scale;
    actualUpVec *= scale;
    
    GeometryShaderOutput toAppend;
    
    toAppend.pos = mul(float4(input[0] - rightVec + actualUpVec, 1.0f), viewProj); //Top left
    toAppend.tex = float2(0.0f, 0.0f); // Texture coordinate
    output.Append(toAppend);
    toAppend.pos = mul(float4(input[0] + rightVec - actualUpVec, 1.0f), viewProj); //Bottom right
    toAppend.tex = float2(1.0f, 1.0f); // Texture coordinate
    output.Append(toAppend);
    toAppend.pos = mul(float4(input[0] - rightVec - actualUpVec, 1.0f), viewProj); //Bottom left
    toAppend.tex = float2(0.0f, 1.0f); // Texture coordinate
    output.Append(toAppend);
    output.RestartStrip(); // Done with first triangle

    
    toAppend.pos = mul(float4(input[0] - rightVec + actualUpVec, 1.0f), viewProj); //Top left
    toAppend.tex = float2(0.0f, 0.0f); // Texture coordinate
    output.Append(toAppend);
    toAppend.pos = mul(float4(input[0] + rightVec + actualUpVec, 1.0f), viewProj); //Top right
    toAppend.tex = float2(1.0f, 0.0f); // Texture coordinate
    output.Append(toAppend);
    toAppend.pos = mul(float4(input[0] + rightVec - actualUpVec, 1.0f), viewProj); //Bottom right
    toAppend.tex = float2(1.0f, 1.0f); // Texture coordinate
    output.Append(toAppend);
}