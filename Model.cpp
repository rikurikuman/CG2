#include "Model.h"
#include <fstream>
#include <sstream>
#include <cassert>

using namespace std;

Model Model::Load(std::string filepath)
{
    Model model;

    ifstream file;
    file.open(filepath.c_str());
    if (file.fail()) {
        assert(0);
    }

    string line = "";
    while (getline(file, line)) {
        istringstream line_stream(line);

        string key;
        getline(line_stream, key, ' ');

        if (key == "v") {
            Vector3 pos;
            line_stream >> pos.x;
            line_stream >> pos.y;
            line_stream >> pos.z;
            Vertex vertex;
            vertex.pos = pos;
            model.vertexs.emplace_back(vertex);
        }

        if (key == "f") {
            string index_string;
            while (getline(line_stream, index_string, ' ')) {
                istringstream index_stream(index_string);
                unsigned int index;
                index_stream >> index;
                model.indices.emplace_back(index - 1);
            }
        }
    }

    model.vertexBuff.Init(model.vertexs);
    model.indexBuff.Init(model.indices);
    model.name = filepath;
    return model;
}
