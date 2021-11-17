#include "Camera.h"
#include "Window.h"
#include <algorithm>

Camera::Camera(void) {
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;
}

Camera::Camera(float pitch, float yaw, Vector3 position) {
	this->pitch = pitch;
	this->yaw = yaw;
	this->position = position;
	roll = 0.0f;
}

Camera::Camera(float pitch, float yaw, float roll, Vector3 position) {
	this->pitch = pitch;
	this->yaw = yaw;
	this->position = position;
	this->roll = roll;
}

void Camera::UpdateCamera(float dt /*= 1.0f*/)
{

	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	float speed = 100.0f * dt;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * speed;

	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * speed;

	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * speed;

	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y += speed;

	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y -= speed;
	}
}

// Inverse of matrix created from camera's position and rotation
Matrix4 Camera::BuildViewMatrix()
{
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
}

