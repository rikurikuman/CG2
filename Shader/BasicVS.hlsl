#include "Basic.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutput output; // ピクセルシェーダーに渡す値
	
	matrix matFinal = mul(matViewProjection, matWorld);
	output.svpos = mul(matFinal, pos); //座標に行列を乗算
	output.normal = mul(matWorld, normal).xyz;
	output.uv = uv;
	return output;
}