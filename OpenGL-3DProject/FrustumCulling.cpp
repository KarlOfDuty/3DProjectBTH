#include "FrustumCulling.h"
//Distance to a point from the plane
float Plane::getSignedDistanceTo(const glm::vec3 &point) const
{
	//Q->P * N
	return glm::dot(point - this->pointInPlane,this->normal);
}
//True if a model is inside or intersecting the quadrant
bool FrustumCulling::Node::intersectsQuadrant(Model *model, glm::vec4 quad)
{
	//Get the radius for it's bounding sphere
	float radius = model->getBoundingSphereRadius();
	//Get model center point
	glm::vec3 modelCenter = glm::vec3(model->getModelMatrix()[3]);
	//Check if any part of the sphere is inside of the quad
	if (quad[XMIN] <= modelCenter.x + radius && quad[XMAX] >= modelCenter.x - radius && quad[ZMIN] <= modelCenter.z + radius && quad[ZMAX] >= modelCenter.z - radius)
	{
		return true;
	}
	return false;
}
//Models are all models that can be in this quadrant, level is which level in the quadtree the node is in, quad vector are the bounds of the quadrant
void FrustumCulling::Node::buildQuadTree(std::vector<Model*> models, int level, glm::vec4 quad)
{
	this->quad = quad;
	//Check which models are inside of or intersecting this quadrant
	std::vector<Model*> foundModels;
	for (int i = 0; i < models.size(); i++)
	{
		if (intersectsQuadrant(models[i],quad))
		{
			foundModels.push_back(models[i]);
		}
	}
	//Check if the node is a leaf and if it contains any models
	if (level < quadTreeLevels && !foundModels.empty())
	{
		glm::vec4 nextQuad = glm::vec4();

		//Calculate node's bounds
		nextQuad[XMIN] = quad[XMIN] + (quad[XMAX] - quad[XMIN]) / 2;
		nextQuad[XMAX] = quad[XMAX];
		nextQuad[ZMIN] = quad[ZMIN];
		nextQuad[ZMAX] = quad[ZMAX] - (quad[ZMAX] - quad[ZMIN]) / 2;
		//Create the node
		this->northEast = new Node();
		northEast->buildQuadTree(foundModels, level + 1, nextQuad);

		//Calculate node's bounds
		nextQuad[XMIN] = quad[XMIN] + (quad[XMAX] - quad[XMIN]) / 2;
		nextQuad[XMAX] = quad[XMAX];
		nextQuad[ZMIN] = quad[ZMIN] + (quad[ZMAX] - quad[ZMIN]) / 2;
		nextQuad[ZMAX] = quad[ZMAX];
		//Create the node
		this->southEast = new Node();
		southEast->buildQuadTree(foundModels, level + 1, nextQuad);

		//Calculate node's bounds
		nextQuad[XMIN] = quad[XMIN];
		nextQuad[XMAX] = quad[XMAX] - (quad[XMAX] - quad[XMIN]) / 2;
		nextQuad[ZMIN] = quad[ZMIN] + (quad[ZMAX] - quad[ZMIN]) / 2;
		nextQuad[ZMAX] = quad[ZMAX];
		//Create the node
		this->southWest = new Node();
		southWest->buildQuadTree(foundModels, level + 1, nextQuad);

		//Calculate node's bounds
		nextQuad[XMIN] = quad[XMIN];
		nextQuad[XMAX] = quad[XMAX] - (quad[XMAX] - quad[XMIN]) / 2;
		nextQuad[ZMIN] = quad[ZMIN];
		nextQuad[ZMAX] = quad[ZMAX] - (quad[ZMAX] - quad[ZMIN]) / 2;
		//Create the node
		this->northWest = new Node();
		northWest->buildQuadTree(foundModels, level + 1, nextQuad);

		//Don't delete this node in cleanup
		this->hasContents = true;
	}
	else if (foundModels.empty())
	{
		//This node contains nothing and will be deleted in cleanup
		this->northEast = nullptr;
		this->southEast = nullptr;
		this->southWest = nullptr;
		this->northWest = nullptr;
		this->hasContents = false;
	}
	else
	{
		//This node is a leaf
		this->northEast = nullptr;
		this->southEast = nullptr;
		this->southWest = nullptr;
		this->northWest = nullptr;
		this->models = foundModels;
		this->hasContents = true;
	}
}
//Deletes all empty nodes
void FrustumCulling::Node::cleanTree()
{
	//Clean north east quadrant
	if (northEast != nullptr)
	{
		if (northEast->hasContents)
		{
			northEast->cleanTree();
		}
		else
		{
			northEast->cleanTree();
			delete northEast;
			northEast = nullptr;
		}
	}
	//Clean south east quadrant
	if (southEast != nullptr)
	{
		if (southEast->hasContents)
		{
			southEast->cleanTree();
		}
		else
		{
			southEast->cleanTree();
			delete southEast;
			southEast = nullptr;
		}
	}
	//Clean south west quadrant
	if (southWest != nullptr)
	{
		if (southWest->hasContents)
		{
			southWest->cleanTree();
		}
		else
		{
			southWest->cleanTree();
			delete southWest;
			southWest = nullptr;
		}
	}
	//Clean north west quadrant
	if (northWest != nullptr)
	{
		if (northWest->hasContents)
		{
			northWest->cleanTree();
		}
		else
		{
			northWest->cleanTree();
			delete northWest;
			northWest = nullptr;
		}
	}
}
//Recursive function which gets all models to draw
std::vector<Model*> FrustumCulling::Node::getModelsToDraw(const FrustumCulling &fcObject) const
{
	//Run this function in all child nodes until a leaf is reached, then return the vector of models back upwards, combining it with the adjacent ones
	std::vector<Model*> foundModels;
	if (this->models.empty())
	{
		if (this->northEast != nullptr)
		{
			if (fcObject.boxInFrustum(this->northEast->quad))
			{
				//Gather all models from this branch
				std::vector<Model*> tempVector = northEast->getModelsToDraw(fcObject);
				for (int i = 0; i < tempVector.size(); i++)
				{
					foundModels.push_back(tempVector[i]);
				}
			}
		}
		if (this->southEast != nullptr)
		{
			if (fcObject.boxInFrustum(this->southEast->quad))
			{
				//Gather all models from this branch
				std::vector<Model*> tempVector = southEast->getModelsToDraw(fcObject);
				for (int i = 0; i < tempVector.size(); i++)
				{
					foundModels.push_back(tempVector[i]);
				}
			}
		}
		if (this->southWest != nullptr)
		{
			if (fcObject.boxInFrustum(this->southWest->quad))
			{
				//Gather all models from this branch
				std::vector<Model*> tempVector = southWest->getModelsToDraw(fcObject);
				for (int i = 0; i < tempVector.size(); i++)
				{
					foundModels.push_back(tempVector[i]);
				}
			}
		}
		if (this->northWest != nullptr)
		{
			if (fcObject.boxInFrustum(this->northWest->quad))
			{
				//Gather all models from this branch
				std::vector<Model*> tempVector = northWest->getModelsToDraw(fcObject);
				for (int i = 0; i < tempVector.size(); i++)
				{
					foundModels.push_back(tempVector[i]);
				}
			}
		}
	}
	else
	{
		//This is a leaf node
		foundModels = this->models;
	}
	return foundModels;
}
//Constructors
FrustumCulling::Node::Node()
{
	//Variables set when buildQuadTree() is called
}
//Sets up the projection variables
void FrustumCulling::setFrustumShape(float fovAngle, float aspectRatio, float nearDistance, float farDistance)
{
	this->aspectRatio = aspectRatio;
	this->fovAngle = fovAngle;
	//Near plane
	this->nearDistance = nearDistance;
	this->planes[NEAR_P].height = nearDistance * std::tan(fovAngle * PI / 180);
	this->planes[NEAR_P].width = planes[NEAR_P].height * aspectRatio;
	//Far plane
	this->farDistance = farDistance;
	this->planes[FAR_P].height = farDistance * std::tan(fovAngle * PI / 180);
	this->planes[FAR_P].width = planes[FAR_P].height * aspectRatio;
}
//Sets the frustum planes used for culling, has to be called after setFrustumShape()
void FrustumCulling::setFrustumPlanes(glm::vec3 cameraPos, glm::vec3 cameraForward, glm::vec3 cameraUp)
{
	///All calculations in world space
	//Make sure base vectors are normalised
	cameraForward = glm::normalize(cameraForward);
	cameraUp = glm::normalize(cameraUp);
	//A vector perpendicular to the up and forward vectors i.e, going straight to the right from the camera's POV
	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraForward,cameraUp));
	//Calculates the real up vector for the camera instead of the world's Y
	cameraUp = glm::cross(cameraRight,cameraForward);
	//Calculates the center point and normal of the far plane
	this->planes[FAR_P].pointInPlane = cameraPos + (cameraForward * farDistance);
	this->planes[FAR_P].normal = -cameraForward;

	//Calculates the center point and normal of the near plane
	this->planes[NEAR_P].pointInPlane = cameraPos + (cameraForward * nearDistance);
	this->planes[NEAR_P].normal = cameraForward;

	//Calculate a normal for each of the other planes. 
	//They all have a point in the camera position, so no calculation needed for it.
	//The plane vectors are from the camera position to the side of the far plane specified, toFarPlane is from the cameraPos to the centerof the far plane
	float farHalfWidth = planes[FAR_P].width / 2.0f;
	float farHalfHeight = planes[FAR_P].height / 2.0f;
	glm::vec3 toFarPlane = (cameraForward * farDistance);

	//Right plane
	glm::vec3 planeVector = (cameraRight * farHalfWidth) + toFarPlane;
	this->planes[RIGHT_P].normal = glm::normalize(glm::cross(cameraUp,planeVector));
	this->planes[RIGHT_P].pointInPlane = cameraPos;

	//Left plane
	planeVector = (cameraRight * -farHalfWidth) + toFarPlane;
	this->planes[LEFT_P].normal = glm::normalize(glm::cross(planeVector,cameraUp));
	this->planes[LEFT_P].pointInPlane = cameraPos;

	//Top plane
	planeVector = (cameraUp * farHalfHeight) + toFarPlane;
	this->planes[TOP_P].normal = glm::normalize(glm::cross(cameraRight, planeVector));
	this->planes[TOP_P].pointInPlane = cameraPos;

	//Bottom plane
	planeVector = (cameraUp * -farHalfHeight) + toFarPlane;
	this->planes[BOTTOM_P].normal = glm::normalize(glm::cross(planeVector,cameraRight));
	this->planes[BOTTOM_P].pointInPlane = cameraPos;
}
//Quads are in 2d and only hold two diagonal corners: x,z min and x,z max
bool FrustumCulling::boxInFrustum(const glm::vec4 &quad) const 
{
	//Check which quadrants can be seen from the frustum
	int out;
	int in;
	//Corners of the box
	std::vector<glm::vec3> points;
	points.push_back(glm::vec3(quad[XMIN], mapHeight, quad[ZMIN]));
	points.push_back(glm::vec3(quad[XMIN], mapBottom, quad[ZMIN]));
	points.push_back(glm::vec3(quad[XMAX], mapHeight, quad[ZMAX]));
	points.push_back(glm::vec3(quad[XMAX], mapBottom, quad[ZMAX]));
	points.push_back(glm::vec3(quad[XMIN], mapHeight, quad[ZMAX]));
	points.push_back(glm::vec3(quad[XMIN], mapBottom, quad[ZMAX]));
	points.push_back(glm::vec3(quad[XMAX], mapHeight, quad[ZMIN]));
	points.push_back(glm::vec3(quad[XMAX], mapBottom, quad[ZMIN]));
	//Cycle through planes of the frustum
	for (int i = 0; i < 6; i++) 
	{
		out = 0;
		in = 0;
		for (int j = 0; j < points.size() && (in == 0 || out == 0); j++)
		{
			//Check if the corner is inside or outside
			if (planes[i].getSignedDistanceTo(points[j]) < 0)
			{
				out++;
			}
			else
			{
				in++;
			}
		}
		//If all corners are outside of this plane, it cannot be inside the frustum
		if (in == 0)
		{
			return false;
		}
	}
	return true;
}
//Gets the root of the quadtree
FrustumCulling::Node* FrustumCulling::getRoot()
{
	return root;
}
//Constructors
FrustumCulling::FrustumCulling()
{
	//setFrustumShape(), setFrustumPlanes() and buildQuadTree() are used to set up the object
	root = new Node();
}
//Destructors
FrustumCulling::~FrustumCulling()
{
	//The quadtree isn't deleted as it may be shared with other FrustumCulling objects
}