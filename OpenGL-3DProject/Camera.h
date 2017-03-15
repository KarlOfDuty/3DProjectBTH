#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SFML\Window.hpp>
#include "Model.h"
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
	int RESOLUTION_WIDTH;
	int RESOLUTION_HEIGHT;
	bool testIntersection(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 aabb_min, glm::vec3 aabb_max, glm::mat4 ModelMatrix, float& intersection_distance);
public:
	Camera();
	~Camera();
	glm::mat4 Update(float deltaTime, sf::Window &window);
	glm::vec3 getCameraPos();
	int mousePicking(sf::Window &window, glm::mat4 &projectionMatrix, glm::mat4 &viewMatrix, std::vector<Model> &allModels);
};

