#pragma once
#include <d3dcompiler.h>
#include <wrl.h>
#include <string>
#include <unordered_map>

#pragma comment(lib, "d3dcompiler.lib")

class Shader
{
public:
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	bool succeeded = false;

	Shader() {}
	Shader(std::string filename, std::string entrypoint, std::string target);

	static void Register(std::string id, Shader shader);
	static Shader GetRegistered(std::string id);

private:
	class ShaderRegister {
	public:
		std::unordered_map<std::string, Shader> shaderRegister;
		static ShaderRegister* GetInstance() {
			static ShaderRegister reg;
			return &reg;
		}
	};
};

