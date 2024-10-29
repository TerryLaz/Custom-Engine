// ELEFTERIOS LAZARIDIS
struct Particle
{
    float3 startPosition;
    float lifeTime;
    float3 position;
    float velocity;
    float randomSeed;
};

StructuredBuffer<Particle> Particles : register(t0);

float3 main(uint vertexID : SV_VertexID) : POSITION
{
    return Particles[vertexID].position;
}