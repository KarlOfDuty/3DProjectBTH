#ifndef FRUSTUMCULLING_H
#define FRUSTUMCULLING_H
#define PI 3.14159265358979323846
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
struct Plane
{
	//Points
	glm::vec3 pointInPlane; //Center in the near and far planes but the camera position in the others
	//Vectors
	glm::vec3 normal; //Always points inwards
	float width, height;
	float getDistanceTo(glm::vec3 &point);
};
class FrustumCulling
{
private:

public:

	enum { FAR, NEAR, RIGHT, LEFT, TOP, BOTTOM };
	//Planes in the order of the enum above
	std::vector<Plane> planes;

	float nearDistance, farDistance;
	float aspectRatio, fovAngle;

	void setFrustumShape(float fovAngle, float aspectRatio, float nearDistance, float farDistance);
	void setFrustumPlanes(glm::vec3 &cameraPos, glm::vec3 &cameraForward, glm::vec3 &cameraUp);
	int pointInFrustum(glm::vec3 &point);
	int sphereInFrustum(glm::vec3 &centerPoint, float radius);
	FrustumCulling();
	~FrustumCulling();
};
#endif