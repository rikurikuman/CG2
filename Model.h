#pragma once
#include <string>
#include <vector>
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"

class Model
{
public:
	VertexBuffer vertexBuff;
	IndexBuffer indexBuff;
	std::vector<VertexPNU> vertexs;
	std::vector<unsigned int> indices;
	Material material;
	std::string name;

	static Model Load(std::string filepath, std::string filename);
};

