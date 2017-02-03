#include "Model.h"

std::vector<glm::vec3> Model::getVerticies() const
{
	return verticies;
}

std::vector<glm::vec3> Model::getColours() const
{
	return colours;
}

std::vector<glm::vec3> Model::getNormals() const
{
	return normals;
}

Model::Model()
{

}

Model::~Model()
{

}
