#include "FrustumCulling.h"

//Sets up the camera
void FrustumCulling::setFrustumShape(float fovAngle, float aspectRatio, float nearDistance, float farDistance)
{
	this->aspectRatio = aspectRatio;
	this->fovAngle = fovAngle;

	this->tang = (float)tan(fovAngle* PI/180 * 0.5);
	//Near plane
	this->nearDistance = nearDistance;
	this->nearPlane.height = nearDistance * tan(fovAngle * PI / 180 * 0.5);
	this->nearPlane.width = nearPlane.height * aspectRatio;
	//Far plane
	this->farDistance = farDistance;
	this->farPlane.height = farDistance * tan(fovAngle * PI / 180 * 0.5);
	this->farPlane.width = farPlane.height * aspectRatio;
}

void FrustumCulling::setCameraVariables(glm::vec3 &cameraPos, glm::vec3 &cameraForward, glm::vec3 &cameraUp)
{
	///All calculations in world space
	//Make sure base vectors are normalised
	cameraForward = glm::normalize(cameraForward);
	cameraUp = glm::normalize(cameraUp);
	//A vector perpendicular to the up and forward vectors i.e, going straight to the right from the camera's POV
	glm::vec3 cameraRight = glm::cross(cameraUp,cameraForward);

	//Calculates the center and normal of the far plane
	farPlane.center = cameraPos + cameraForward * farDistance;
	farPlane.normal = -cameraForward;


	//Calculates three corners of the plane
	//farPlane.topRight = farPlane.center + (cameraRight * farPlane.width / 2.0f) + (cameraUp * farPlane.height / 2.0f);
	//farPlane.topLeft = farPlane.topRight - (cameraRight * farPlane.width);
	//farPlane.bottomLeft = farPlane.topLeft - (cameraUp * farPlane.height);

	//Calculates the center and normal of the near plane
	nearPlane.center = cameraPos + cameraForward * nearDistance;
	nearPlane.normal = cameraForward;
	//Calculate a point in the plane and normal for the other planes.
	//tempVector is a vector from the camera position to the appropriate side of the near plane

	//Right plane
	glm::vec3 tempVector = nearPlane.center + (cameraRight * nearPlane.width / 2.0f) - cameraPos;
	tempVector = glm::normalize(tempVector);
	rightPlane.normal = cross(cameraUp, tempVector);
	//Left plane
	tempVector = nearPlane.center - (cameraRight * nearPlane.width / 2.0f) - cameraPos;
	tempVector = glm::normalize(tempVector);
	leftPlane.normal = cross(cameraUp, tempVector);
	//Top plane
	tempVector = nearPlane.center + (cameraUp * nearPlane.height / 2.0f) - cameraPos;
	tempVector = glm::normalize(tempVector);
	rightPlane.normal = cross(cameraRight, tempVector);
	//Bottom plane
	tempVector = nearPlane.center - (cameraUp * nearPlane.width / 2.0f) - cameraPos;
	tempVector = glm::normalize(tempVector);
	rightPlane.normal = cross(cameraRight, tempVector);

	//Calculates three corners of the plane
	//nearPlane.topRight = nearPlane.center + (cameraRight * nearPlane.width / 2.0f) + (cameraUp * nearPlane.height / 2.0f);
	//nearPlane.topLeft = nearPlane.topRight - (cameraRight * nearPlane.width);
	//nearPlane.bottomLeft = nearPlane.topLeft - (cameraUp * nearPlane.height);
}

FrustumCulling::FrustumCulling()
{

}

FrustumCulling::~FrustumCulling()
{

}
