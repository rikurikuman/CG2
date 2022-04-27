#include "ViewProjection.h"

using namespace DirectX;

void ViewProjection::UpdateMatrix()
{
	matrix = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up))
		* XMMatrixPerspectiveFovLH(fov, aspect, nearclip, farclip);
}

void ViewProjection::Transfer(ViewProjectionBuffer* target)
{
	target->matrix = this->matrix;
}
