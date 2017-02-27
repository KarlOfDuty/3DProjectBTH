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
glm::mat4 Camera::Update(float deltaTime, bool inFocus)
{
	if (inFocus && !sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
	{
		cameraSpeed = 5 * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			cameraPos += cameraSpeed * cameraFront;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			cameraPos -= cameraSpeed * cameraFront;
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