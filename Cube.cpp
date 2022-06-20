#include "Cube.h"
#include "Vector3.h"

Cube::Cube(Vector2 size)
{
	this->size = size;
	for (int i = 0; i < 6; i++) {
		faces[i].SetParent(this);
	}

	for (int i = 0; i < 4; i++) {
		faces[i].size = size;
	}
	for (int i = 4; i < 6; i++) {
		float dekaihou = max(size.x, size.y);
		faces[i].size = { dekaihou, dekaihou };
	}
}

Cube::Cube(TextureHandle texture, Vector2 size)
{
	this->size = size;
	for (int i = 0; i < 6; i++) {
		faces[i].SetParent(this);
		faces[i].texture = texture;
	}

	for (int i = 0; i < 4; i++) {
		faces[i].size = size;
	}
	for (int i = 4; i < 6; i++) {
		float dekaihou = max(size.x, size.y);
		faces[i].size = { dekaihou, dekaihou };
	}

	for (int i = 0; i < 6; i++) {
		faces[i].SetParent(this);
		faces[i].texture = texture;
		faces[i].Init();
	}

	//ŽG‚ÉƒLƒ…[ƒu‚É‚·‚é‚æ
	faces[Direction::Front].transform.position = { 0, 0, -size.x / 2.0f };
	faces[Direction::Right].transform.position = { -size.x / 2.0f, 0, 0 };
	faces[Direction::Right].transform.rotation.y = Util::AngleToRadian(90);
	faces[Direction::Back].transform.position = { 0, 0, size.x / 2.0f };
	faces[Direction::Back].transform.rotation.y = Util::AngleToRadian(180);
	faces[Direction::Left].transform.position = { size.x / 2.0f, 0, 0 };
	faces[Direction::Left].transform.rotation.y = Util::AngleToRadian(270);
	faces[Direction::Top].transform.position = { 0, size.y / 2.0f, 0 };
	faces[Direction::Top].transform.rotation = { Util::AngleToRadian(90), Util::AngleToRadian(180), 0};
	faces[Direction::Bottom].transform.position = { 0, -size.y / 2.0f, 0 };
	faces[Direction::Bottom].transform.rotation = { Util::AngleToRadian(-90), 0, 0 };
}

void Cube::SetTexture(TextureHandle texture, Direction direction)
{
	faces[direction].texture = texture;
	faces[direction].Init();
}

void Cube::SetAllTexture(TextureHandle texture)
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
