#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const float NORMAL_FOV = 100.0F;
const float ZOOM_FOV = 10.0f;
const float SPRINT_FOV_MULTIPLIER = 1.2f;


class Camera {
private:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;
	//fuggoleges(pitch) es vizszintes(yaw) elfordulasa a kameranak
	float pitch;
	float yaw;

	//a camera es a physics engine erosen ossze van kotve
	glm::vec3 velocity;
	glm::vec3 previousVelocity;

public:
	//alapertekekre allitja a tagvaltozokat
	Camera();

	float GetPitch() { return pitch; }
	float GetYaw() { return yaw; }

	glm::vec3 GetPosition();

	void SetPosition(float x, float y, float z);
	void SetPosition(glm::vec3 pos);

	glm::vec3 GetFront();
	glm::vec3 GetRight();
	glm::vec3 GetUp();

	glm::vec3 GetVelocity(bool previous = false);
	void SetVelocity(float x, float y, float z, bool previous = false);
	void SetVelocity(glm::vec3 velocity, bool previous = false);

	//a parameterkent beadott elfordulast hozzaadja a terbeli orientaciojahoz a kameranak
	void Rotate(float horizontal, float vertical);

	void Translate(glm::vec3 movement);
	void Translate(float x, float y, float z);

	void CalculateVectors();

	glm::mat4 GetViewMatrix();


	//static

	//a glm konyvtarhoz megegy absztrakcios reteget hozzaad
	static glm::mat4 GetProjectionMatrix(bool isOrthographic, float fov, float aspectRatio, float nearPlane, float farPlane);
};

#endif