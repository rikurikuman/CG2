#include "Cube.h"
#include "Vector3D.h"

Cube::Cube(XMFLOAT2 size)
{
	this->size = size;
	for (int i = 0; i < 6; i++) {
		faces[i].SetParent(this);
	}
}

Cube::Cube(Texture* texture, XMFLOAT2 size)
{
	this->size = size;
	for (int i = 0; i < 6; i++) {
		faces[i].SetParent(this);
		faces[i].texture = texture;
		faces[i].Init();
	}

	//ŽG‚ÉƒLƒ…[ƒu‚É‚·‚é‚æ
	faces[Direction::Front].transform.position = { 0, 0, -size.x / 2.0f };
	faces[Direction::Right].transform.position = { -size.x / 2.0f, 0, 0 };
	faces[Direction::Right].transform.rotation.y = XMConvertToRadians(90);
	faces[Direction::Back].transform.position = { 0, 0, size.x / 2.0f };
	faces[Direction::Back].transform.rotation.y = XMConvertToRadians(180);
	faces[Direction::Left].transform.position = { size.x / 2.0f, 0, 0 };
	faces[Direction::Left].transform.rotation.y = XMConvertToRadians(270);
	faces[Direction::Top].transform.position = { 0, size.y / 2.0f, 0 };
	faces[Direction::Top].transform.rotation = { XMConvertToRadians(90), XMConvertToRadians(180), 0};
	faces[Direction::Bottom].transform.position = { 0, -size.y / 2.0f, 0 };
	faces[Direction::Bottom].transform.rotation = { XMConvertToRadians(-90), 0, 0 };
}

void Cube::SetTexture(Texture* texture, Direction direction)
{
	faces[direction].texture = texture;
	faces[direction].Init();
}

void Cube::SetAllTexture(Texture* texture)
{
	for (int i = 0; i < 6; i++) {
		faces[i].texture = texture;
		faces[i].Init();
	}
}

void Cube::UpdateFaces()
{
	for (int i = 0; i < 6; i++) {
		faces[i].transform.UpdateMatrix();
	}
}

void Cube::TransferBuffer(ViewProjection viewprojection)
{
	for (int i = 0; i < 6; i++) {
		faces[i].TransferBuffer(viewprojection);
	}
}

void Cube::DrawCommands()
{
	for (int i = 0; i < 6; i++) {
		faces[i].DrawCommands();
	}
}
