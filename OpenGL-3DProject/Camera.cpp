#include "Camera.h"
Camera::Camera()
{
	//True for first run only, to add default values
	this->firstMouse = true;
	this->oldMouseX = RESOLUTION_WIDTH / 2;
	this->oldMouseY = RESOLUTION_HEIGHT / 2;
	this->cameraHasMoved = true;
	this->mouseSensitivity = 0.05f;
	this->cameraSpeed = 0.05f;
	this->cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
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
void Camera::frustumCulling(FrustumCulling &fcObject, std::vector<Model*> &visibleModels)
{
	//If the camera has moved, update the visible models
	if (cameraHasMoved)
	{
		fcObject.setFrustumPlanes(cameraPos, cameraFront, cameraUp);
		visibleModels = fcObject.getRoot()->getModelsToDraw(fcObject);
		//Remove duplicate pointers
		std::sort(visibleModels.begin(), visibleModels.end());
		visibleModels.erase(std::unique(visibleModels.begin(), visibleModels.end()), visibleModels.end());
	}
}

glm::mat4 Camera::Update(float deltaTime, sf::Window &window)
{
	cameraHasMoved = false;
	if (window.hasFocus() && !sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
	{
		//Movement
		cameraSpeed = 5 * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			cameraPos += cameraSpeed * cameraFront;
			cameraHasMoved = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			cameraPos -= cameraSpeed * cameraFront;
			cameraHasMoved = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			cameraHasMoved = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
			cameraHasMoved = true;
		}
		//Setup
		if (firstMouse)
		{
			sf::Mouse::setPosition(sf::Vector2i(RESOLUTION_WIDTH / 2, RESOLUTION_HEIGHT / 2));
			oldMouseX = sf::Mouse::getPosition().x;
			oldMouseY = sf::Mouse::getPosition().y;
			//std::cout << "CameraPos(" << cameraPos.x << "," << cameraPos.y << "," << cameraPos.z << "). cameraFront(" << cameraFront.x << "," << cameraFront.y << "," << cameraFront.z << "). cameraUp(" << cameraUp.x << "," << cameraUp.y << "," << cameraUp.z << ")." << std::endl;
			firstMouse = false;
		}
		//Rotate camera
		if (sf::Mouse::getPosition().x != RESOLUTION_WIDTH / 2)
		{
			cameraYaw += (float)(sf::Mouse::getPosition().x - oldMouseX) * mouseSensitivity;
			cameraHasMoved = true;
		}
		if (sf::Mouse::getPosition().y != RESOLUTION_HEIGHT / 2)
		{
			cameraPitch += (float)(oldMouseY - sf::Mouse::getPosition().y) * mouseSensitivity;
			cameraHasMoved = true;
		}
		//Make sure the lowest and highest angles the camera can be turned to are -89 and 89 degrees
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
		glm::vec3 ray_nds = glm::vec3(x, y, z);
		//Convert to 
		glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
		//Convert to eye space
		glm::vec4 ray_eye = glm::inverse(projectionMatrix) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
		//Convert to world space
		glm::vec3 ray_wor = glm::vec3((glm::inverse(viewMatrix)*ray_eye));

		//Set max and miniumum boxes from positions, max distance from camera to test intersection
		glm::vec3 aabb_min(-0.5f, -0.5f, -0.5f);
		glm::vec3 aabb_max(0.5f, 0.5f, 0.5f);
		float distance = 100;

		for (int i = 0; i < allModels.size(); i++)
		{
			glm::mat4 ModelMatrix = allModels[i]->getModelMatrix();
			ModelMatrix[0][0] = 1.0f;
			ModelMatrix[1][1] = 1.0f;
			ModelMatrix[2][2] = 1.0f;
			//ModelMatrix *= allModels[i].getRotationMatrix();
			glm::vec3 minPos = allModels[i]->getMinBounding();
			glm::vec3 maxPos = allModels[i]->getMaxBouding();
			
			if (testIntersection(cameraPos, ray_wor, minPos, maxPos, ModelMatrix, distance))
			{
				if (closestModel == -1)
				{
					closestModel = i;
				}
				else
				{
					//Coordinates for the closest model
					glm::mat4 closestModelMatrix(allModels[closestModel]->getModelMatrix());
					glm::vec3 closestModelPos(closestModelMatrix[3].x, closestModelMatrix[3].y, closestModelMatrix[3].z);
					//Coordinates for new model
					glm::vec3 modelPos(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);
					//Check if new model is closer, if true then its the new closest
					if (glm::distance(cameraPos, closestModelPos) > glm::distance(cameraPos, modelPos))
					{
						closestModel = i;
					}
				}
			}
		}
	}
	return closestModel;
}
bool Camera::testIntersection( glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 aabb_min, glm::vec3 aabb_max, glm::mat4 ModelMatrix, float& intersection_distance)

{
	// Intersection method from Real-Time Rendering and Essential Mathematics for Games

	float tMin = 0.0f;
	float tMax = 100000.0f;

	glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

	glm::vec3 delta = OBBposition_worldspace - ray_origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
		float e = glm::dot(xaxis, delta);
		float f = glm::dot(ray_direction, xaxis);

		if (fabs(f) > 0.001f) { // Standard case

			float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
			float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
											 // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

											 // We want t1 to represent the nearest intersection,
											 // so if it's not the case, invert t1 and t2
			if (t1>t2) {
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin)
				return false;

		}
		else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(ray_direction, yaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.y) / f;
			float t2 = (e + aabb_max.y) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(ray_direction, zaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.z) / f;
			float t2 = (e + aabb_max.z) / f;

			if (t1>t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;
}