#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SFML\Window.hpp>
#include <iostream>
class Camera
{
private:
	bool firstMouse;
	int oldMouseX;
	int oldMouseY;
	float mouseSensitivity;
	float cameraSpeed;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	float cameraYaw;
	float cameraPitch;
public:
	Camera();
	~Camera();
	glm::mat4 Update(float deltaTime);
};

