#ifndef FRUSTUMCULLING_H
#define FRUSTUMCULLING_H
#define PI 3.14159265358979323846
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Model.h"
struct Plane
{
	//Points
	glm::vec3 pointInPlane; //Center in the near and far planes but the camera position in the others
	//Vectors
	glm::vec3 normal; //Always points inwards
	float width, height;
	float getDistanceTo(glm::vec3 &point);
};
class Node
{
	//Child quadrants
	Node *northEast;
	Node *southEast;
	Node *southWest;
	Node *northWest;
	bool hasContents;
	//Only leafs have data
	std::vector<Model*> models;
	void buildQuadTree(std::vector<Model*> models, int level, glm::vec2 xMinMax, glm::vec2 zMinMax);
};
static const int quadTreeLevels = 4;
class FrustumCulling
{
private:
	//Planes in the order of the enum below
	std::vector<Plane> planes;
	//Frustum varibles
	float nearDistance;
	float farDistance;
	//Camera variables
	float aspectRatio;
	float fovAngle;
	//
	Node *root;
public:
	static enum { MIN, MAX };
	static enum { FAR, NEAR, RIGHT, LEFT, TOP, BOTTOM };
	void setFrustumShape(float fovAngle, float aspectRatio, float nearDistance, float farDistance);
	void setFrustumPlanes(glm::vec3 &cameraPos, glm::vec3 &cameraForward, glm::vec3 &cameraUp);
	int pointInFrustum(glm::vec3 &point);
	int sphereInFrustum(glm::vec3 &centerPoint, float radius);
	FrustumCulling();
	~FrustumCulling();
};
#endif