#include "Model.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "Util.h"

using namespace std;

Model Model::Load(string filepath, string filename)
{
    Model model;

    ifstream file;
    file.open((filepath + filename).c_str());
    if (file.fail()) {
        assert(0);
    }

    vector<Vector3> vertPosList;
    vector<Vector2> vertTexcoordList;
    vector<Vector3> vertNormalList;

    int cou = 0;

    string line = "";
    while (getline(file, line)) {
        istringstream line_stream(line);

        string key;
        getline(line_stream, key, ' ');

        if (key == "#") {
            continue;
        }

        if (key == "v") { //頂点座標
            Vector3 pos;
            line_stream >> pos.x;
            line_stream >> pos.y;
            line_stream >> pos.z;

            vertPosList.emplace_back(pos.x, pos.y, pos.z);
        }

        if (key == "vt") { //頂点UV
            Vector2 texcoord;
            line_stream >> texcoord.x;
            line_stream >> texcoord.y;
            vertTexcoordList.emplace_back(texcoord.x, 1.0f - texcoord.y);
        }

        if (key == "vn") { //頂点法線
            Vector3 normal;

            line_stream >> normal.x;
            line_stream >> normal.y;
            line_stream >> normal.z;

            vertNormalList.emplace_back(normal.x, normal.y, normal.z);
        }

        if (key == "f") { //インデックス
            vector<string> indexs = Util::StringSplit(line_stream.str(), " ");

            vector<VertexPNU> _vertices;
            for (int i = 1; i < indexs.size(); i++) {
                vector<string> indexText = Util::StringSplit(indexs[i], "/");

                bool ok = false;
                VertexPNU vertex;
                if (indexText.size() >= 1) {
                    int index = atoi(indexText[0].c_str()) - 1;
                    if (index >= 0 && vertPosList.size() > index) {
                        vertex.pos = vertPosList[index];
                        ok = true;
                    }
                }
                if (indexText.size() >= 2) {
                    int index = atoi(indexText[1].c_str()) - 1;
                    if (index >= 0 && vertTexcoordList.size() > index) {
                        vertex.uv = vertTexcoordList[index];
                    }
                }
                if (indexText.size() >= 3) {
                    int index = atoi(indexText[2].c_str()) - 1;
                    if (index >= 0 && vertNormalList.size() > index) {
                        vertex.normal = vertNormalList[index];
                    }
                }

                if (ok) {
                    _vertices.emplace_back(vertex);
                } 
            }

            if (_vertices.size() >= 3) {
                for (int i = 0; i < _vertices.size() - 2; i++) {
                    int indexB = 1 + i;
                    int indexC = 2 + i;

                    //めちゃくちゃ重いから一回廃止
                    /*size_t a = Util::IndexOf(model.vertexs, _vertices[0]);
                    size_t b = Util::IndexOf(model.vertexs, _vertices[indexB]);
                    size_t c = Util::IndexOf(model.vertexs, _vertices[indexC]);*/

                    size_t a = -1;
                    size_t b = -1;
                    size_t c = -1;

                    if (a == -1) {
                        model.vertexs.emplace_back(_vertices[0]);
                        a = model.vertexs.size() - 1;
                    }
                    if (b == -1) {
                        model.vertexs.emplace_back(_vertices[indexB]);
                        b = model.vertexs.size() - 1;
                    }
                    if (c == -1) {
                        model.vertexs.emplace_back(_vertices[indexC]);
                        c = model.vertexs.size() - 1;
                    }

                    model.indices.emplace_back((UINT)a);
                    model.indices.emplace_back((UINT)b);
                    model.indices.emplace_back((UINT)c);
                }
            }
        }

        if (key == "mtllib") { //マテリアルテンプレートライブラリ
            string mfilename;
            line_stream >> mfilename;
            model.material = Material::Load(filepath, mfilename);
        }
    }

    model.vertexBuff.Init(model.vertexs);
    model.indexBuff.Init(model.indices);
    model.name = filepath + filename;
    return model;
}
