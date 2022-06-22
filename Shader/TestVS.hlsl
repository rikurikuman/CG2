#include "Test.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l

	matrix matFinal = mul(matViewProjection, matWorld);
	output.svpos = mul(matFinal, pos); //���W�ɍs�����Z
	output.normal = mul(matWorld, normal).xyz;
	output.uv = uv;
	return output;
}