#include "ViewProjection.h"

void ViewProjection::UpdateMatrix()
{
	view = Matrix4::View(eye, target, up);
	projection = Matrix4::PerspectiveProjection(fov, aspect, nearclip, farclip);
	matrix = view * projection;
}

void ViewProjection::Transfer(ViewProjectionBuffer* target)
{
	target->matrix = this->matrix;
}
