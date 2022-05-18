#pragma once
#include <string>
#include <vector>
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Model
{
public:
	VertexBuffer vertexBuff;
	IndexBuffer indexBuff;
	std::vector<Vertex> vertexs;
	std::vector<unsigned int> indices;
	std::string name;

	static Model Load(std::string filepath);
};

