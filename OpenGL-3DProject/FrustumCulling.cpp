#include "FrustumCulling.h"
//Distance to a point from the plane
float Plane::getDistanceTo(glm::vec3 &point)
{
	//|P->A * N|/|N|
	return glm::abs(glm::dot(this->pointInPlane - point,this->normal))/this->normal.length();
}
//True if a model is inside or intersecting the quadrant
bool Node::intersectsQuadrant(Model *model, glm::vec4 quad)
{
	float radius = model->boundingSphereRadius;
	//Because all models totally have their center in their pivot point
	glm::vec3 modelCenter = glm::vec3(model->getModelMatrix()[3]);
	//TODO: Fix nested if statements after testing is done
	if (quad[XMIN] - radius <= modelCenter.x)
	{
		//std::cout << "XMIN: TRUE" << std::endl;
		if (quad[XMAX] + radius >= modelCenter.x)
		{
			//std::cout << "XMAX: TRUE" << std::endl;
			if (quad[ZMIN] - radius <= modelCenter.z)
			{
				//std::cout << "ZMIN: TRUE" << std::endl;
				if (quad[ZMAX] + radius >= modelCenter.z)
				{
					//std::cout << "ZMAX: TRUE" << std::endl;
					return true;
				}
			}
		}
	}
	return false;
}
//Models are all models that can be in this quadrant, level is which level in the quadtree the node is in, quad vector are the bounds of the quadrant
void Node::buildQuadTree(std::vector<Model*> models, int level, glm::vec4 quad)
{
	this->quad = quad;
	//Check which models are inside of intersecting this quadrant
	std::vector<Model*> foundModels;
	for (int i = 0; i < models.size(); i++)
	{
		if (intersectsQuadrant(models[i],quad))
		{
			//std::cout << "FOUND: TRUE. LEVEL: " << level << ". X bounds: " << quad.x << " " << quad.z << std::endl;
			foundModels.push_back(models[i]);
		}
		else
		{
			//std::cout << "FOUND: FALSE. LEVEL: " << level << ". X bounds: " << quad.x << " " << quad.z << std::endl;
		}
	}
	//Check if leaf and if it contains any models
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
		northEast->buildQuadTree(foundModels, level+1, nextQuad);

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
		this->northEast = nullptr;
		this->southEast = nullptr;
		this->southWest = nullptr;
		this->northWest = nullptr;
		this->hasContents = false;
	}
	else
	{
		this->northEast = nullptr;
		this->southEast = nullptr;
		this->southWest = nullptr;
		this->northWest = nullptr;
		this->models = foundModels;
		this->hasContents = true;
	}
}
//Deletes all empty nodes
void Node::cleanTree()
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
			std::cout << "Empty node deleted." << std::endl;
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
			std::cout << "Empty node deleted." << std::endl;
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
			std::cout << "Empty node deleted." << std::endl;
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
			std::cout << "Empty node deleted." << std::endl;
		}
	}
}

std::vector<Model*> Node::getModelsToDraw()
{
	std::vector<Model*> foundModels;
	if (this->models.empty())
	{
		if (this->northEast != nullptr)
		{
			//Gather all models from this branch
			std::vector<Model*> tempVector = northEast->getModelsToDraw();
			for (int i = 0; i < tempVector.size(); i++)
			{
				//Only add the model if it does not already exist
				if (std::find(foundModels.begin(), foundModels.end(), tempVector[i]) != foundModels.end())
				{
					foundModels.push_back(tempVector[i]);
				}
			}
		}
		if (this->southEast != nullptr)
		{
			//Gather all models from this branch
			std::vector<Model*> tempVector = southEast->getModelsToDraw();
			for (int i = 0; i < tempVector.size(); i++)
			{
				//Only add the model if it does not already exist
				if (std::find(foundModels.begin(), foundModels.end(), tempVector[i]) != foundModels.end())
				{
					foundModels.push_back(tempVector[i]);
				}
			}
		}
		if (this->southWest != nullptr)
		{
			//Gather all models from this branch
			std::vector<Model*> tempVector = southWest->getModelsToDraw();
			for (int i = 0; i < tempVector.size(); i++)
			{
				//Only add the model if it does not already exist
				if (std::find(foundModels.begin(), foundModels.end(), tempVector[i]) != foundModels.end())
				{
					foundModels.push_back(tempVector[i]);
				}
			}
		}
		if (this->northWest != nullptr)
		{
			//Gather all models from this branch
			std::vector<Model*> tempVector = northWest->getModelsToDraw();
			for (int i = 0; i < tempVector.size(); i++)
			{
				//Only add the model if it does not already exist
				if (std::find(foundModels.begin(), foundModels.end(), tempVector[i]) != foundModels.end())
				{
					foundModels.push_back(tempVector[i]);
				}
			}
		}
	}
	else
	{
		foundModels = this->models;
	}
	return foundModels;
}

