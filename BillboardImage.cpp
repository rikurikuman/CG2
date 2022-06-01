#include "BillboardImage.h"

void BillboardImage::Init(Texture* texture, Vector2 size)
{
	image = Image3D(texture, size);
	image.SetParent(this);
}

void BillboardImage::Update(const ViewProjection& vp)
{
	Matrix4 mat = vp.view;
	mat = -mat;
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	transform.UpdateMatrix();
	transform.matrix *= mat;

	image.transform.UpdateMatrix();
	TransferBuffer(vp);
}

void BillboardImage::TransferBuffer(ViewProjection v) {
	image.TransferBuffer(v);
}

void BillboardImage::DrawCommands()
{
	image.DrawCommands();
}
