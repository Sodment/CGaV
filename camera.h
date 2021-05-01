#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

typedef struct cameraMovementDirection
{
	bool Forward;
	bool Backward;
	bool Left;
	bool Right;
	bool Up;
	bool Down;
}CameraMovementDirection;

extern CameraMovementDirection direction;

class Camera
{

public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	bool movement;
	bool mouse_movement;
	bool zooming;

	Camera(bool mov, bool mos_mov, bool zoom);
	Camera();

	glm::mat4 GetViewMatrix();
	glm::mat4 GetPerspectiveMatrix(float aspect, float near_plane, float far_plane);

	void ProcessMouseScroll(float yoffset);
	void ProcessMouseMovement(float xoffset, float yoffset);
	void ProcessKeyboard(float deltaTime);

	void updateCamera();

private:
	float Yaw;
	float Pitch;
	float MovmentSpeed;
	float MouseSensitivity;
	float Zoom;
};
#endif

