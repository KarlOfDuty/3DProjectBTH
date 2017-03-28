#include "Camera.h"
Camera::Camera()
{
	this->firstMouse = true;
	this->oldMouseX = RESOLUTION_WIDTH / 2;
	this->oldMouseY = RESOLUTION_HEIGHT / 2;
	this->mouseSensitivity = 0.05f;
	this->cameraSpeed = 0.05f;
	this->cameraPos = glm::vec3(0.0f, 0.0f, 2.0f);
	this->cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->cameraYaw = -90.0f;
	this->cameraPitch = 0.0f;
	this->RESOLUTION_WIDTH = sf::VideoMode::getDesktopMode().width;
	this->RESOLUTION_HEIGHT = sf::VideoMode::getDesktopMode().height;
}
Camera::~Camera()
{

}
glm::mat4 Camera::Update(float deltaTime, sf::Window &window)
{
	if (window.hasFocus() && !sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
	{
		cameraSpeed = 0.5 * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			cameraPos.x += (cameraSpeed * cameraFront).x;
			cameraPos.z += (cameraSpeed * cameraFront).z;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			cameraPos.x -= (cameraSpeed * cameraFront).x;
			cameraPos.z -= (cameraSpeed * cameraFront).z;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (firstMouse)
		{
			sf::Mouse::setPosition(sf::Vector2i(RESOLUTION_WIDTH / 2, RESOLUTION_HEIGHT / 2));
			oldMouseX = sf::Mouse::getPosition().x;
			oldMouseY = sf::Mouse::getPosition().y;
			firstMouse = false;
		}
		if (sf::Mouse::getPosition().x != RESOLUTION_WIDTH / 2)
		{
			cameraYaw += (float)(sf::Mouse::getPosition().x - oldMouseX) * mouseSensitivity;
		}
		if (sf::Mouse::getPosition().y != RESOLUTION_HEIGHT / 2)
		{
			cameraPitch += (float)(oldMouseY - sf::Mouse::getPosition().y) * mouseSensitivity;
		}
		if (cameraPitch > 89.0f)
		{
			cameraPitch = 89.0f;
		}
		if (cameraPitch < -89.0f)
		{
			cameraPitch = -89.0f;
		}
		glm::vec3 front;
		front.x = cos(glm::radians(cameraPitch)) * cos(glm::radians(cameraYaw));
		front.y = sin(glm::radians(cameraPitch));
		front.z = cos(glm::radians(cameraPitch)) * sin(glm::radians(cameraYaw));
		cameraFront = glm::normalize(front);
		sf::Mouse::setPosition(sf::Vector2i(RESOLUTION_WIDTH / 2, RESOLUTION_HEIGHT / 2));
		oldMouseX = sf::Mouse::getPosition().x;
		oldMouseY = sf::Mouse::getPosition().y;
	}
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}
glm::vec3 Camera::getCameraPos()
{
	return this->cameraPos;
}
void Camera::cameraFall(float terrainY, float scale, float dt)
{
	float cameraOffset = 0.2f;
	float fallSpeed = 0.8f;
	if (cameraPos.y-cameraOffset > terrainY*scale)
	{
		cameraPos.y = cameraPos.y - (fallSpeed*dt);
	}
	else
	{
		cameraPos.y = (terrainY*scale)+cameraOffset-fallSpeed*dt;
	}
}
int Camera::mousePicking(sf::Window &window, glm::mat4 &projectionMatrix, glm::mat4 &viewMatrix, std::vector<Model*> &allModels)
{
	int closestModel = -1;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		float x, y, z;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			//Get mouse pos
			x = (2.0f * sf::Mouse::getPosition(window).x / window.getSize().x - 1.0f);
			y = 1.0f - (2.0f * sf::Mouse::getPosition(window).y / window.getSize().y);
			z = 1.0f;
		}
		else
		{
			//Get middle of windows position
			x = (2.0f * (window.getSize().x / 2)) / window.getSize().x - 1.0f;
			y = 1.0f - (2.0f * (window.getSize().y / 2) / window.getSize().y);
			z = 1.0f;
		}
		glm::vec3 rayNDCSpace = glm::vec3(x, y, z);
		//Convert to clip space
		glm::vec4 rayClipSpace = glm::vec4(rayNDCSpace.x, rayNDCSpace.y, -1.0, 1.0);
		//Convert to eye space
		glm::vec4 rayEyeSpace = glm::inverse(projectionMatrix) * rayClipSpace;
		rayEyeSpace = glm::vec4(rayEyeSpace.x, rayEyeSpace.y, -1.0, 0.0);
		//Convert to world space
		glm::vec3 rayWorldSpace = glm::vec3((glm::inverse(viewMatrix)*rayEyeSpace));

		float distance;
		float clostestDistance = -1;

		for (int i = 0; i < allModels.size(); i++)
		{
			glm::mat4 ModelMatrix = allModels[i]->getModelMatrix();
			glm::vec3 scaleVec(ModelMatrix[0][0], ModelMatrix[1][1], ModelMatrix[2][2]);
			float scaleValue = sqrt(ModelMatrix[0][0] * ModelMatrix[0][0] + ModelMatrix[1][0] * ModelMatrix[1][0] + ModelMatrix[2][0] * ModelMatrix[2][0]);
			glm::vec3 minPos = allModels[i]->getMinBounding()*scaleValue;
			glm::vec3 maxPos = allModels[i]->getMaxBounding()*scaleValue;
			
			if (testIntersection(cameraPos, rayWorldSpace, minPos, maxPos, ModelMatrix, distance))
			{
				if (closestModel == -1)
				{
					closestModel = i;
					clostestDistance = distance;
				}
				else if (distance < clostestDistance)
				{
					closestModel = i;
				}
			}
		}
	}
	return closestModel;
}
bool Camera::testIntersection( glm::vec3 rayOrigin, glm::vec3 rayDirection, glm::vec3 obbMin, glm::vec3 obbMax, glm::mat4 modelMatrix, float& intersection_distance)
{	
	float tMin = 0.0f;
	float tMax = 100000.0f;
	glm::vec3 OBBposition_worldspace(modelMatrix[3]);
	glm::vec3 delta = OBBposition_worldspace - rayOrigin;

	// Test x intersection
	{
		glm::vec3 xaxis(modelMatrix[0]);
		float e = glm::dot(xaxis, delta);
		float f = glm::dot(rayDirection, xaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + obbMin.x) / f;
			float t2 = (e + obbMax.x) / f;

			if (t1>t2) {
				float w = t1;
				t1 = t2;
				t2 = w;
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;
			// Check if there is a intersection, if no return false
			if (tMax < tMin)
				return false;

		}
		else { 
			if (-e + obbMin.x > 0.0f || -e + obbMax.x < 0.0f)
				return false;
		}
	}

	// Test y intersections
	{
		glm::vec3 yaxis(modelMatrix[1]);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(rayDirection, yaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + obbMin.y) / f;
			float t2 = (e + obbMax.y) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + obbMin.y > 0.0f || -e + obbMax.y < 0.0f)
				return false;
		}
	}
	// Test z intersections
	{
		glm::vec3 zaxis(modelMatrix[2]);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(rayDirection, zaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + obbMin.z) / f;
			float t2 = (e + obbMax.z) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + obbMin.z > 0.0f || -e + obbMax.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;
	
}