#include "FPSCamera.h"

/*
// Constructor with vectors
//FPSCamera::FPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
//	: Front(glm::vec3(0.0f, 0.0f, -1.0f)){
//	Position = position; // a vector in world space that points to the camera's position
//	WorldUp = up;
//	Yaw = yaw;
//	Pitch = pitch;
//
//	updateCameraVectors();
//


// Constructor with scalar values
FPSCamera::FPSCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: Front(glm::vec3(0.0f, 0.0f, 100.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;

	updateCameraVectors();
}
*/

FPSCamera::FPSCamera(double fov, double aspect, double nearC, double farC) {

	Position = glm::vec3(0.0f, 0.0f, 10.0f);
	WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	perspective = glm::perspective(glm::radians(fov), aspect, nearC, farC);

	MouseSensitivity = SENSITIVITY;
	MovementSpeed = SPEED;
	Yaw = YAW;
	Pitch = PITCH;
	Zoom = ZOOM;
}

FPSCamera::~FPSCamera() {

}


// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 FPSCamera::getViewMatrix() {
	return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 FPSCamera::getProjectionMatrix() {
	return perspective;
}

glm::mat4 FPSCamera::getViewProjectionMatrix() {
	updateViewMatrix(); // make sure the view matrix is updated
	return perspective * view;
}

glm::vec3 FPSCamera::getPosition() {
	return this->Position;
}

glm::vec3 FPSCamera::getFront() {
	return this->Front;
}

glm::vec3 FPSCamera::getUp() {
	return this->Up;
}

glm::vec3 FPSCamera::getRight() {
	return this->Right;
}

glm::vec3 FPSCamera::getWorldUp() {
	return this->WorldUp;
}

void FPSCamera::setPosition(glm::vec3 newPosition) {
	Position = newPosition;
}

glm::vec3 FPSCamera::resetPosition() {
	return Position = glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 FPSCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {

	float velocity = MovementSpeed * deltaTime;

	if (direction == FORWARD) {
		Position += Front * velocity;
		return Front * velocity;
	}

	if (direction == BACKWARD) {
		Position -= Front * velocity;
		return -Front * velocity;
	}

	if (direction == LEFT) {
		Position -= Right * velocity;
		return -Right * velocity;
	}

	if (direction == RIGHT) {
		Position += Right * velocity;
		return Right * velocity;
	}


	// make sure the user stays at the ground level
	Position.y = 0.0f; // <-- this one-liner keeps the user at the ground level (xz plane)

}

void FPSCamera::ProcessMouseMovement(float xoffset, float yoffset) {
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	//Yaw += xoffset;
	Yaw = glm::mod(Yaw + (float)xoffset, 360.0f);
	Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (Pitch > 89.0f) {
		Pitch = 89.0f;
	}
	if (Pitch < -89.0f) {
		Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void FPSCamera::updateCameraVectors(){
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

	Front = glm::normalize(front);

	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}


void FPSCamera::updateViewMatrix() {
	view = glm::lookAt(Position, Position + Front, Up);
}


