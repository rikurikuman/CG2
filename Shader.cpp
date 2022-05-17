#include "Shader.h"

Shader::Shader(std::string filename, std::string entrypoint, std::string target)
{
	HRESULT result = D3DCompileFromFile(
		std::wstring(filename.begin(), filename.end()).c_str(), //�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), //�G���g�����A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //�f�o�b�O�p
		0,
		&shaderBlob, &errorBlob);
	succeeded = SUCCEEDED(result);
}
