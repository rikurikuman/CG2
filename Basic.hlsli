// �}�e���A��
cbuffer ConstBufferDataMaterial : register(b0)
{
	float4 color; // �F(RGBA)
};

// ���[���h�ϊ��s��
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix matWorld;
};

// �r���[&�ˉe�ϊ��s��
cbuffer ConstBufferDataViewProjection : register(b2)
{
	matrix matViewProjection;
};

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
	float4 svpos : SV_POSITION;// �V�X�e���p���_���W
	float3 normal : NORMAL; //�@���x�N�g��
	float2 uv : TEXCOORD;// uv�l
};