#include "Grid.hlsli"

VSOutput main(float4 pos : POSITION)
{
	VSOutput output; // ピクセルシェーダーに渡す値

	output.svpos = mul(matViewProjection, pos); //座標に行列を乗算
	return output;
}