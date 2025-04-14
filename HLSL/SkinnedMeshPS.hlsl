#include "SkinnedMesh.hlsli"

SamplerState samplerState : register(s0);
Texture2D textureMap : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = textureMap.Sample(samplerState, pin.texcoord);
    float alpha = color.a;
	
    float3 N = normalize(pin.world_normal.xyz);
    float3 L = normalize(-light_direction.xyz);
    float3 diffuse = color.rgb * max(0, dot(N, L) * 0.5f + 0.5f);
    
    return float4(diffuse, alpha) * pin.color;
}