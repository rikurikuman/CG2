// マテリアル
cbuffer ConstBufferDataMaterial : register(b0)
{
	float4 color; // 色(RGBA)
};

// ワールド変換行列
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix matWorld;
};

// ビュー&射影変換行列
cbuffer ConstBufferDataViewProjection : register(b2)
{
	matrix matViewProjection;
};

// 頂点シェーダーの出力構造体
struct VSOutput
{
	float4 svpos : SV_POSITION;// システム用頂点座標
	float3 normal : NORMAL; //法線ベクトル
	float2 uv : TEXCOORD;// uv値
};