Node::Node()
{
	//Variables set when buildQuadTree() is called
}
//Sets up the camera
void FrustumCulling::setFrustumShape(float fovAngle, float aspectRatio, float nearDistance, float farDistance)
{
	this->aspectRatio = aspectRatio;
	this->fovAngle = fovAngle;
	//Near plane
	this->nearDistance = nearDistance;
	this->planes[NEAR_P].height = nearDistance * tan(fovAngle * PI / 180 * 0.5);
	this->planes[NEAR_P].width = planes[NEAR_P].height * aspectRatio;
	//Far plane
	this->farDistance = farDistance;
	this->planes[FAR_P].height = farDistance * tan(fovAngle * PI / 180 * 0.5);
	this->planes[FAR_P].width = planes[FAR_P].height * aspectRatio;
}
//Sets the frustum planes used for culling, has to be called after setFrustumShape()
void FrustumCulling::setFrustumPlanes(glm::vec3 &cameraPos, glm::vec3 &cameraForward, glm::vec3 &cameraUp)
{
	///All calculations in world space
	//Make sure base vectors are normalised
	cameraForward = glm::normalize(cameraForward);
	cameraUp = glm::normalize(cameraUp);
	//A vector perpendicular to the up and forward vectors i.e, going straight to the right from the camera's POV
	glm::vec3 cameraRight = glm::cross(cameraUp,cameraForward);

	//Calculates the center point and normal of the far plane
	this->planes[FAR_P].pointInPlane = cameraPos + cameraForward * farDistance;
	this->planes[FAR_P].normal = -cameraForward;

	//Calculates the center point and normal of the near plane
	this->planes[NEAR_P].pointInPlane = cameraPos + cameraForward * nearDistance;
	this->planes[NEAR_P].normal = cameraForward;

	//Calculate a normal for each of the other planes. 
	//They all have a point in the camera position, so no calculation needed for it.
	//The vectors to the sides are from the camera to the side of the near plane
	glm::vec3 halfWidth = cameraRight * planes[NEAR_P].width / 2.0f;
	glm::vec3 halfHeight = cameraUp * planes[NEAR_P].height / 2.0f;

	//Right plane
	glm::vec3 vectorToRightSide = planes[NEAR_P].pointInPlane + halfWidth - cameraPos;
	vectorToRightSide = glm::normalize(vectorToRightSide);
	this->planes[RIGHT_P].normal = cross(cameraUp, vectorToRightSide);
	this->planes[RIGHT_P].pointInPlane = cameraPos;

	//Left plane
	glm::vec3 vectorToLeftSide = planes[NEAR_P].pointInPlane - halfWidth - cameraPos;
	vectorToLeftSide = glm::normalize(vectorToLeftSide);
	this->planes[LEFT_P].normal = cross(cameraUp, vectorToLeftSide);
	this->planes[LEFT_P].pointInPlane = cameraPos;

	//Top plane
	glm::vec3 vectorToTopSide = planes[NEAR_P].pointInPlane + halfHeight - cameraPos;
	vectorToTopSide = glm::normalize(vectorToTopSide);
	this->planes[TOP_P].normal = cross(cameraRight, vectorToTopSide);
	this->planes[TOP_P].pointInPlane = cameraPos;

	//Bottom plane
	glm::vec3 vectorToBottomSide = planes[NEAR_P].pointInPlane - halfHeight - cameraPos;
	vectorToBottomSide = glm::normalize(vectorToBottomSide);
	this->planes[BOTTOM_P].normal = cross(cameraRight, vectorToBottomSide);
	this->planes[BOTTOM_P].pointInPlane = cameraPos;
}
//Quad is in 2d, x and z coordinates. Holds two corners diagonal to eachother
bool FrustumCulling::boxInFrustum(glm::vec4 quad) 
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
	for (int i = 0; i < 6; i++) 
	{
		out = 0; in = 0;
		for (int j = 0; j < points.size() && (in == 0 || out == 0); j++)
		{
			//Check if the corner is inside or outside
			if (planes[i].getDistanceTo(points[j]) < 0)
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

Node* FrustumCulling::getRoot()
{
	return root;
}

FrustumCulling::FrustumCulling()
{
	//setFrustumShape(), setFrustumPlanes() and buildQuadTree() are used to set up the object
	root = new Node();
}

FrustumCulling::~FrustumCulling()
{

}