#ifndef FRUSTUMCULLING_H
#define FRUSTUMCULLING_H
#define PI 3.14159265358979323846
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
class FrustumCulling
{
private:
	enum 
	{
		TOP = 0, BOTTOM, LEFT,
		RIGHT, NEARP, FARP
	};
public:
	static enum { OUTSIDE, INTERSECT, INSIDE };
	//Plane pl[6];
	struct Plane
	{
		//Points
		//glm::vec3 topLeft, topRight, bottomLeft, center;
		glm::vec3 center; //Center in the near and far planes but the camera position in the others
		//Vectors
		glm::vec3 normal; //Always points inwards
		float width, height;
	};
	//Planes used for culling
	Plane nearPlane;
	Plane farPlane;
	Plane topPlane;
	Plane leftPlane;
	Plane rightPlane;
	Plane bottomPlane;

	float nearDistance, farDistance;
	float aspectRatio, fovAngle, tang;

	void setFrustumShape(float fovAngle, float aspectRatio, float nearDistance, float farDistance);
	void setCameraVariables(glm::vec3 &cameraPos, glm::vec3 &cameraForward, glm::vec3 &cameraUp);
	//int pointInFrustum(glm::vec3 &p);
	//int sphereInFrustum(glm::vec3 &p, float raio);
	//int boxInFrustum(AABox &b);
	FrustumCulling();
	~FrustumCulling();
};
#endif