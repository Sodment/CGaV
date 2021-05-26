#include "camera.h"

CameraMovementDirection direction = { false, false, false, false, false, false, };

Camera::Camera(bool mov, bool mos_mov, bool zoom)
{
	Position = glm::vec3(0.0f, 0.0f, -3.0f);
	WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	Yaw = -90.0f;
	Pitch = 0.0f;
	MovmentSpeed = 5.0f;
	MouseSensitivity = 0.1f;
	Zoom = 45.0f;
	movement = mov;
	mouse_movement = mos_mov;
	zooming = zoom;
	updateCamera();

}

Camera::Camera()
{
	Position = glm::vec3(0.0f, 0.0f, 3.0f);
	WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	Yaw = -90.0f;
	Pitch = 0.0f;
	MovmentSpeed = 5.0f;
	MouseSensitivity = 0.1f;
	Zoom = 45.0f;
	movement = true;
	mouse_movement = true;
	zooming = true;
	updateCamera();

}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::GetPerspectiveMatrix(float aspect, float near_plane, float far_plane)
{
	return glm::perspective(glm::radians(Zoom), aspect, near_plane, far_plane);
}

void Camera::ProcessMouseScroll(float yoffset)
{
	if (zooming)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw = glm::mod(Yaw + xoffset, 360.0f);
	Pitch += yoffset;

	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	updateCamera();
}

void Camera::ProcessKeyboard(float deltaTime)
{
	float velocity = MovmentSpeed * deltaTime;
	if (direction.Forward) Position += velocity * Front;
	if (direction.Backward) Position -= velocity * Front;
	if (direction.Left) Position -= velocity * Right;
	if (direction.Right) Position += velocity * Right;
	if (direction.Up) Position += velocity * glm::vec3(0.0f, 1.0f, 0.0f);
	if (direction.Down) Position -= velocity * glm::vec3(0.0f, 1.0f, 0.0f);
}


void Camera::updateCamera()
{
	glm::vec3 currentFront;
	currentFront.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	currentFront.y = sin(glm::radians(Pitch));
	currentFront.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(currentFront);
	//printf("Camera Pos: %f %f %f\n", this->Position[0], this->Position[1], this->Position[2]);

	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}
