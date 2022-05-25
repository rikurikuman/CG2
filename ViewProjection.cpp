#include "ViewProjection.h"

void ViewProjection::UpdateMatrix()
{
	if (Util::debugBool) {
		matrix = Matrix4(
			DirectX::XMMatrixLookAtLH(
				DirectX::XMLoadFloat3(&DirectX::XMFLOAT3{ eye.x, eye.y, eye.z }),
				DirectX::XMLoadFloat3(&DirectX::XMFLOAT3{ target.x, target.y, target.z }),
				DirectX::XMLoadFloat3(&DirectX::XMFLOAT3{ up.x, up.y, up.z })
			)
		) * Matrix4::PerspectiveProjection(fov, aspect, nearclip, farclip);
	}
	else {
		matrix = Matrix4::View(eye, target, up) * Matrix4::PerspectiveProjection(fov, aspect, nearclip, farclip);
	}
	
}

void ViewProjection::Transfer(ViewProjectionBuffer* target)
{
	target->matrix = this->matrix;
}
