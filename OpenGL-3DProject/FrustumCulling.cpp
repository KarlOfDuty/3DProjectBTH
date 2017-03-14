#include "FrustumCulling.h"

//Sets up the camera
void FrustumCulling::setFrustumShape(float fovAngle, float aspectRatio, float nearDistance, float farDistance)
{
	this->aspectRatio = aspectRatio;
	this->fovAngle = fovAngle;

	float tang = (float)tan(fovAngle* PI/180 * 0.5);
	//Near plane
	this->nearDistance = nearDistance;
	this->planes[NEAR].height = nearDistance * tan(fovAngle * PI / 180 * 0.5);
	this->planes[NEAR].width = planes[NEAR].height * aspectRatio;
	//Far plane
	this->farDistance = farDistance;
	this->planes[FAR].height = farDistance * tan(fovAngle * PI / 180 * 0.5);
	this->planes[FAR].width = planes[FAR].height * aspectRatio;
}

void FrustumCulling::setFrustumPlanes(glm::vec3 &cameraPos, glm::vec3 &cameraForward, glm::vec3 &cameraUp)
{
	///All calculations in world space
	//Make sure base vectors are normalised
	cameraForward = glm::normalize(cameraForward);
	cameraUp = glm::normalize(cameraUp);
	//A vector perpendicular to the up and forward vectors i.e, going straight to the right from the camera's POV
	glm::vec3 cameraRight = glm::cross(cameraUp,cameraForward);

	//Calculates the center point and normal of the far plane
	this->planes[FAR].pointInPlane = cameraPos + cameraForward * farDistance;
	this->planes[FAR].normal = -cameraForward;

	//Calculates the center point and normal of the near plane
	this->planes[NEAR].pointInPlane = cameraPos + cameraForward * nearDistance;
	this->planes[NEAR].normal = cameraForward;

	//Calculate a normal for each of the other planes. 
	//They all have a point in the camera position, so no calculation needed for it.
	//The vectors to the sides are from the camera to the side of the near plane
	glm::vec3 halfWidth = cameraRight * planes[NEAR].width / 2.0f;
	glm::vec3 halfHeight = cameraUp * planes[NEAR].height / 2.0f;

	//Right plane
	glm::vec3 vectorToRightSide = planes[NEAR].pointInPlane + halfWidth - cameraPos;
	vectorToRightSide = glm::normalize(vectorToRightSide);
	this->planes[RIGHT].normal = cross(cameraUp, vectorToRightSide);
	this->planes[RIGHT].pointInPlane = cameraPos;

	//Left plane
	glm::vec3 vectorToLeftSide = planes[NEAR].pointInPlane - halfWidth - cameraPos;
	vectorToLeftSide = glm::normalize(vectorToLeftSide);
	this->planes[LEFT].normal = cross(cameraUp, vectorToLeftSide);
	this->planes[LEFT].pointInPlane = cameraPos;

	//Top plane
	glm::vec3 vectorToTopSide = planes[NEAR].pointInPlane + halfHeight - cameraPos;
	vectorToTopSide = glm::normalize(vectorToTopSide);
	this->planes[TOP].normal = cross(cameraRight, vectorToTopSide);
	this->planes[TOP].pointInPlane = cameraPos;

	//Bottom plane
	glm::vec3 vectorToBottomSide = planes[NEAR].pointInPlane - halfHeight - cameraPos;
	vectorToBottomSide = glm::normalize(vectorToBottomSide);
	this->planes[BOTTOM].normal = cross(cameraRight, vectorToBottomSide);
	this->planes[BOTTOM].pointInPlane = cameraPos;
}

int FrustumCulling::pointInFrustum(glm::vec3 &point) 
{
	//Returns true if inside or intersecting, false if outside
	for (int i = 0; i < 6; i++) 
	{
		if (planes[i].getDistanceTo(point) < 0)
		{
			return false;
		}
	}
	return true;
}

int FrustumCulling::sphereInFrustum(glm::vec3 &centerPoint, float radius)
{

	float distance;
	for (int i = 0; i < 6; i++) 
	{
		distance = planes[i].getDistanceTo(centerPoint);
		if (distance < -radius)
		{
			return false;
		}
	}
	return true;
}

FrustumCulling::FrustumCulling()
{
	//Use the setFrustumShape() and the setFrustumPlanes() functions to set up the object
}

FrustumCulling::~FrustumCulling()
{

}

float Plane::getDistanceTo(glm::vec3 &point)
{
	//|P->A * N|/|N|
	return glm::abs(glm::dot(this->pointInPlane - point,this->normal))/this->normal.length;;
}
