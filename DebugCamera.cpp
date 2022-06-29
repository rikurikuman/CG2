#include "DebugCamera.h"
#include "RInput.h"

void DebugCamera::Update()
{
	if (RInput::GetKey(DIK_W)) {
		viewProjection.eye.x += -moveSpeed * cos(Util::PI / 180 * angle.x);
		viewProjection.eye.z += moveSpeed * sin(Util::PI / 180 * angle.x);
	}
	if (RInput::GetKey(DIK_S)) {
		viewProjection.eye.x += -moveSpeed * cos(Util::PI / 180 * (angle.x + 180));
		viewProjection.eye.z += moveSpeed * sin(Util::PI / 180 * (angle.x + 180));
	}
	if (RInput::GetKey(DIK_A)) {
		viewProjection.eye.x += -moveSpeed * cos(Util::PI / 180 * (angle.x - 90));
		viewProjection.eye.z += moveSpeed * sin(Util::PI / 180 * (angle.x - 90));
	}
	if (RInput::GetKey(DIK_D)) {
		viewProjection.eye.x += -moveSpeed * cos(Util::PI / 180 * (angle.x + 90));
		viewProjection.eye.z += moveSpeed * sin(Util::PI / 180 * (angle.x + 90));
	}
	if (RInput::GetKey(DIK_SPACE)) {
		viewProjection.eye.y += moveSpeed;
	}
	if (RInput::GetKey(DIK_LSHIFT)) {
		viewProjection.eye.y -= moveSpeed;
	}

	angle.x += RInput::GetMouseMove().x / sensitivity;
	angle.y -= RInput::GetMouseMove().y / sensitivity;

	if (angle.x >= 360) {
		angle.x -= 360;
	}
	if (angle.x < 0) {
		angle.x += 360;
	}

	if (angle.y >= 90) {
		angle.y = 89.9f;
	}
	if (angle.y <= -90) {
		angle.y = -89.9f;
	}

	viewProjection.target.x = viewProjection.eye.x + -100 * cos(Util::PI / 180 * angle.x) * cos(Util::PI / 180 * angle.y);
	viewProjection.target.y = viewProjection.eye.y + 100 * sin(Util::PI / 180 * angle.y);
	viewProjection.target.z = viewProjection.eye.z + 100 * sin(Util::PI / 180 * angle.x) * cos(Util::PI / 180 * angle.y);

	viewProjection.UpdateMatrix();
}
