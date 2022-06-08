#include "Basic.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	float3 shade_color;

	float3 light = normalize(float3(1, -1, 1)); //右下奥向きのライト
	float diffuse = saturate(dot(-light, input.normal)); //拡散反射光を[0, 1]の範囲にClampする

	texcolor = texcolor * m_color;
	shade_color = m_ambient;
	shade_color += m_diffuse * diffuse;

	return float4(texcolor.rgb * shade_color, texcolor.a);
}