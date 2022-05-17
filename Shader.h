#pragma once
#include <d3dcompiler.h>
#include <wrl.h>
#include <string>

#pragma comment(lib, "d3dcompiler.lib")

class Shader
{
public:
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	bool succeeded = false;

	Shader() {}
	Shader(std::string filename, std::string entrypoint, std::string target);
};

