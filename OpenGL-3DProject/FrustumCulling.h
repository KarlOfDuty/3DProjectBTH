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
static const int quadTreeLevels = 4;
static enum { XMIN, ZMIN, XMAX, ZMAX };
class Node
{
private:
	//Child quadrants, north is negative Z
	Node *northEast;
	Node *southEast;
	Node *southWest;
	Node *northWest;
	//Only leafs have data
	std::vector<Model*> models;
	//True if it contains data or nodes containing data somewhere down the line
	bool hasContents;
	static bool intersectsQuadrant(Model *model, glm::vec4 quad);
public:
	Node();	
	void buildQuadTree(std::vector<Model*> models, int level, glm::vec4 quad);
};

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
	//Root node of the quadtree
	Node *root;
public:
	static enum { FAR_P, NEAR_P, RIGHT_P, LEFT_P, TOP_P, BOTTOM_P };
	void setFrustumShape(float fovAngle, float aspectRatio, float nearDistance, float farDistance);
	void setFrustumPlanes(glm::vec3 &cameraPos, glm::vec3 &cameraForward, glm::vec3 &cameraUp);
	Node* getRoot();
	FrustumCulling();
	~FrustumCulling();
};
#endif