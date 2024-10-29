// ELEFTERIOS LAZARIDIS
struct Particle
{
    float3 startPosition;
    float lifeTime;
    float3 position;
    float velocity;
    float randomSeed;
};

RWStructuredBuffer<Particle> Particles : register(u0);

[numthreads(32, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x < 32)
    {
        Particle p = Particles[DTid.x];
        
        // Generating pseudo-random seed
        float seedX = frac(sin(dot(p.position.xy, float2(12.9898, 78.233))) * 43758.5453);
        float seedY = frac(sin(dot(p.position.yz, float2(39.346, 11.135))) * 43758.5453);
        float seedZ = frac(sin(dot(p.position.xz, float2(54.2283, 19.123))) * 43758.5453);


        // Use the random seed to generate a pseudo-random direction for movement
        float3 randomDir = normalize(float3(2.0 * sin(seedX * 3.14159f * 2.0f),
                                            abs(cos(seedY * 3.14159f * 2.0f)),
                                            2.0 * cos(seedZ * 3.14159f * 2.0f)));
        
        p.position += p.velocity * randomDir;
        p.lifeTime -= 0.01f; // Decrease life time
        if (p.lifeTime < 0.0f) // Reset particle after its life time is over
        {
            p.position = p.startPosition;
            p.lifeTime = p.randomSeed;
        }

        Particles[DTid.x] = p;
    }
}