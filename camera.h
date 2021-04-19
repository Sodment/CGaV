#pragma once
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

struct CAMERA_MOVEMENT_DIRECTION
{
	bool Forward;
	bool Backward;
	bool Left;
	bool Right;
	bool Up;
	bool Down;
} direction = { false, false, false, false, false, false };

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;
	float MovmentSpeed;
	float MouseSensitivity;
	float Zoom;

	bool movement;
	bool mouse_movement;
	bool zooming;


	Camera(bool mov, bool mos_mov, bool zoom)
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

	Camera()
	{
		Position = glm::vec3(0.0f, 0.0f, -3.0f);
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

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	glm::mat4 GetPerspectiveMatrix(float aspect, float near_plane, float far_plane)
	{
		return glm::perspective(glm::radians(Zoom), aspect, near_plane, far_plane);
	}

	void ProcessMouseScroll(float yoffset)
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

	void ProcessMouseMovement(float xoffset, float yoffset)
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



	void ProcessKeyboard(float deltaTime)
	{
		float velocity = MovmentSpeed * deltaTime;
		if (direction.Forward) Position += velocity * Front;
		if (direction.Backward) Position -= velocity * Front;
		if (direction.Left) Position -= velocity * Right;
		if (direction.Right) Position += velocity * Right;
		if (direction.Up) Position += velocity * glm::vec3(0.0f, 1.0f, 0.0f);
		if (direction.Down) Position -= velocity * glm::vec3(0.0f, 1.0f, 0.0f);
	}


	void updateCamera()
	{
		glm::vec3 currentFront;
		currentFront.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		currentFront.y = sin(glm::radians(Pitch));
		currentFront.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(currentFront);


		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

