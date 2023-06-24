#include "camera.h"

Camera::Camera()
{
	cameraPos = glm::vec3(0.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraRight = glm::vec3(0.0f);
	cameraUp = glm::vec3(0.0f);

	velocity = glm::vec3(0.0f);
	previousVelocity = glm::vec3(0.0f);

	pitch = 0.0f;
	yaw = 0.0f;
}

glm::vec3 Camera::GetPosition()
{
	return cameraPos;
}

void Camera::SetPosition(float x, float y, float z)
{
	cameraPos.x = x;
	cameraPos.y = y;
	cameraPos.z = z;
}

void Camera::SetPosition(glm::vec3 pos)
{
	cameraPos = pos;
}

glm::vec3 Camera::GetFront()
{
	return cameraFront;
}

glm::vec3 Camera::GetRight()
{
	return cameraRight;
}

glm::vec3 Camera::GetUp()
{
	return cameraUp;
}

glm::vec3 Camera::GetVelocity(bool previous)
{
	if (previous)
		return this->previousVelocity;
	else
		return this->velocity;
}

void Camera::SetVelocity(float x, float y, float z, bool previous)
{
	if (previous)
	{
		previousVelocity.x = x;
		previousVelocity.y = y;
		previousVelocity.z = z;
	}
	else
	{
		velocity.x = x;
		velocity.y = y;
		velocity.z = z;
	}
}

void Camera::SetVelocity(glm::vec3 velocity, bool previous)
{
	if (previous)
	{
		previousVelocity.x = velocity.x;
		previousVelocity.y = velocity.y;
		previousVelocity.z = velocity.z;
	}
	else
	{
		this->velocity.x = velocity.x;
		this->velocity.y = velocity.y;
		this->velocity.z = velocity.z;
	}
}

void Camera::Rotate(float horizontal, float vertical)
{
	horizontal = glm::radians(horizontal);
	vertical = glm::radians(vertical);

	yaw += horizontal;
	pitch += vertical;
	if (pitch < glm::radians(-88.0f))
		pitch = glm::radians(-88.0f);
	else if (pitch > glm::radians(88.0f))
		pitch = glm::radians(88.0f);
}

void Camera::Translate(glm::vec3 movement)
{
	cameraPos.x += movement.x;
	cameraPos.y += movement.y;
	cameraPos.z += movement.z;
}

void Camera::Translate(float x, float y, float z)
{
	cameraPos.x += x;
	cameraPos.y += y;
	cameraPos.z += z;
}

void Camera::CalculateVectors()
{
	cameraFront.y = sin(pitch);
	cameraFront.z = cos(yaw) * cos(pitch);
	cameraFront.x = sin(yaw) * cos(pitch);

	cameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraFront));
	cameraUp = glm::cross(cameraFront, cameraRight);
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::GetProjectionMatrix(bool isOrthographic, float fov, float aspectRatio, float nearPlane, float farPlane)
{
	if (isOrthographic)
		return glm::mat4(1.0f);//nem mukszik mert nemtom hogy kell hasznalni a glm::ortho-t xd
	else
		return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}