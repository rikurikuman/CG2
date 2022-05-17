#include "ViewProjection.h"

void ViewProjection::UpdateMatrix()
{
	matrix = Matrix4::View(eye, target, up) * Matrix4::PerspectiveProjection(fov, aspect, nearclip, farclip);
}

void ViewProjection::Transfer(ViewProjectionBuffer* target)
{
	target->matrix = this->matrix;
}
