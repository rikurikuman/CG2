#include "Transform.h"
#include "Util.h"

void Transform::UpdateMatrix()
{
	matrix = Matrix4();

	Matrix4 matScaleM = Matrix4::Scaling(scale.x, scale.y, scale.z);
	Matrix4 matRotM = Matrix4::RotationZXY(rotation.x, rotation.y, rotation.z);
	Matrix4 matTransM = Matrix4::Translation(position.x, position.y, position.z);

	matrix = matScaleM * matRotM * matTransM;

	if (parent != nullptr) {
		matrix *= parent->matrix;
	}
}

void Transform::Transfer(TransformBuffer* target)
{
	target->matrix = this->matrix;
}

void Transform::Transfer(TransformBuffer* target, Matrix4 matrix)
{
	target->matrix = this->matrix * matrix;
}
