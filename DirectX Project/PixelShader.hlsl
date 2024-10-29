// ELEFTERIOS LAZARIDIS
cbuffer CB_PS_Light : register(b0)
{
    float3 ambientLightColor;
    float ambientLightStrength;
    
    float3 directionalLightColor;
    float directionalLightStrength;
    
    float3 spotlightColor;
    float spotlightStrength;
    float4 lightsPosition[4];
    float4 lightsDir[4];
    
    float spotFalloff;
    float attenuation_a;
    float attenuation_b;
    float attenuation_c;
}

cbuffer CB_Camera : register(b1)
{
    float4x4 viewProj;
    float3 cameraPosition;
    float padding;
};

cbuffer CB_PS_Material : register(b2)
{
    float specularExponent;
    int hasSpecMap;
    int hasMask;
    int isReflective;
}

Texture2D diffuseTexture : TEXTURE : register(t0);
Texture2D specularTexture : TEXTURE : register(t1);
Texture2D maskTexture : TEXTURE : register(t2);
Texture2DArray<float> shadowMaps : TEXTURE : register(t3);
TextureCube environmentMap : register(t4);

SamplerState objSampler : SAMPLER : register(s0);
SamplerComparisonState shadowSampler : SAMPLER : register(s1);
SamplerState environmentSampler : SAMPLER : register(s2);

struct PS_INPUT
{
    float4 pos : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLD_POSITION;
    float4 shadowPos[4] : SHADOW_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // Sample textures
    float4 diffuseSample = diffuseTexture.Sample(objSampler, input.texCoord);
    float4 specularSample = specularTexture.Sample(objSampler, input.texCoord);
    
    // Handle transperancy
    float alphaValue = 1.0f;
    if (hasMask)
    {
        clip(diffuseSample.a < 0.1f ? -1 : 1);
        alphaValue = diffuseSample.a;
    }
    
    // Initialize lighting contributions
    float3 ambientLight = diffuseSample.rgb * ambientLightColor * ambientLightStrength;
    float3 directionalLight = 0.0f;
    float3 spotLight = 0.0f;
    float3 diffuseLight = 0.0f;
    float3 specularLight = 0.0f;
    float shadowFactor = 1.0;
    float3 viewDir = normalize(cameraPosition - input.worldPos);
    float3 reflectVec = reflect(-viewDir, input.normal);
    
    for (int i = 0; i < 4; i++)
    {
        float3 lightDir = normalize(lightsDir[i].xyz);
        float3 vectorToLight;
        float attenuation = 1.0f;
        float spotLightFactor = 1.0f;

        // Calculate vector to light differently based on light type
        if (i < 3) // For spot lights
        {
            vectorToLight = normalize(lightsPosition[i].xyz - input.worldPos);
            float distanceToLight = distance(lightsPosition[i].xyz, input.worldPos);
            attenuation = 1.0f / (attenuation_a + attenuation_b * distanceToLight + attenuation_c * pow(distanceToLight, 2));
            spotLightFactor = dot(-vectorToLight, lightDir);
        }
        else // For the directional light
        {
            vectorToLight = -lightDir;
        }
 
        // Check if the light contributes (for spotlights) or if it's the directional light
        if (i < 3 && spotLightFactor > spotFalloff || i == 3)
        {
            float smoothStepFactor = 1.0f;
            if (i < 3)
            {
                smoothStepFactor = smoothstep(spotFalloff, 1.0f, spotLightFactor);
                // Diffuse light for spotlights
                float diffuseIntensity = max(dot(vectorToLight, input.normal), 0.0f);
                spotLight += diffuseSample.rgb * diffuseIntensity * attenuation * spotlightColor * spotlightStrength * smoothStepFactor;
            }
            else // For the directional light
            {
                // Diffuse light for directional light
                float diffuseIntensity = max(dot(vectorToLight, input.normal), 0.0f);
                directionalLight += diffuseSample.rgb * diffuseIntensity * attenuation * directionalLightColor * directionalLightStrength;
            }
    
            // Specular light calculation
            float specularIntensity = 0.0f;
            float3 reflectDir = reflect(-vectorToLight, input.normal);
            if (hasSpecMap == 1)
            {
                float specularPower = pow(2.0f, specularSample.a * 13.0f);
                specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0f), specularPower);
            }
            else
            {
                specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0f), specularExponent);
            }

            // Apply specular intensity to spotlight and directional light separately
            if (i < 3) // For spot lights
            {
                spotLight += specularSample.rgb * specularIntensity * attenuation * spotlightColor * spotlightStrength * smoothStepFactor;
            }
            else // For the directional light
            {
                directionalLight += specularSample.rgb * specularIntensity * attenuation * directionalLightColor * directionalLightStrength;
            }

            // Shadow mapping
            input.shadowPos[i].xyz /= input.shadowPos[i].w; //re-homogenize position after interpolation
            input.shadowPos[i].x = input.shadowPos[i].x / 2 + 0.5; //transform clip space coords x to texture space coords (-1:1 to 0:1)
            input.shadowPos[i].y = input.shadowPos[i].y / -2 + 0.5; //transform clip space coords y to texture space coords (-1:1 to 0:1)
            float shadowDepth = shadowMaps.SampleCmpLevelZero(shadowSampler, float3(input.shadowPos[i].xy, i), input.shadowPos[i].z).r; //perform shadow map comparison
            shadowFactor *= (input.shadowPos[i].z > shadowDepth) ? 0.5f : 1.0f;
        }
    }
    
    // Combine lighting contributions
    float3 finalColor = saturate((ambientLight + directionalLight + spotLight) * shadowFactor);

    // Cube mapping    
    if (isReflective == 1)
    {
        float4 envMapColor = environmentMap.Sample(environmentSampler, reflectVec);
        float reflectivity = 1.0; // Adjust reflectivity as needed, 1.0 for full reflection, 0.0 for no reflection
        float3 reflectedColor = lerp(finalColor, envMapColor.rgb, reflectivity);
        //Combine lighting contributions with reflection
        float3 finalColorWithReflection = saturate(reflectedColor.rgb);
        return float4(finalColorWithReflection, 1.0f);
    }
    else
    {
        return float4(finalColor, alphaValue);
    }
    
}