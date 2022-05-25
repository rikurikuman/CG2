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

    string line = "";
    while (getline(file, line)) {
        istringstream line_stream(line);

        string key;
        getline(line_stream, key, ' ');

        if (key == "v") { //���_���W
            Vector3 pos;
            line_stream >> pos.x;
            line_stream >> pos.y;
            line_stream >> pos.z;

            vertPosList.emplace_back(pos.x, pos.y, pos.z);
        }

        if (key == "vt") { //���_UV
            Vector2 texcoord;
            line_stream >> texcoord.x;
            line_stream >> texcoord.y;
            vertTexcoordList.emplace_back(texcoord.x, texcoord.y);
        }

        if (key == "vn") { //���_�@��
            Vector3 normal;

            line_stream >> normal.x;
            line_stream >> normal.y;
            line_stream >> normal.z;

            vertNormalList.emplace_back(normal.x, normal.y, normal.z);
        }

        if (key == "f") { //�C���f�b�N�X
            vector<string> indexs = Util::StringSplit(line_stream.str(), " ");
            for (int i = 1; i < indexs.size(); i++) {
                vector<string> indexText = Util::StringSplit(indexs[i], "/");

                Vertex vertex;
                if (indexText.size() >= 1) {
                    int index = atoi(indexText[0].c_str()) - 1;
                    if (index >= 0 && vertPosList.size() > index) {
                        vertex.pos = vertPosList[index];
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

                model.vertexs.emplace_back(vertex);
                model.indices.emplace_back((unsigned int)model.indices.size());
            }
        }

        if (key == "mtllib") { //�}�e���A���e���v���[�g���C�u����
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
