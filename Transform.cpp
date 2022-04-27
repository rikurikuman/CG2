#include "Transform.h"

void Transform::UpdateMatrix()
{
	matrix = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMMATRIX matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(rotation.z); //Roll
	matRot *= XMMatrixRotationX(rotation.x); //Pitch
	matRot *= XMMatrixRotationY(rotation.y); //Yaw

	XMMATRIX matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	matrix *= matScale;
	matrix *= matRot;
	matrix *= matTrans;

	if (parent != nullptr) {
		matrix *= parent->matrix;
	}
}

void Transform::Transfer(TransformBuffer* target)
{
	target->matrix = this->matrix;
}

void Transform::Transfer(TransformBuffer* target, XMMATRIX matrix)
{
	target->matrix = this->matrix * matrix;
}
