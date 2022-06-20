#include "BillboardImage.h"

void BillboardImage::Init(TextureHandle texture, Vector2 size)
{
	image = Image3D(texture, size);
	image.SetParent(this);
}

void BillboardImage::Update(const ViewProjection& vp)
{
	Matrix4 mat = vp.view;
	mat = -mat;
	if (billboardY) {
		Vector3 yVec = Vector3( 0, 1, 0 ).GetNormalize();
		Vector3 xVec = Vector3(mat[0][0], mat[0][1], mat[0][2]).GetNormalize();
		Vector3 zVec = xVec.Cross(yVec).GetNormalize();

		mat[1][0] = yVec.x;
		mat[1][1] = yVec.y;
		mat[1][2] = yVec.z;
		mat[2][0] = zVec.x;
		mat[2][1] = zVec.y;
		mat[2][2] = zVec.z;
	}
	
	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;

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
