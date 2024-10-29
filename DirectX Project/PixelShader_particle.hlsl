// ELEFTERIOS LAZARIDIS
Texture2D ParticleTex : register(t0);
SamplerState TexSampler : register(s0);

struct PSInput
{
    float4 Pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    return ParticleTex.Sample(TexSampler, input.tex);
}
