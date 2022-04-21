// �}�e���A��
cbuffer ConstBufferDataMaterial : register(b0)
{
	// �F(RGBA)
	float4 color;
};

// �ϊ��s��
cbuffer ConstBufferDataTransform : register(b1)
{
	matrix mat; //3D�ϊ��s��
};

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
	// �V�X�e���p���_���W
	float4 svpos : SV_POSITION;
	// uv�l
	float2 uv : TEXCOORD;
};