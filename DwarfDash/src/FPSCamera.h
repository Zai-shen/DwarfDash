/*
This code is based on: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h
*/

#pragma once

#ifndef FPSCAMERA_H
#define FPSCAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f; // X
const float PITCH = 0.0f; // Y
const float SPEED = 0.1f; // camera speed
const float SENSITIVITY = 0.1f; // mouse sensitivity
const float ZOOM = 45.0f;


class FPSCamera {

public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// my declarations
	glm::mat4 perspective; // projection 
	glm::mat4 view;


	// Euler Angles
	float Yaw;
	float Pitch;

	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// Constructor with vectors
	FPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

	// Constructor with scalar values
	FPSCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	FPSCamera(double fov, double aspect, double nearC, double farC);

	~FPSCamera();

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 getViewMatrix();

	glm::vec3 getPosition();

	glm::vec3 getFront();

	glm::vec3 getUp();

	glm::vec3 getRight();

	glm::vec3 getWorldUp();

	void setPosition(glm::vec3 newPosition);

	glm::vec3 resetPosition();

	void ProcessKeyboard(Camera_Movement direction, float deltaTime);

	void ProcessMouseMovement(float xoffset, float yoffset);

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors();

	void updateViewMatrix();


};
#endif