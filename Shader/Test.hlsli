// ビュー&射影変換行列
cbuffer ConstBufferDataViewProjection : register(b0)
{
	matrix matViewProjection;
};

// 頂点シェーダーの出力構造体
struct VSOutput
{
	float4 pos : POSITION;// 頂点座標
};

// ジオメトリシェーダーの出力構造体
struct GSOutput
{
	float4 svpos : SV_POSITION;// システム用頂点座標
	float2 uv : TEXCOORD;// uv値
};