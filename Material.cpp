#include "Material.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "Util.h"

using namespace std;

void Material::Transfer(MaterialBuffer* target)
{
	target->color = color;
	target->ambient = ambient;
	target->diffuse = diffuse;
	target->specular = specular;
}

Material Material::Load(string filepath, string filename)
{
    Material material;

    ifstream file;
    file.open((filepath + filename).c_str());
    if (file.fail()) {
        assert(0);
    }

    string line = "";
    while (getline(file, line)) {
        istringstream line_stream(line);

        string key;
        getline(line_stream, key, ' ');

        if (key[0] == '\t') {
            key.erase(key.begin());
        }

        if (key == "newmtl") { //マテリアル定義（お名前はここから）
            line_stream >> material.name;
        }

        if (key == "Ka") { //環境光
            line_stream >> material.ambient.x;
            line_stream >> material.ambient.y;
            line_stream >> material.ambient.z;
        }

        if (key == "Kd") { //拡散反射光
            line_stream >> material.diffuse.x;
            line_stream >> material.diffuse.y;
            line_stream >> material.diffuse.z;
        }

        if (key == "Ks") { //鏡面反射光
            line_stream >> material.specular.x;
            line_stream >> material.specular.y;
            line_stream >> material.specular.z;
        }

        if (key == "map_Kd") { //テクスチャ指定
            string texfile;
            line_stream >> texfile;
            material.texture = TextureManager::Load(filepath + texfile);
        }
    }
    return material;
}
